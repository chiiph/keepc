#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <QImage>
#include <cv.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <QDebug>

using namespace std;

class Utils{

private:

    static bool VERBOSE;

public:

    static void setVerbose(bool verbose);

    static void loadImages(char* path1, char* path2, IplImage* &img1, IplImage* &img2, bool GRAYSCALE=false);

    static IplImage* loadImage(char* path, bool GRAYSCALE=false, bool QIMG=false);

    static QImage* cvToQt(IplImage *img);

    static IplImage* qtToCv(QImage *qImage);    

    static void drawFeatureCircles(IplImage* &img, CvSeq *imgKeypoints);

    static void toGrayScale(QImage* &img);

    static IplImage* qtToCvGrayscale(QImage *qImage);

    static void drawFeatureMatchLines(IplImage* &img, CvSeq *objectKeypoints, CvSeq *imageKeypoints, vector<int> ptpairs, int widthOffset);

    static void toBlack(IplImage* &img);

    static IplImage* drawResultImage(IplImage* img1, IplImage* img2, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> img1Tri, vector<int> img2Tri, vector<int> ptpairs);

    static void drawTriangle(IplImage* &img, CvSeq *imgKeypoints, vector<int> imgTri);

    static bool toggleRequired(IplImage* img1, IplImage* img2);
};

#endif // UTILS_H
