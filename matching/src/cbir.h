#ifndef CBIR_H
#define CBIR_H
#include <cv.h>
#include <cv.hpp>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QMap>
#include <QMapIterator>
#include <QDebug>
#include <QPair>
#include <QList>
//#include "KeyfileIncIndex.hpp"

using namespace std;
//using namespace lemur::api;
//using namespace lemur::index;

class CBIR
{
public:
    CBIR();
    static const int DESCRIPTOR_DIMS = 128;
    QList<QPair<QString, int> > featuresCount;
    cv::Mat descriptorsMat;
    cv::Mat clustersMat;
    void buildIndex(QString path);
    void getDescriptorsSequence(QString path);
    void buildDescriptorsMatrix(QString path = "");
    int computeClusters();
    int buildClustersIndex();
    void addImage(QString path);
    void query(QString path);
    cv::Mat getClustersIndices(QString path);
    void setupQuery(QString path, cv::Mat &indices);
    void setupAdd(QString path, cv::Mat &indices);
    void printMatrix(cv::Mat *mat);
    void printDescriptorsMatrix();
    void buildLemurIndex();
};

#endif // CBIR_H
