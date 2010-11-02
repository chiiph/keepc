#ifndef FEATURES_H
#define FEATURES_H
#define ANGLEDELTA 8
#define PI 3.14159
#define MINPAIRS 3
#define USE_FLANN //define whether to use approximate nearest-neighbor search

#include "vector3d.h"
#include "utils.h"
#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <QtGui>

using namespace std;

class Features{

private:

    static bool VERBOSE;

    static bool GRAPHIC;

public:

    static void setVerbose(bool verbose);

    static void setGraphic(bool graphic);

    static double compareSURFDescriptors(const float* d1, const float* d2, double best, int length);

    static int naiveNearestNeighbor(const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors);

    static void findPairs(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs);

    static void flannFindPairs(const CvSeq*, const CvSeq* objectDescriptors,const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs);

    static int locatePlanarObject(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, const CvPoint src_corners[4], CvPoint dst_corners[4]);

    static vector<int> getTriangle( const CvSeq* imgKeypoints, vector<int> ptpairs );

    static vector<int> getMatchingTriangle(const CvSeq* objectKeypoints, vector<int> srcTri, vector<int> ptpairs );

    static bool vertexHorzCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    static bool vertexVertCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    static bool checkTriangles(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    static void removeMatch(const CvSeq* objectKeypoints, vector<int> &ptpairs, int pt);

    static bool findGoodTriangles(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> ptpairs, vector<int> objSize, vector<int> &objTri, vector<int> &imgTri);

    static QTransform getTransformation(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> &objTri, vector<int> &imgTri);

    static bool featuresBasedTransform(IplImage* object, IplImage* image, QTransform &transform, IplImage* img1=NULL, IplImage* img2=NULL);
};

#endif // FEATURES_H
