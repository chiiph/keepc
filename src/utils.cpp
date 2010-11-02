#include "utils.h"

bool Utils::VERBOSE = false;

void Utils::setVerbose(bool verbose){
    VERBOSE = verbose;
}

void Utils::loadImages(char* path1, char* path2, IplImage* &img1, IplImage* &img2, bool GRAYSCALE){
    bool gif = QString(path1).endsWith(".gif", Qt::CaseInsensitive) || QString(path2).endsWith(".gif", Qt::CaseInsensitive);
    img1 = loadImage(path1, GRAYSCALE, gif);
    img2 = loadImage(path2, GRAYSCALE, gif);
    if(img1 == NULL){
        qDebug() << "Ocurrio un error en la carga de la imagen " << path1;
        exit(1);
    }
    else{
        if(img2 == NULL){
            qDebug() << "Ocurrio un error en la carga de la imagen " << path2;
            exit(1);
        }
    }
}

IplImage* Utils::loadImage(char* path, bool GRAYSCALE, bool QIMG){
    qDebug() << "PATH: " << path;
    IplImage* img = NULL;
    if(GRAYSCALE){
        QImage *q = new QImage(path);
        qDebug() << "QIMAGE: " << q->height();
        if(QIMG)
            img = Utils::qtToCvGrayscale(new QImage(path));
        else
            img = cvLoadImage(path, CV_LOAD_IMAGE_GRAYSCALE);
    }else{
        if(QIMG)
            img = Utils::qtToCv(new QImage(path));
        else
            img = cvLoadImage(path);
    }
    qDebug() << "HEIGHT: " << img->height;
    return img;
}

//Converts an IplImage to a QImage
QImage* Utils::cvToQt(IplImage *img){
    QImage aux((uchar*) img->imageData , img->width, img->height, img->widthStep, QImage::Format_RGB888);
    return(new QImage(aux.rgbSwapped())) ;
}

//Converts a QImage to an IplImage
IplImage* Utils::qtToCv(QImage *qImage){
    IplImage* cvImage;   
    //qImage = new QImage(qImage->rgbSwapped());
    cvImage = cvCreateImageHeader(cvSize(qImage->width(), qImage->height()), IPL_DEPTH_8U, 4);
    cvImage->imageData = (char*)qImage->bits();
    IplImage* colorImage = cvCreateImage( cvGetSize(cvImage), IPL_DEPTH_8U, 3 );
    cvConvertImage( cvImage, colorImage );
    return colorImage;
}

IplImage* Utils::qtToCvGrayscale(QImage *qImage){
    IplImage* img = cvCreateImage(cvSize(qImage->width(), qImage->height()), 8, 1);
    uchar* data = (uchar *)img->imageData;
    int height = img->height;
    int width = img->width;
    int step = img->widthStep;
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
                data[i*step + j] = qGray(qImage->pixel(j, i));
    qDebug() << "QTTOGRAY: " << img->height;
    return img;
}

void Utils::toGrayScale(QImage* &img){
    for(int i=0; i<img->width(); i++)
        for(int j=0; j<img->height(); j++){
            int g = qGray(img->pixel(i, j));
            img->setPixel(i, j, qRgb(g, g, g));
        }
}

void Utils::toBlack(IplImage* &img){
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


IplImage* Utils::drawResultImage(IplImage* img1, IplImage* img2, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> img1Tri, vector<int> img2Tri, vector<int> ptpairs){
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

void Utils::drawFeatureCircles(IplImage* &img, CvSeq *imgKeypoints){
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

void Utils::drawFeatureMatchLines(IplImage* &img, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> ptpairs, int widthOffset){
    for(int i = 0; i < (int)ptpairs.size(); i += 2 ){
        CvSURFPoint* r1 = (CvSURFPoint*)cvGetSeqElem( img1Keypoints, ptpairs[i] );
        CvSURFPoint* r2 = (CvSURFPoint*)cvGetSeqElem( img2Keypoints, ptpairs[i+1] );
        cvLine( img, cvPointFrom32f(r1->pt), cvPoint(cvRound(r2->pt.x + widthOffset), cvRound(r2->pt.y)), cvScalar(255, 0, 255) );
    }
}

void Utils::drawTriangle(IplImage* &img, CvSeq *imgKeypoints, vector<int> imgTri){
    CvSURFPoint* imgV1 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[0] );
    CvSURFPoint* imgV2 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[1] );
    CvSURFPoint* imgV3 = (CvSURFPoint*)cvGetSeqElem( imgKeypoints, imgTri[2] );
    cvLine(img, cvPointFrom32f(imgV1->pt), cvPointFrom32f(imgV2->pt), cvScalar(0, 255, 0) );
    cvLine(img, cvPointFrom32f(imgV1->pt), cvPointFrom32f(imgV3->pt), cvScalar(0, 255, 0) );
    cvLine(img, cvPointFrom32f(imgV2->pt), cvPointFrom32f(imgV3->pt), cvScalar(0, 255, 0) );
}

void sort(vector<int> &v) {
    QList<int> l;
    l << v[0];
    l << v[1];
    l << v[2];
    qSort(l);
    v[0] = l[0];
    v[1] = l[1];
    v[2] = l[2];
}

bool Utils::toggleRequired(IplImage* img1, IplImage* img2){
    float w1 = img1->width;
    float h1 = img1->height;
    float w2 = img2->width;
    float h2 = img2->height;
    cout << w1 << " - " << h1 << " - " << w2 << " - " << h2;
    if(w1 >= w2){
        if(h1 >= h2){
            return true;
        }
        else{
            if(w1/w2 > h2/h1)
                return true;
            else
                return false;
        }
    }
    else{
        if(h1 >= h2){
            if(w2/w1 > h1/h2)
                return false;
            else
                return true;
        }
        else
            return false;
    }
}
