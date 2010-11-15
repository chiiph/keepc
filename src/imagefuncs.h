#ifndef IMAGEFUNCS_H
#define IMAGEFUNCS_H
#define MAX_DIFF 69416.609  // 0.8*(255^2) + 0.15*(303.807^2) + 0.05*(266.526^2)
#include "cv.h"
#include "highgui.h"
#include <QImage>
#include "iostream"
#include "math.h"
#include "features.h"
#include "utils.h"

using namespace std;

/*!
 * Provee funcionalidades para el manejo de imágenes y la realización de cálculos basados en estas.
 */
class ImageFuncs
{
public:

    /*!
     * Determina si las operaciones de la clase deben imprimir datos del procesamiento por pantalla.
     */
    static void setVerbose(bool verbose);

    /*!
     * Recorta la imagen según la región de interés determinada (en caso de ser posible).
     * La región de interés (roi) es determinada mediante un rectángulo de la forma (X inicial, Y inicial, Ancho, Alto).
     */
    static void crop(IplImage* &img, CvRect roi);

    /*!
     * Adapta (redimensionando y cortando) una de las imágenes recibidas para que los tamaños de ambas resulten iguales.
     */
    static void ratioResize(IplImage* &img1, IplImage* &img2);

    /*!
     * Determina el Root Mean Square (RMS) existente entre las imágenes recibidas en el espacio de color YIQ.
     */
    static double yiqRms(IplImage* img1, IplImage* img2);

    /*!
     * Determina el Root Mean Square (RMS) existente entre las imágenes recibidas en el espacio de color YIQ.
     * Establece la región a tener en cuenta según el desplazamiento (img1x, img1y) que presenta la imagen 1.
     */
    static double yiqRms(QImage* qImg1, QImage* qImg2, int img1x, int img1y);

    /*!
     * Determina el Root Mean Square (RMS) existente entre las imágenes recibidas.
     * Para esto, halla y aplica (en caso de ser posible) una transformación basada en los features de las imágenes
     * con el objectivo de minimizar la diferencia entre ellas, y posteriormente procede a realizar el cálculo RMS.
     */
    static bool featuresBasedRMS(double &result, IplImage* img1, IplImage* img2, IplImage* img1Color=NULL, IplImage* img2Color=NULL);

    static double resizeBasedRMS(IplImage* &img1, IplImage* &img2);

    static double yiqDiff(QRgb pix1, QRgb pix2);



private:

    static bool VERBOSE;
};

#endif // IMAGEFUNCS_H
