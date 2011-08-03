#include "utils.h"

bool Utils::VERBOSE = false;

void Utils::setVerbose(bool verbose)
{
    VERBOSE = verbose;
}

void Utils::loadImages(char* path1, char* path2, IplImage* &img1, IplImage* &img2, bool GRAYSCALE)
{
    bool gif = QString(path1).endsWith(".gif", Qt::CaseInsensitive) || QString(path2).endsWith(".gif", Qt::CaseInsensitive);
    img1 = loadImage(path1, GRAYSCALE, gif);
    img2 = loadImage(path2, GRAYSCALE, gif);
}

IplImage* Utils::loadImage(char* path, bool GRAYSCALE, bool QIMG)
{
    IplImage* img = NULL;
    if(QString(path).endsWith(".gif", Qt::CaseInsensitive)){
        QImage *qImg = new QImage(path);
        if(qImg->height() == 0 || qImg->width() == 0){
            qDebug() << "Imagen no encontrada o error en la carga de:" << path;
            exit(1);
        }
        if(GRAYSCALE)
            img = Utils::qtToCvGrayscale(qImg);
        else
            //img = Utils::qtToCv(new QImage(path));
            img = Utils::qtToCv(qImg);
        delete qImg;
    }else{
        if(GRAYSCALE)
            img = cvLoadImage(path, CV_LOAD_IMAGE_GRAYSCALE);
        else
            img = cvLoadImage(path);
    }    
    if(img == NULL){
        qDebug() << "Imagen no encontrada o error en la carga de:" << path;
        exit(1);
    }
    return img;
}

IplImage* Utils::loadImageNoExit(char* path, bool GRAYSCALE)
{
    IplImage* img = NULL;
    if(QString(path).endsWith(".gif", Qt::CaseInsensitive)){
        QImage *qImg = new QImage(path);
        if(!(qImg->height() == 0 || qImg->width() == 0)){
            if(GRAYSCALE)
                img = Utils::qtToCvGrayscale(qImg);
            else                
                img = Utils::qtToCv(qImg);
        }
        delete qImg;
    }else{
        if(GRAYSCALE)
            img = cvLoadImage(path, CV_LOAD_IMAGE_GRAYSCALE);
        else
            img = cvLoadImage(path);
    }    
    return img;
}

QImage* Utils::cvToQt(IplImage *img)
{
    QImage aux((uchar*) img->imageData , img->width, img->height, img->widthStep, QImage::Format_RGB888);
    return(new QImage(aux.rgbSwapped()));
}

IplImage* Utils::qtToCv(QImage *qImage)
{
    IplImage* cvImage;
    cvImage = cvCreateImageHeader(cvSize(qImage->width(), qImage->height()), IPL_DEPTH_8U, 4);
    cvImage->imageData = (char*)qImage->bits();
    IplImage* colorImage = cvCreateImage( cvGetSize(cvImage), IPL_DEPTH_8U, 3 );
    cvConvertImage( cvImage, colorImage );
    cvReleaseImage(&cvImage);
    return colorImage;
}

IplImage* Utils::qtToCvGrayscale(QImage *qImage)
{
    IplImage* img = cvCreateImage(cvSize(qImage->width(), qImage->height()), 8, 1);
    uchar* data = (uchar *)img->imageData;
    int height = img->height;
    int width = img->width;
    int step = img->widthStep;
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
                data[i*step + j] = qGray(qImage->pixel(j, i));   
    return img;
}

void Utils::toGrayScale(QImage* &img)
{
    for(int i=0; i<img->width(); i++){
        for(int j=0; j<img->height(); j++){
            int g = qGray(img->pixel(i, j));
            img->setPixel(i, j, qRgb(g, g, g));
        }
    }
}

void Utils::toBlack(IplImage* &img)
{
    uchar* data = (uchar *)img->imageData;
    int height = img->height;
    int width = img->width;
    int channels = img->nChannels;
    int step = img->widthStep;
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            for(int k=0; k<channels; k++)
                data[i*step + j*channels + k] = 0;
}

IplImage* Utils::drawResultImage(IplImage* img1, IplImage* img2, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> img1Tri, vector<int> img2Tri, vector<int> ptpairs)
{
    int sep = 5;
    int maxHeight = (img1->height > img2->height) ? img1->height : img2->height;
    IplImage* correspond = cvCreateImage( cvSize(img1->width + img2->width + sep, maxHeight), 8, 3 );
    Utils::toBlack(correspond);
    cvSetImageROI( correspond, cvRect( 0, 0, img1->width, img1->height ) );
    cvCopy( img1, correspond );
    Utils::drawFeatureCircles(correspond, img1Keypoints);    
    cvSetImageROI( correspond, cvRect( img1->width + sep, 0, img2->width, img2->height ) );
    cvCopy( img2, correspond );
    Utils::drawFeatureCircles(correspond, img2Keypoints);
    cvResetImageROI(correspond);   
    Utils::drawFeatureMatchLines(correspond, img1Keypoints, img2Keypoints, ptpairs, img1->width + sep);   
    cvSetImageROI( correspond, cvRect( 0, 0, img1->width, img1->height ) );
    Utils::drawTriangle(correspond, img1Keypoints, img1Tri);
    cvSetImageROI( correspond, cvRect( img1->width + sep, 0, img2->width, img2->height ) );
    Utils::drawTriangle(correspond, img2Keypoints, img2Tri);
    cvResetImageROI(correspond);

    return correspond;    
}

void Utils::drawFeatureCircles(IplImage* &img, CvSeq *imgKeypoints)
{
    for(int i = 0; i < imgKeypoints->total; i++ ){
        CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, i );
        CvPoint center;
        int radius;
        center.x = cvRound(r->pt.x);
        center.y = cvRound(r->pt.y);
        radius = cvRound(r->size*1.2/9.*2);
        cvCircle( img, center, radius, cvScalar(0, 0, 255), 1, 8, 0 );
    }
}

void Utils::drawFeatureMatchLines(IplImage* &img, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> ptpairs, int widthOffset)
{
    for(int i = 0; i < (int)ptpairs.size(); i += 2 ){
        CvSURFPoint* r1 = (CvSURFPoint*)cvGetSeqElem( img1Keypoints, ptpairs[i] );
        CvSURFPoint* r2 = (CvSURFPoint*)cvGetSeqElem( img2Keypoints, ptpairs[i+1] );
        cvLine( img, cvPointFrom32f(r1->pt), cvPoint(cvRound(r2->pt.x + widthOffset), cvRound(r2->pt.y)), cvScalar(255, 0, 150) );
    }
}

void Utils::drawTriangle(IplImage* &img, CvSeq *imgKeypoints, vector<int> imgTri)
{
    CvSURFPoint* imgV1 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[0] );
    CvSURFPoint* imgV2 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[1] );
    CvSURFPoint* imgV3 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[2] );
    cvLine(img, cvPointFrom32f(imgV1->pt), cvPointFrom32f(imgV2->pt), cvScalar(0, 255, 0) );
    cvLine(img, cvPointFrom32f(imgV1->pt), cvPointFrom32f(imgV3->pt), cvScalar(0, 255, 0) );
    cvLine(img, cvPointFrom32f(imgV2->pt), cvPointFrom32f(imgV3->pt), cvScalar(0, 255, 0) );
}

bool Utils::toggleRequired(IplImage* img1, IplImage* img2)
{
    float w1 = img1->width;
    float h1 = img1->height;
    float w2 = img2->width;
    float h2 = img2->height;
    //Se determina si es necesario invertir el orden de las imágenes según las diferencias de altura y ancho.
    if(w1 >= w2)
        if(h1 >= h2)
            return true;
        else
            if(w1/w2 > h2/h1)
                return true;
            else
                return false;
    else
        if(h1 >= h2)
            if(w2/w1 > h1/h2)
                return false;
            else
                return true;
        else
            return false;
}

double Utils::getY(QRgb rgb){
    return 0.299*qRed(rgb) + 0.587*qGreen(rgb) + 0.114*qBlue(rgb);
}

double Utils::getI(QRgb rgb){
    return 0.595716*qRed(rgb) - 0.274453*qGreen(rgb) - 0.321263*qBlue(rgb);
}

double Utils::getQ(QRgb rgb){
    return 0.211456*qRed(rgb) - 0.522591*qGreen(rgb) + 0.311135*qBlue(rgb);
}

double Utils::linearRangeMap(double rms){
    double x = (rms - 0.5) * LRM_K;
    if(VERBOSE)
        cout << "Mapeo lineal de rango: " << x << " (K=" << LRM_K << ")" << endl << endl;
    return x;
}

double Utils::sigmoidal(double x){
    double v = 1 / (1 + exp(-x));
    if(VERBOSE)
        qDebug() << "Sigmoidal:" << v << endl;
    return v;
}

void Utils::show(char* title, IplImage* img){
    cvNamedWindow(title, 1);
    cvMoveWindow(title, 100, 100);
    cvShowImage(title, img);
}

void Utils::show(QString title, QImage* img){
    QLabel label;
    label.setPixmap(QPixmap::fromImage(*img));
    label.setWindowTitle(title);
    label.show();
}

QList<int> Utils::sortByHessian(CvSeq*&keypoints){
    QList<int> list;
    for(int i=0; i<keypoints->total; i++){
        CvSURFPoint* p = (CvSURFPoint*)cvGetSeqElem(keypoints, i);
        list << (int)p->hessian;
    }
    qSort(list.begin(), list.end(), qGreater<int>());
    return list;
}

void Utils::printPairsInfo(CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> ptpairs){
    for(int i = 0; i < (int)ptpairs.size(); i+=2 ){
        CvSURFPoint* f1 = (CvSURFPoint*)cvGetSeqElem( img1Keypoints, ptpairs[i] );
        CvSURFPoint* f2 = (CvSURFPoint*)cvGetSeqElem( img2Keypoints, ptpairs[i+1] );
        qDebug() << "i:" << ptpairs[i];// << ", Hessian:" << f1->hessian << ", Laplacian:" << f1->laplacian << ", Size:" << f1->size << ", Dir:" << f1->dir;
        qDebug() << "i:" << ptpairs[i+1];// << ", Hessian:" << f2->hessian << ", Laplacian:" << f2->laplacian << ", Size:" << f2->size << ", Dir:" << f2->dir;
        qDebug() << "-------";
    }
}

double Utils::trace(QTransform tr){
    return tr.m11() + tr.m22() + tr.m33();
}
