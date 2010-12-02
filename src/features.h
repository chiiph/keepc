#ifndef FEATURES_H
#define FEATURES_H
#define ANGLEDELTA 8
#define PI 3.14159
#define MINPAIRS 3
#define USE_FLANN
#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <QtGui>
#include "vector3d.h"
#include "utils.h"

using namespace std;

/*!
 * Provee la funcionalidad necesaria para obtener una transformación que permita minimizar la diferencia entre dos imágenes.
 * Permite realizar operaciones basadas en los puntos característicos (features) de las imágenes, como:
 * hallar el conjunto de pares que mapean puntos de ambas imágenes, obtener los triángulos de área máxima,
 * determinar las transformaciones asociadas a estos triángulos, entre otras.
 */
class Features
{
public:

    /*!
     * Determina si las operaciones de la clase deben imprimir datos del procesamiento por pantalla.
     */
    static void setVerbose(bool verbose);

    /*!
     * Determina si las operaciones de la clase deben mostrar imágenes por pantalla en caso de ser posible.
     * Actualmente determina específicamente si debe graficarse la correspondencia puntos entre las imágenes.
     */
    static void setGraphic(bool graphic);

    /*!
     * Determina los pares de correspondencia que asocian los puntos de ambas imágenes según los puntos característicos (features) de cada una indicados.
     */
    static void findPairs(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs);

    /*!
     * Determina los pares de correspondencia que asocian los puntos de ambas imágenes según los puntos característicos (features) de cada una indicados.
     * Para esto emplea la librería FLANN: Fast Library for Approximate Nearest Neighbor.
     */
    static void flannFindPairs(const CvSeq*, const CvSeq* objectDescriptors,const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs);

    /*!
     * Determina la terna de puntos que representan el triángulo de área máxima según los puntos característicos de la imagen.
     */
    static vector<int> getTriangle(const CvSeq* imgKeypoints, vector<int> ptpairs);

    /*!
     * Determina la terna de puntos de la imagen 2 que se corresponde con el triángulo de la imagen 1 indicado.
     */
    static vector<int> getMatchingTriangle(const CvSeq* objectKeypoints, vector<int> srcTri, vector<int> ptpairs );

    /*!
     * Determina si los triángulos especificados contienen outliers matchings.
     * Esto permite determinar si permitirán generar una transformación válida.
     */
    static bool checkTriangles(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    /*!
     * Halla, en caso de ser posible, los triángulos de correspondencia entre las imágenes, lo que permitirá calcular
     * y aplicar la transformación más conveniente.
     * En caso de no tener éxito, la operación retorna falso.
     */
    static bool findGoodTriangles(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> ptpairs, vector<int> objSize, vector<int> &objTri, vector<int> &imgTri);

    /*!
     * Determina la transformación que debe ser aplicada sobre la imagen 1, dados los triángulos de correspondencia de ambas imágenes.
     */
    static QTransform getTransformation(const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> &objTri, vector<int> &imgTri);

    /*!
     * Dadas dos imágenes, retorna la transformación que debe ser aplicada a la imagen 1 para minimizar la diferencia entre las mismas.
     * Si no es posible hallar dicha transformación, la operación retorna falso.
     */
    static bool featuresBasedTransform(IplImage* object, IplImage* image, IplImage* img1, IplImage* img2, QTransform &transform);

    static void getFeatures(IplImage* img, CvSeq* &keypoints, CvSeq* &descriptors);

    static QList<int> getHessians(IplImage* img);

    static void filterByDirection(CvSeq* img1Keypoints, CvSeq* img2Keypoints, vector<int> &ptpairs);

private:

    static bool VERBOSE;

    static bool GRAPHIC;

    /*!
     * Realiza una comparación entre los descriptores de features de ambas imágenes.
     */
    static double compareSURFDescriptors(const float* d1, const float* d2, double best, int length);

    /*!
     * Algoritmo que permite llevar a cabo la búsqueda de pares de puntos efectuada por la operación findPairs().
     */
    static int naiveNearestNeighbor(const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors);

    /*!
     * Control utilizado por la operación checkTriangles() para determinar si los triángulos contienen outliers matchings.
     */
    static bool vertexHorzCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    /*!
     * Control utilizado por la operación checkTriangles() para determinar si los triángulos contienen outliers matchings.
     */
    static bool vertexVertCorrespondence(vector<int> objTri, vector<int> imgTri, const CvSeq* objectKeypoints, const CvSeq* imageKeypoints, vector<int> objSize, int &index);

    /*!
     * Remueve un par de puntos conjunto de pares.
     * Son eliminados tanto el punto de la imagen 1 indicado como el punto correspondiente de la imagen 2.
     */
    static void removeMatch(const CvSeq* objectKeypoints, vector<int> &ptpairs, int pt);
};

#endif // FEATURES_H
