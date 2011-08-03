#include "cbir.h"

CBIR::CBIR(){
    clustersMat.data = NULL;    

    //Creo una Mat de features a partir de la matriz.
    //descriptorsMat = cv::Mat(12, DESCRIPTOR_DIMS, CV_32F, f);

}

void CBIR::buildIndex(QString path){
    this->buildDescriptorsMatrix(path);
    this->computeClusters();
    this->buildClustersIndex();
}

void CBIR::buildDescriptorsMatrix(QString path){
    QString rootG = "../img/keepcon2-g/";
    //QString root = "../img/keepcon2/";
    QString root = path;
    QStringList images;
    QStringList fileDirs;

    //Carga todas las imágenes.
    QDir dir(root);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    images << dir.entryList();

    //Carga las imágenes de los grupos indicados.
    /*
    fileDirs << "auto" << "converse" << "doki";
    for(int i=0; i<fileDirs.count(); i++){
        QDir dir(rootG + fileDirs[i] + "/");
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name | QDir::Reversed);
        images << dir.entryList();
    }
    */

    //Carga de imágenes específicas.
    //images << "auto.jpg" << "auto-f.jpg" << "converse.jpg";
    cout << "Cantidad de imagenes: " << images.count() << endl;

    //int length = (int)(imgDescriptors->elem_size/sizeof(float));
    //descriptorsMat = cv::Mat(1106, DESCRIPTOR_DIMS, CV_32F);
    //float* img_ptr = descriptorsMat.ptr<float>(0);
    //CvSeqReader img_reader;

    CvSeq* totalDescriptors = 0;
    CvSeq* nextSeq = 0;

    IplImage* img;
    CvSeq *imgKeypoints, *imgDescriptors;
    CvSURFParams params = cvSURFParams(500, 1);
    //Calcula features para cada imagen
    for(int i=0; i<images.count(); i++){
        img = NULL;
        imgKeypoints = 0;
        imgDescriptors = 0;
        qDebug() << "Intento de carga de la imagen" << images[i] << ".";
        img = Utils::loadImage((root + images[i]).toAscii().data(), true);
        if(img != NULL){
            CvMemStorage* storage = cvCreateMemStorage(0);
            cvExtractSURF(img, 0, &imgKeypoints, &imgDescriptors, storage, params);
            qDebug() << "Imagen" << images[i] << "cargada con exito. Features:" << imgKeypoints->total;
            featuresCount.append(QPair<QString, int>(images[i], imgDescriptors->total));

            //Linkeo la nueva secuencia de descriptores a la lista.
            if(totalDescriptors == 0){
                totalDescriptors = imgDescriptors;
                nextSeq = totalDescriptors;
            }else{
                nextSeq->h_next = imgDescriptors;
                nextSeq = nextSeq->h_next;                
            }
            //Copiar los descriptores a la matriz de features
            /*
            cvStartReadSeq(imgDescriptors, &img_reader);
            for(int j=0; j<imgDescriptors->total; j++){ // j<1 para cargar un solo descriptor por cada imagen (prueba).
                const float* descriptor = (const float*)img_reader.ptr;
                CV_NEXT_SEQ_ELEM(img_reader.seq->elem_size, img_reader);
                memcpy(img_ptr, descriptor, DESCRIPTOR_DIMS*sizeof(float));
                img_ptr += DESCRIPTOR_DIMS;
            }
            */
            //cvReleaseMemStorage(&storage);
            cvReleaseImage(&img);
        }
    }    
    int descriptorsCount = 0;
    int sequencesCount = 0;
    CvSeq* iterateSeq = totalDescriptors;

    while(iterateSeq != nextSeq){
        descriptorsCount += iterateSeq->total;
        iterateSeq = iterateSeq->h_next;
        sequencesCount++;
    }
    if(iterateSeq != 0){
        descriptorsCount += iterateSeq->total;
        sequencesCount++;
    }
    qDebug() << "Total de secuencias:" << sequencesCount;
    qDebug() << "Total de descriptores:" << descriptorsCount;

    //Creo la matriz de descriptores ahora que se conoce la cantidad.
    descriptorsMat = cv::Mat(descriptorsCount, DESCRIPTOR_DIMS, CV_32F);
    float* img_ptr = descriptorsMat.ptr<float>(0);
    CvSeqReader img_reader;

    //Copia los descriptores de la lista de secuencias a la matriz.
    iterateSeq = totalDescriptors;
    CvSeq* deallocateSeq = 0;
    for(int i=0; i<sequencesCount; i++){
        deallocateSeq = iterateSeq;
        cvStartReadSeq(iterateSeq, &img_reader);
        int j;
        for(j=0; j<iterateSeq->total; j++){
            const float* descriptor = (const float*)img_reader.ptr;
            CV_NEXT_SEQ_ELEM(img_reader.seq->elem_size, img_reader);
            memcpy(img_ptr, descriptor, DESCRIPTOR_DIMS*sizeof(float));
            img_ptr += DESCRIPTOR_DIMS;
        }
        iterateSeq = iterateSeq->h_next;
        //Probar que efectivamente se libera la memoria.
        cvReleaseMemStorage(&deallocateSeq->storage);
    }
}

int CBIR::computeClusters(){
    //Creo una Mat para los centros de los clusters con la misma dimensión que la Mat de features.
    if(clustersMat.data) {
        delete[] clustersMat.data;
        clustersMat.data = NULL;
    }
    clustersMat = cv::Mat(descriptorsMat.rows, DESCRIPTOR_DIMS, CV_32F);
    //Inicializo los parámetros para el clustering.
    cv::flann::KMeansIndexParams kmiParams = cv::flann::KMeansIndexParams(10, 15, cv::flann::CENTERS_GONZALES, 0.6);

    int clustersCount = hierarchicalClustering(descriptorsMat, clustersMat, kmiParams);
    qDebug() << "Numero de clusters computados: " << clustersCount;
}

int CBIR::buildClustersIndex(){
    if(clustersMat.data == NULL)
        exit(1);

    //Creo el índice.
//    cv::flann::KDTreeIndexParams kdtiParams(1);
//    cv::Mat savedIndexMat(1, 1, CV_32F);
//    cv::flann::Index clustersIndex = cv::flann::Index(savedIndexMat, kdtiParams);
    cv::flann::Index *clustersIndex;
    qDebug() << "Declaro el indice.";

    QFile indexFile("cbir\\flann\\index\\index.hdf5");
    if(indexFile.exists()){
        cv::flann::SavedIndexParams siParams("cbir\\flann\\index\\index.hdf5");
        clustersIndex = new cv::flann::Index(clustersMat, siParams);
        qDebug() << "Creo el indice a partir del archivo.";
    }else{
        cv::flann::KDTreeIndexParams kdtiParams(8);
        clustersIndex = new cv::flann::Index(clustersMat, kdtiParams);
        clustersIndex->save("cbir\\flann\\index\\index.hdf5");
        qDebug() << "Creo el indice a partir de la matriz.";
    }

    cv::Mat indices(descriptorsMat.rows, 1, CV_32S);
    cv::Mat dists(descriptorsMat.rows, 1, CV_32F);
    //cv::Mat_<float> indices(descriptorsMat.rows, 1);
    //cv::Mat_<int> dists(descriptorsMat.rows, 1);

    //Ubica cada feature en un determinado cluster basándose en Knn-Search
    clustersIndex->knnSearch(descriptorsMat, indices, dists, 1, cv::flann::SearchParams(1024));
    qDebug() << endl << "Indices:" << indices.rows << endl;

    //DOCUMENTO    
    int totalFeatures = 0;
    QFile sourceFile("cbir/lemur/index/index_source");
    sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream sourceStream(&sourceFile);
    QFile trecFile;
    QTextStream trecStream(&trecFile);

    //Itero sobre las imágenes.
    for(int i=0; i<featuresCount.count() && totalFeatures < descriptorsMat.rows; i++){
        trecFile.setFileName("cbir/lemur/files/" + featuresCount[i].first + ".trec");
        if (!trecFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return 1;
        trecStream.flush();

        trecStream << "<DOC>" << endl
                << "<DOCNO>" << featuresCount[i].first << "</DOCNO>" << endl
                << "<TEXT>" << endl;

        // Itero sobre todos los features de la imagen.
        for(int j=0; j<featuresCount[i].second && totalFeatures < descriptorsMat.rows; j++, totalFeatures++){
            trecStream << indices.at<int>(totalFeatures, 0) << endl;
        }
        trecStream << "</TEXT>" << endl
            << "</DOC>" << endl;

        trecFile.close();
        qDebug() << "Archivo guardado:" << trecFile.fileName();

        //Agrego la imagen al archivo index_source
        sourceStream << "e:\\Proyectos\\Git\\keepc\\release\\cbir\\lemur\\files\\" << featuresCount[i].first + ".trec" << endl;
    }
    sourceFile.close();
}

void CBIR::query(QString path){
    cv::Mat indices = this->getClustersIndices(path);
    //this->setupQuery(path, indices);
}

void CBIR::addImage(QString path){
    cv::Mat indices = this->getClustersIndices(path);
    this->setupAdd(path, indices);
}

cv::Mat CBIR::getClustersIndices(QString path){
    QString imgName = path.section('/', -1);
    IplImage* img = Utils::loadImage(path.toAscii().data(), true);
    if(img == NULL){
        qDebug() << "La imagen no fue cargada.";
        exit(1);
    }
    //Computo los features.
    CvSeq *imgKeypoints, *imgDescriptors;
    CvSURFParams params = cvSURFParams(500, 1);
    imgKeypoints = 0;
    imgDescriptors = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    cvExtractSURF(img, 0, &imgKeypoints, &imgDescriptors, storage, params);
    qDebug() << "Imagen" << path << "cargada con exito. Features:" << imgKeypoints->total;

    //Copio los descriptores a una Mat.
    cv::Mat queryDescriptorsMat(imgDescriptors->total, DESCRIPTOR_DIMS, CV_32F);
    float* img_ptr = queryDescriptorsMat.ptr<float>(0);
    CvSeqReader img_reader;
    cvStartReadSeq(imgDescriptors, &img_reader);
    for(int j=0; j<imgDescriptors->total; j++){
        const float* descriptor = (const float*)img_reader.ptr;
        CV_NEXT_SEQ_ELEM(img_reader.seq->elem_size, img_reader);
        memcpy(img_ptr, descriptor, DESCRIPTOR_DIMS*sizeof(float));
        img_ptr += DESCRIPTOR_DIMS;
    }
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&img);

    if(clustersMat.data == NULL)
        exit(1);

    //Creo el índice para los cluster centers.
    cv::flann::KDTreeIndexParams kdtiParams = cv::flann::KDTreeIndexParams(8);
    cv::flann::Index clustersIndex(clustersMat, kdtiParams);

    //Clusterizo cada feature de la query según Knn-Search.
    cv::Mat indices(queryDescriptorsMat.rows, 1, CV_32S);
    cv::Mat dists(queryDescriptorsMat.rows, 1, CV_32F);
    clustersIndex.knnSearch(queryDescriptorsMat, indices, dists, 1, cv::flann::SearchParams(1024));

    /***************************************************************************************************************/
    //Guardo el archivo para realizar la query al índice.
    QFile query("cbir/lemur/query/" + imgName + ".query");
    QTextStream stream(&query);
    if (!query.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << "Ocurrio un error al intentar abrir el archivo" + imgName + ".query";
    stream << "<DOC 1>" << endl;

    // Itero sobre todos los features de la imagen.
    for(int i=0; i<queryDescriptorsMat.rows ;i++){
        stream << indices.at<int>(i, 0) << endl;
    }
    stream << "</DOC>";
    query.close();

    //Guardo el archivo con los parámetros de la query.
    QFile qP("cbir/lemur/query/query_params");
    QTextStream qPStream(&qP);
    if (!qP.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << "Ocurrio un error al intentar abrir el archivo query_params";
    qPStream << "<parameters>" << endl <<
                    "<index>e:\\Proyectos\\Git\\keepc\\release\\cbir\\lemur\\index\\index.key</index>" << endl <<
                    "<retModel>tfidf</retModel>" << endl <<
                    "<textQuery>e:\\Proyectos\\Git\\keepc\\release\\cbir\\lemur\\query\\" << imgName << ".query</textQuery>" << endl <<
                    "<resultFile>e:\\Proyectos\\Git\\keepc\\release\\cbir\\lemur\\query\\" << imgName << ".results</resultFile>" << endl <<
                    "<TRECResultFormat>1</TRECResultFormat>" << endl <<
                    "<resultCount>10</resultCount>" << endl <<
                "</parameters>";
    qP.close();

    return indices;
}

void CBIR::setupQuery(QString path, cv::Mat &indices){

}

void CBIR::setupAdd(QString path, cv::Mat &indices){

}

void CBIR::printMatrix(cv::Mat *mat){
    for(int i=0; i<mat->rows; i++){
        for(int j=0; j<DESCRIPTOR_DIMS; j++){
            cout << "[" << i << "," << j << "] " << mat->at<float>(i, j) << endl;
        }
        cout << endl;
    }
}

void CBIR::printDescriptorsMatrix(){
    for(int i=0; i<descriptorsMat.rows; i++){
        for(int j=0; j<DESCRIPTOR_DIMS; j++){
            cout << "[" << i << "," << j << "] " << descriptorsMat.at<float>(i, j) << endl;
        }
        cout << endl;
    }
}

void CBIR::buildLemurIndex(){
    //Parser * parser = NULL;
    //lemur::index::KeyfileIncIndex* index = NULL;
    //index = new lemur::index::KeyfileIncIndex("", 1);
}
