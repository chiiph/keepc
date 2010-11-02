#include <QApplication>
#include <QImage>
#include <QtPlugin>
#include <QString>
#include "iostream"
#include "cv.h"
#include <highgui.h>
#include <ctype.h>
#include "highgui.h"
#include "imagefuncs.h"
#include "features.h"
#include "utils.h"

//Q_IMPORT_PLUGIN(qjpeg)
//Q_IMPORT_PLUGIN(qgif)

using namespace std;

const int MIN_ARGS = 2;
bool VERBOSE = false;
bool GRAPHIC = false;

//Imprime el modo de uso de la aplicación.
void printHelp(){
    cout << endl << "El modo de uso de la aplicacion es el siguiente:" << endl << endl;
	cout << "\tKeepcon imagen1 imagen2 [-v] [-h]" << endl << endl;
    cout << "\t-v: la aplicacion imprime datos sobre el procesamiento." << endl;
    cout << "\t-v: la aplicacion muestra la imagen de correspondencia de puntos caracteristicos (si es posible)." << endl;
	cout << "\t-h: muestra esta ayuda." << endl << endl;
	exit(0);
}

//Verifica si la aplicación ha sido invocada correctamente.
//En caso negativo, se imprime la ayuda en pantalla.
//En caso afirmativo, se retornan los paths de las imágenes a procesar.
void checkCall(int argc, char *argv[], char* &path1, char* &path2){
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
            qArgs.removeAt(i);
            i--;
            continue;
		}
        if(arg == "-g"){
            GRAPHIC = true;
            qArgs.removeAt(i);
            i--;
        }
	}
    if(qArgs.size() != 2){
        printHelp();
    }else{
        path1 = qArgs[0];
        path2 = qArgs[1];
    }
}

//Carga las imágenes de acuerdo a los argumentos especificados.
//En caso de producirse un error, se indica que imagen no pudo ser cargada correctamente.
void loadImages(char *argv[], IplImage* &img1, IplImage* &img2){
    img1 = cvLoadImage(argv[1]);
	img2 = cvLoadImage(argv[2]);
	if(img1 == NULL){
        cout << "El argumento "<< argv[1] << " no representa un archivo de imagen valido." << endl;
		exit(1);
	}
	else{
		if(img2 == NULL){
            cout << "El argumento "<< argv[2] << " no representa un archivo de imagen valido." << endl;
			exit(1);
		}
	}
}

int main(int argc, char *argv[]){	

    QApplication app(argc, argv);

    IplImage* img1;
    IplImage* img2;
    IplImage* img1C;
    IplImage* img2C;
    char *path1, *path2;
    checkCall(argc, argv, path1, path2);
    Utils::loadImages(path1, path2, img1, img2, true);
    Utils::loadImages(path1, path2, img1C, img2C);

    cout << endl;
    if(VERBOSE){
        cout << "Imagenes originales:" << endl;
        cout << "Imagen 1: " << img1->width << " x " << img1->height << " (" << argv[1] << ")" << endl;
        cout << "Imagen 2: " << img2->width << " x " << img2->height << " (" << argv[2] << ")" << endl << endl;        
    }

    ImageFuncs::setVerbose(VERBOSE);
    Features::setVerbose(VERBOSE);
    Utils::setVerbose(VERBOSE);
    Features::setGraphic(GRAPHIC);

    double rms = 1;
    if(!ImageFuncs::featuresBasedRMS(rms, img1, img2, img1C, img2C)){
        ImageFuncs::ratioResize(img1, img2);
        rms = ImageFuncs::rms(img1, img2);
    }
    cout << "RMS: " << rms << endl;

    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&img1C);
    cvReleaseImage(&img2C);

    return app.exec();
}

