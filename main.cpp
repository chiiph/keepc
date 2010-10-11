//#include <QApplication>
#include "iostream"
#include "cv.h"
#include "highgui.h"
#include "imagefuncs.h"
using namespace std;

const int MIN_ARGS = 2;
bool VERBOSE = false;

//Imprime el modo de uso de la aplicación.
void printHelp(){
	cout << endl << "El modo de uso de la aplicación es el siguiente:" << endl << endl;
	cout << "\tKeepcon imagen1 imagen2 [-v] [-h]" << endl << endl;
	cout << "\t-v: la aplicación imprime datos sobre el procesamiento." << endl;
	cout << "\t-h: muestra esta ayuda." << endl << endl;
	exit(0);
}

//Verifica si la aplicación ha sido invocada correctamente.
//En caso negativo, se imprime la ayuda en pantalla.
void checkCall(int argc, char *argv[]){
	int flags = 0;
	string arg;
	for(int i=1; i<argc; i++){
		arg = argv[i];
		if(arg == "-h")
			printHelp();
		if(arg == "-v"){
			VERBOSE = true;
			flags++;
		}
	}
	if((argc - flags - 1) < MIN_ARGS)
		printHelp();
}

//Carga las imágenes de acuerdo a los argumentos especificados.
//En caso de producirse un error, se indica que imagen no pudo ser cargada correctamente.
void loadImages(char *argv[], IplImage* &img1, IplImage* &img2){
	img1 = cvLoadImage(argv[1]);
	img2 = cvLoadImage(argv[2]);
	if(img1 == NULL){
		cout << "El argumento "<< argv[1] << " no representa un archivo de imagen válido." << endl;
		exit(1);
	}
	else{
		if(img2 == NULL){
			cout << "El argumento "<< argv[2] << " no representa un archivo de imagen válido." << endl << endl;
			exit(1);
		}
	}
}

int main(int argc, char *argv[]){

	//QApplication app(argc, argv);

	IplImage* img1;
	IplImage* img2;

	checkCall(argc, argv);
	loadImages(argv, img1, img2);

	if(VERBOSE){
		cout << "Imágenes originales:" << endl;
		cout << "Imagen 1: " << img1->width << " x " << img1->height << " (" << argv[1] << ")" << endl;
		cout << "Imagen 2: " << img2->width << " x " << img2->height << " (" << argv[2] << ")" << endl << endl;
	}

	ImageFuncs* funcs = new ImageFuncs(VERBOSE);
	funcs->ratioResize(img1, img2);
	cout << "RMS: " << funcs->rms(img1, img2) << endl << endl;


	cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win1", 100, 100);
	cvShowImage("win1", img1);
	cvNamedWindow("win2", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win2", 150, 150);
	cvShowImage("win2", img2);
	cvWaitKey(0);


	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

	//return app.exec();
}
