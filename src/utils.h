#ifndef UTILS_H
#define UTILS_H
#define LRM_K 6
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
#include <QRgb>
#include <QLabel>

using namespace std;

class Utils{

public:

    /*!
     * Determina si las operaciones de la clase deben imprimir datos del procesamiento por pantalla.
     */
    static void setVerbose(bool verbose);

    /*!
     * Carga las dos imágenes a procesar (en color o escala de grises) según los paths indicados.
     */
    static void loadImages(char* path1, char* path2, IplImage* &img1, IplImage* &img2, bool GRAYSCALE=false);

    /*!
     * Carga una imagen según el path indicado.
     * Puede determinarse si la imagen debe ser cargada en color (por defecto) o escala de grises,
     * y si debe utilizarse OpenCV (por defecto) o Qt para realizar la operación (utilizado para la carga de imágenes con formato GIF).
     */
    static IplImage* loadImage(char* path, bool GRAYSCALE=false, bool QIMG=false);

    /*!
     * Retorna una QImage (Qt) creada a partir de una IplImage (OpenCV).
     */
    static QImage* cvToQt(IplImage *img);

    /*!
     * Retorna una IplImage (OpenCV) creada a partir de una QImage (Qt).
     */
    static IplImage* qtToCv(QImage *qImage);

    /*!
     * Convierte una QImage (Qt) a escala de grises.
     */
    static void toGrayScale(QImage* &img);

    /*!
     * Convierte una IplImage (OpenCV) a color negro.
     */
    static void toBlack(IplImage* &img);

    /*!
     * Retorna una IplImage (OpenCV) en escala de grises creada a partir de una QImage (Qt).
     */
    static IplImage* qtToCvGrayscale(QImage *qImage);

    /*!
     * Retorna una imagen que contiene las dos imágenes indicadas, sus features, los líneas de correspondencia entre estos
     * y los triángulos hallados que permitirán realizar la transformación.
     */
    static IplImage* drawResultImage(IplImage* img1, IplImage* img2, CvSeq *img1Keypoints, CvSeq *img2Keypoints, vector<int> img1Tri, vector<int> img2Tri, vector<int> ptpairs);

    /*!
     * Determina si es necesario invertir el orden de consideración de las imágenes con el objetivo de obtener
     * una transformación más conveniente.
     */
    static bool toggleRequired(IplImage* img1, IplImage* img2);

    static double getY(QRgb rgb);

    static double getI(QRgb rgb);

    static double getQ(QRgb rgb);

    static double linearRangeMap(double rms);

    static double sigmoidal(double x);

    static void show(char* title, IplImage* img);

    static void show(QString title, QImage* img);

private:

    static bool VERBOSE;

    /*!
     * Dibuja círculos en la imagen basados en cada uno de los puntos de la secuencia.
     * Resulta de utilidad para identificar los puntos característicos hallados para cada imagen.
     */
    static void drawFeatureCircles(IplImage* &img, CvSeq *imgKeypoints);

    /*!
     * Dibuja líneas que unen los pares de puntos indicados en ptpairs.
     * Resulta de utilidad para graficar la correspondencia entre los features de ambas imágenes.
     */
    static void drawFeatureMatchLines(IplImage* &img, CvSeq *objectKeypoints, CvSeq *imageKeypoints, vector<int> ptpairs, int widthOffset);

    /*!
     * Dibuja las tres líneas que forman el triángulo determinado por los tres vértices indicados.
     * Resulta de utilidad para identificar los triángulos utilizados para generar la transformación.
     */
    static void drawTriangle(IplImage* &img, CvSeq *imgKeypoints, vector<int> imgTri);
};

#endif // UTILS_H
