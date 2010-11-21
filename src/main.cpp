#include <QApplication>
#include <QString>
#include <highgui.h>
#include <ctype.h>
#include "cv.h"
#include "iostream"
#include "highgui.h"
#include "imagefuncs.h"
#include "features.h"
#include "utils.h"
#include "imagehash.h"

using namespace std;

bool VERBOSE = false;
bool GRAPHIC = false;

/*!
 * Imprime por pantalla el modo de uso de la aplicación.
 */
void printHelp()
{
    cout << endl << "El modo de uso de la aplicacion es el siguiente:" << endl << endl;
    cout << "\tKeepcon imagen1 imagen2 [-v] [-g] [-h]" << endl << endl;
    cout << "\t-v: la aplicacion imprime datos sobre el procesamiento." << endl;
    cout << "\t-g: la aplicacion muestra la imagen de correspondencia hallada para las imagenes (si es posible)." << endl;
    cout << "\t-h: muestra esta ayuda." << endl << endl;
    cout << "\tLos argumentos pueden ser dispuestos en cualquier orden." << endl;
    exit(0);
}

/*!
 * Verifica si la aplicación ha sido invocada de manera correcta.
 * En caso afirmativo, retorna los paths de las imágenes a procesar y setea los flags correspondientes.
 * En caso negativo, se imprime la ayuda por pantalla.
 */
void checkCall(int argc, char *argv[], char* &path1, char* &path2)
{
    string arg;
    QList<char*> qArgs;
    for(int i=1; i<argc; i++)
        qArgs << argv[i];
    for(int i=0; i<qArgs.size(); i++){
        arg = qArgs[i];
        if(arg == "-h")
            printHelp();
        if(arg == "-v"){
            VERBOSE = true;
            qArgs.removeAt(i--);
            continue;
        }
        if(arg == "-g"){
            GRAPHIC = true;
            qArgs.removeAt(i--);
        }
    }
    if(qArgs.size() != 2)
        printHelp();
    path1 = qArgs[0];
    path2 = qArgs[1];
}

/*!
 * Aplicación principal:
 * Carga las dos imágenes recibidas como argumentos y determina el RMS que existe entre ellas.
 * En primera instancia, intenta realizar el cálculo basado en una transformación dada por los features de las mismas.
 * Si este paso no resulta exitoso, se procede a redimensionar y realizar el cálculo básico.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(".");

    IplImage *img = Utils::loadImage(argv[1], true);
    ImageHash hash = ImageHash();
    hash.insert("sb.jpg", 120);
    hash.insert("sb1.jpg", 121);
    hash.insert("sb2.jpg", 122);
    hash.insert("sb3.jpg", 123);
    hash.insert("sb4.jpg", 123);
    hash.insert("sb5.jpg", 122);
    hash.insert("sb6.jpg", 125);
    hash.insert("sb7.jpg", 121);
    hash.insert("sb8.jpg", 125);
    qDebug() << hash.getMatchs(Features::getHashKey(img));

    /*
    IplImage *img1, *img2, *img1C, *img2C;
    char *path1, *path2;

    cout << endl;
    checkCall(argc, argv, path1, path2);
    Utils::loadImages(path1, path2, img1, img2, true);
    Utils::loadImages(path1, path2, img1C, img2C);    

    if(VERBOSE){
        cout << "Imagenes originales:" << endl;
        cout << "Imagen 1: " << img1->width << " x " << img1->height << " (" << path1 << ")" << endl;
        cout << "Imagen 2: " << img2->width << " x " << img2->height << " (" << path2 << ")" << endl << endl;
        ImageFuncs::setVerbose(true);
        Features::setVerbose(true);
        Utils::setVerbose(true);
    }
    if(GRAPHIC)
        Features::setGraphic(true);

    double result = 1;    
    if(!ImageFuncs::featuresBasedRMS(result, img1, img2, img1C, img2C))
        result = ImageFuncs::resizeBasedRMS(img1C, img2C);

    cout << "Resultado: " << result << endl;

    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&img1C);
    cvReleaseImage(&img2C);
    */
    exit(0);
    return app.exec();
}
