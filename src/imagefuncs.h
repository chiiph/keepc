#ifndef IMAGEFUNCS_H
#define IMAGEFUNCS_H
#include "cv.h"
#include "highgui.h"
#include <QImage>
#include "iostream"
#include "math.h"
#include "features.h"
#include "utils.h"

using namespace std;

class ImageFuncs{    

private:

    static bool VERBOSE;

public:    

    //Constructor de la clase
    //El argumento verbose indica si las funciones deben imprimir datos en pantalla.
    //ImageFuncs(bool verbose=false);
    static void setVerbose(bool verbose);

    //Recorta la imagen img según la región de interés expresada (en caso de ser posible).
    //La región de interés (roi) es determinada mediante un rectángulo de la forma (X, Y, Ancho, Alto).
    static void crop(IplImage* &img, CvRect roi);

    //Recibe dos imágenes y adapta una de ellas para que sus tamaños resulten iguales.
    static void ratioResize(IplImage* &img1, IplImage* &img2);

    //Recibe dos imágenes y determina el Root Mean Square (RMS) que existe entre ellas.
    static double rms(IplImage* img1, IplImage* img2);

    static double rms(QImage* qImg1, QImage* qImg2, int img1x, int img1y);

    static bool featuresBasedTransform(QImage* img1, QImage* img2);

    static bool featuresBasedRMS(double &rms, IplImage* img1, IplImage* img2, IplImage* img1Color=NULL, IplImage* img2Color=NULL);
};

//ImageFuncs::VERBOSE = false;

#endif // IMAGEFUNCS_H
