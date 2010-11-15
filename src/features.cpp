#include "features.h"

bool Features::VERBOSE = false;
bool Features::GRAPHIC = false;

void Features::setVerbose(bool verbose)
{
    VERBOSE = verbose;
}

void Features::setGraphic(bool graphic)
{
    GRAPHIC = graphic;
}

double Features::compareSURFDescriptors(const float* d1, const float* d2, double best, int length)
{
    double total_cost = 0;
    assert( length % 4 == 0 );
    for( int i = 0; i < length; i += 4 ){
        double t0 = d1[i] - d2[i];
        double t1 = d1[i+1] - d2[i+1];
        double t2 = d1[i+2] - d2[i+2];
        double t3 = d1[i+3] - d2[i+3];
        total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
        if( total_cost > best )
            break;
    }
    return total_cost;
}

int Features::naiveNearestNeighbor(const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors)
{
    int length = (int)(model_descriptors->elem_size/sizeof(float));
    int i, neighbor = -1;
    double d, dist1 = 1e6, dist2 = 1e6;
    CvSeqReader reader, kreader;
    cvStartReadSeq( model_keypoints, &kreader, 0 );
    cvStartReadSeq( model_descriptors, &reader, 0 );

    for( i = 0; i < model_descriptors->total; i++ ){
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* mvec = (const float*)reader.ptr;
        CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        if( laplacian != kp->laplacian )
            continue;
        d = compareSURFDescriptors( vec, mvec, dist2, length );
        if( d < dist1 )
        {
            dist2 = dist1;
            dist1 = d;
            neighbor = i;
        }
        else if ( d < dist2 )
            dist2 = d;
    }
    if ( dist1 < 0.6*dist2 )
        return neighbor;
    return -1;
}

void Features::findPairs(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs)
{
    int i;
    CvSeqReader reader, kreader;
    cvStartReadSeq( objectKeypoints, &kreader );
    cvStartReadSeq( objectDescriptors, &reader );
    ptpairs.clear();

    for( i = 0; i < objectDescriptors->total; i++ ){
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* descriptor = (const float*)reader.ptr;
        CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        int nearest_neighbor = naiveNearestNeighbor( descriptor, kp->laplacian, imageKeypoints, imageDescriptors );
        if( nearest_neighbor >= 0 )
        {
            ptpairs.push_back(i);
            ptpairs.push_back(nearest_neighbor);
        }
    }
}

void Features::flannFindPairs(const CvSeq*, const CvSeq* objectDescriptors, const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs)
{
	int length = (int)(objectDescriptors->elem_size/sizeof(float));

    cv::Mat m_object(objectDescriptors->total, length, CV_32F);
    cv::Mat m_image(imageDescriptors->total, length, CV_32F);

    CvSeqReader obj_reader;
	float* obj_ptr = m_object.ptr<float>(0);
    cvStartReadSeq( objectDescriptors, &obj_reader );
    for(int i = 0; i < objectDescriptors->total; i++ )
    {
        const float* descriptor = (const float*)obj_reader.ptr;
        CV_NEXT_SEQ_ELEM( obj_reader.seq->elem_size, obj_reader );
        memcpy(obj_ptr, descriptor, length*sizeof(float));
        obj_ptr += length;
    }
    CvSeqReader img_reader;
	float* img_ptr = m_image.ptr<float>(0);
    cvStartReadSeq( imageDescriptors, &img_reader );
    for(int i = 0; i < imageDescriptors->total; i++ ){
        const float* descriptor = (const float*)img_reader.ptr;
        CV_NEXT_SEQ_ELEM( img_reader.seq->elem_size, img_reader );
        memcpy(img_ptr, descriptor, length*sizeof(float));
        img_ptr += length;
    }

    //Encontrar "nearest neighbors" usando FLANN
    cv::Mat m_indices(objectDescriptors->total, 2, CV_32S);
    cv::Mat m_dists(objectDescriptors->total, 2, CV_32F);
    cv::flann::Index flann_index(m_image, cv::flann::KDTreeIndexParams(4));  //Using 4 randomized kdtrees
    flann_index.knnSearch(m_object, m_indices, m_dists, 2, cv::flann::SearchParams(64) ); //Maximum number of leafs checked

    int* indices_ptr = m_indices.ptr<int>(0);
    float* dists_ptr = m_dists.ptr<float>(0);
    for (int i=0;i<m_indices.rows;++i){
        if (dists_ptr[2*i]<0.6*dists_ptr[2*i+1]){
            ptpairs.push_back(i);
            ptpairs.push_back(indices_ptr[2*i]);
        }
    }
}

vector<int> Features::getTriangle(const CvSeq* imgKeypoints, vector<int> ptpairs)
{
    float maxArea = 0;
    vector<int> ret;
    ret.reserve(3);
    Vector3D vec1;
    Vector3D vec2;

    //Se recorre todo el espacio de puntos para hallar el trío que determina el triángulo de área máxima.
    for(int i = 0; i < (int)ptpairs.size(); i+=2 ){
        for(int j = 0; j < (int)ptpairs.size(); j+=2 ){
            for(int k = 0; k < (int)ptpairs.size(); k+=2 ){
                if (j!=k and j!=i and i!=k){
                    CvSURFPoint* spi = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, ptpairs[i] );
                    CvSURFPoint* spj = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, ptpairs[j] );
                    CvSURFPoint* spk = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, ptpairs[k] );
                    vec1 = Vector3D(spj->pt.x, spj->pt.y, 0) - Vector3D(spi->pt.x, spi->pt.y, 0);
                    vec2 = Vector3D(spk->pt.x, spk->pt.y, 0) - Vector3D(spi->pt.x, spi->pt.y, 0);

                    float area = fabs(vec1.cross(vec2).getMagn()/2.0);
                    if (area > maxArea){
                        maxArea = area;
                        ret.clear();
                        ret.push_back(ptpairs[i]);
                        ret.push_back(ptpairs[j]);
                        ret.push_back(ptpairs[k]);
                    }
                }
            }
        }
    }   
    return ret;
}

vector<int> Features::getMatchingTriangle(const CvSeq* objectKeypoints, vector<int> srcTri, vector<int> ptpairs )
{
    vector<int> matchTri;
    int m1, m2, m3;
    matchTri.reserve(3);

    //Se halla el grupo de puntos de la imagen 2 que se corresponde con el triángulo hallado para la imagen 1,
    //teniendo en cuenta sólo aquellos puntos que tienen correspondencia (ptpairs).
    for(int i = 0; i < (int)ptpairs.size(); i+=2 ){
        CvSURFPoint* pt = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, ptpairs[i] );
        CvSURFPoint* v1 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, srcTri[0] );
        if((v1->pt.x == pt->pt.x) && (v1->pt.y == pt->pt.y)){
            m1 = ptpairs[i+1];
            continue;
        }
        CvSURFPoint* v2 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, srcTri[1] );
        if((v2->pt.x == pt->pt.x) && (v2->pt.y == pt->pt.y)){
            m2 = ptpairs[i+1];
            continue;
        }
        CvSURFPoint* v3 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, srcTri[2] );
        if((v3->pt.x == pt->pt.x) && (v3->pt.y == pt->pt.y)){
            m3 = ptpairs[i+1];
            continue;
        }
    }
    matchTri.push_back(m1);
    matchTri.push_back(m2);
    matchTri.push_back(m3);
    return matchTri;
}

bool Features::vertexHorzCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index)
{
    CvSURFPoint* objV1 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[0] );
    CvSURFPoint* objV2 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[1] );
    CvSURFPoint* objV3 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[2] );
    CvSURFPoint* imgV1 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[0] );
    CvSURFPoint* imgV2 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[1] );
    CvSURFPoint* imgV3 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[2] );
    double angle1 = (180/PI) * atan((double)fabs(objV1->pt.y - imgV1->pt.y) / (double)fabs(objSize[0] - objV1->pt.x + imgV1->pt.x));
    double angle2 = (180/PI) * atan((double)fabs(objV2->pt.y - imgV2->pt.y) / (double)fabs(objSize[0] - objV2->pt.x + imgV2->pt.x));
    double angle3 = (180/PI) * atan((double)fabs(objV3->pt.y - imgV3->pt.y) / (double)fabs(objSize[0] - objV3->pt.x + imgV3->pt.x));

    if ((fabs(angle1 - angle2) > ANGLEDELTA) && (fabs(angle1 - angle3) > ANGLEDELTA))
        index = 0;
    if ((fabs(angle2 - angle3) > ANGLEDELTA) && (fabs(angle1 - angle2) > ANGLEDELTA))
        index = 1;
    if ((fabs(angle1 - angle3) > ANGLEDELTA) && (fabs(angle2 - angle3) > ANGLEDELTA))
        index = 2;

    return ((fabs(angle1 - angle2) < ANGLEDELTA) && (fabs(angle1 - angle3) < ANGLEDELTA) && (fabs(angle2 - angle3) < ANGLEDELTA));
}

bool Features::vertexVertCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index)
{
    CvSURFPoint* objV1 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[0] );
    CvSURFPoint* objV2 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[1] );
    CvSURFPoint* objV3 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[2] );
    CvSURFPoint* imgV1 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[0] );
    CvSURFPoint* imgV2 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[1] );
    CvSURFPoint* imgV3 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[2] );
    double angle1 = (180/PI) * atan((double)fabs(objSize[1] - objV1->pt.y + imgV1->pt.y) / (double)fabs(objV1->pt.x + imgV1->pt.x));
    double angle2 = (180/PI) * atan((double)fabs(objSize[1] - objV2->pt.y + imgV2->pt.y) / (double)fabs(objV2->pt.x + imgV2->pt.x));
    double angle3 = (180/PI) * atan((double)fabs(objSize[1] - objV3->pt.y + imgV3->pt.y) / (double)fabs(objV3->pt.x + imgV3->pt.x));

    if ((fabs(angle1 - angle2) > ANGLEDELTA) && (fabs(angle1 - angle3) > ANGLEDELTA))
        index = 0;
    if ((fabs(angle2 - angle3) > ANGLEDELTA) && (fabs(angle1 - angle2) > ANGLEDELTA))
        index = 1;
    if ((fabs(angle1 - angle3) > ANGLEDELTA) && (fabs(angle2 - angle3) > ANGLEDELTA))
        index = 2;

    return ((fabs(angle1 - angle2) < ANGLEDELTA) && (fabs(angle1 - angle3) < ANGLEDELTA) && (fabs(angle2 - angle3) < ANGLEDELTA));
}

bool Features::checkTriangles(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index)
{
    //Se chequea que los triángulos no contengan outliers matchings.
    bool res1 = vertexHorzCorrespondence(objTri, imgTri, objectKeypoints, imageKeypoints, objSize, index);
    bool res2 = vertexVertCorrespondence(objTri, imgTri, objectKeypoints, imageKeypoints, objSize, index);
    return res1 && res2;
}

void Features::removeMatch(const CvSeq* objectKeypoints, vector<int> &ptpairs, int pt)
{
    //Se remueve del conjunto de pares tanto el punto indicado por "pt" de la imagen 1,
    //como el punto correspondiente de la imagen 2.
    for(int i = 0; i < (int)ptpairs.size(); i+=2 ){
        CvSURFPoint* spt = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, pt );
        CvSURFPoint* v1 = (CvSURFPoint*)cvGetSeqElem( objectKeypoints, ptpairs[i] );
        if(v1->pt.x == spt->pt.x && v1->pt.y == spt->pt.y){
            ptpairs.erase(ptpairs.begin()+i, ptpairs.begin()+i+2);
            break;
        }
    }
}

bool Features::findGoodTriangles(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> ptpairs, vector<int> objSize, vector<int> &objTri, vector<int> &imgTri)
{
    bool goodTriangle = false;
    int index = 0;
    //Se buscan triángulos que resulten convenientes para realizar la transformación.
    while(!goodTriangle){
        objTri = getTriangle(objectKeypoints, ptpairs);
        imgTri = getMatchingTriangle(objectKeypoints, objTri, ptpairs);
        //goodTriangle = checkTriangles(objTri, imgTri, objectKeypoints, imageKeypoints, objSize, index);
        goodTriangle = true;
        if(!goodTriangle)
           removeMatch(objectKeypoints, ptpairs, objTri[index]);
        //cout << ptpairs.size() << endl;
    }
    if(VERBOSE){
        cout << "Descriptores del triangulo de la imagen 1: <" << objTri[0] << ", " << objTri[1] << ", " << objTri[2] << ">" << endl;
        cout << "Descriptores del triangulo de la imagen 2: <" << imgTri[0] << ", " << imgTri[1] << ", " << imgTri[2] << ">" << endl << endl;
    }
    return goodTriangle;
}

QTransform Features::getTransformation(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> &objTri, vector<int> &imgTri)
{
    //Matriz de la transformación:  |a b c|
    //                              |d e f|
    //                              |0 0 1|

    CvPoint v1 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[0] ))->pt);
    CvPoint v2 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[1] ))->pt);
    CvPoint v3 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( objectKeypoints, objTri[2] ))->pt);
    CvPoint u1 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[0] ))->pt);
    CvPoint u2 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[1] ))->pt);
    CvPoint u3 = cvPointFrom32f(((CvSURFPoint*)cvGetSeqElem( imageKeypoints, imgTri[2] ))->pt);

    double 	x11 = v1.x,
            x12 = v1.y,
            x21 = v2.x,
            x22 = v2.y,
            x31 = v3.x,
            x32 = v3.y,
            y11 = u1.x,
            y12 = u1.y,
            y21 = u2.x,
            y22 = u2.y,
            y31 = u3.x,
            y32 = u3.y;

    double a1 = ((y11-y21)*(x12-x32)-(y11-y31)*(x12-x22))/
                ((x11-x21)*(x12-x32)-(x11-x31)*(x12-x22));
    double a2 = ((y11-y21)*(x11-x31)-(y11-y31)*(x11-x21))/
                ((x12-x22)*(x11-x31)-(x12-x32)*(x11-x21));
    double a3 = y11-a1*x11-a2*x12;
    double a4 = ((y12-y22)*(x12-x32)-(y12-y32)*(x12-x22))/
                ((x11-x21)*(x12-x32)-(x11-x31)*(x12-x22));
    double a5 = ((y12-y22)*(x11-x31)-(y12-y32)*(x11-x21))/
                ((x12-x22)*(x11-x31)-(x12-x32)*(x11-x21));
    double a6 = y12-a4*x11-a5*x12;

    if(VERBOSE)
        cout << "Transformacion hallada: [" << a1 << ", " << a4 << ", 0, " << a2 << ", " << a5 << ", 0, " << a3 << ", " << a6 << ", 1]" << endl << endl;

    return QTransform(a1, a4, 0, a2, a5, 0, a3, a6);;
}

bool Features::featuresBasedTransform(IplImage* object, IplImage* image, IplImage* img1, IplImage* img2, QTransform &transform)
{
    CvMemStorage* storage = cvCreateMemStorage(0);

    //Búsqueda de features para ambas imágenes.
    CvSeq *objectKeypoints = 0, *objectDescriptors = 0;
    CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
    CvSURFParams params = cvSURFParams(500, 1);
    double tt = (double)cvGetTickCount();
    cvExtractSURF( object, 0, &objectKeypoints, &objectDescriptors, storage, params );
    cvExtractSURF( image, 0, &imageKeypoints, &imageDescriptors, storage, params );
    tt = (double)cvGetTickCount() - tt;
    if(VERBOSE){
        qDebug() << "Features hallados en la imagen 1:" << objectDescriptors->total;
        qDebug() << "Features hallados en la imagen 2:" << imageDescriptors->total;
        qDebug() << "Tiempo de extraccion:" << tt/(cvGetTickFrequency()*1000.) << "ms." << endl;
    }

    //En caso de ser insuficiente la cantidad de features para alguna de las imágenes, se retorna sin resultado.
    if(objectKeypoints->total < MINPAIRS || imageKeypoints->total < MINPAIRS){
        if(VERBOSE)
            qDebug() << "La cantidad de features encontrados es insuficiente para calcular una transformacion." << endl;
        return false;
    }

    //Busqueda de correspondencia entre puntos característicos (features).
    vector<int> ptpairs;
#ifdef USE_FLANN
    Features::flannFindPairs( objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs );
#else
    Features::findPairs( objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs );
#endif

    //En caso de ser insuficiente la cantidad de puntos de correspondencia, se retorna sin resultado.
    if(VERBOSE)
        qDebug() << "Cantidad de puntos de correspondencia:" << (int)(ptpairs.size()/2) << endl;
    if(ptpairs.size()/2 < MINPAIRS){
        if(VERBOSE)
            qDebug() << "La cantidad de puntos de correspondencia entre las imagenes es insuficiente para calcular una transformacion." << endl;
        return false;
    }

    //Búsqueda de los triángulos que determinarán la transformación a realizar.
    vector<int> objTri, imgTri, objSize;
    objTri.reserve(3);
    imgTri.reserve(3);
    objSize.push_back(object->width);
    objSize.push_back(object->height);

    bool goodTriangle = Features::findGoodTriangles(objectKeypoints, imageKeypoints, ptpairs, objSize, objTri, imgTri);
    //En caso de no hallarse los triángulos de manera satisfactoria, se retorna sin resultado.
    if(!goodTriangle){
        if(VERBOSE)
            qDebug() << "Los triangulos de correspondencia hallados no permiten realizar una transformacion correcta." << endl;
        return false;
    }

    //Se procede a calcular la transformación.
    transform = getTransformation(objectKeypoints, imageKeypoints, objTri, imgTri);

    if(GRAPHIC){
        IplImage* result = Utils::drawResultImage(img1, img2, objectKeypoints, imageKeypoints, objTri, imgTri, ptpairs);
        cvNamedWindow("Imagen de correspondencia", 1);
        cvMoveWindow("Imagen de correspondencia", 100, 100);
        cvShowImage("Imagen de correspondencia", result);
        cvWaitKey(0);
        cvDestroyAllWindows();
    }
    return true;
}
