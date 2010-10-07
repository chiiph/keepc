//#include <QApplication>
#include "iostream"
#include "cv.h"
#include "highgui.h"
#include "imagefuncs.h"

using namespace std;

int main(int argc, char *argv[]){

	//QApplication app(argc, argv);

	ImageFuncs* funcs = new ImageFuncs();

	if(argc < 3){
		cout << "La aplicación debe ser invocada de la siguiente manera:" << endl << endl << "Keepcon path/imagen/1 path/imagen/2" << endl << endl;
		exit(1);
	}

	IplImage* img1 = cvLoadImage(argv[1]);
	IplImage* img2 = cvLoadImage(argv[2]);

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

	cout << "imagen 1: " << img1->width << " x " << img1->height << endl;
	cout << "imagen 2: " << img2->width << " x " << img2->height << endl << endl;

	funcs->ratioResize(img1, img2);

	/*
	cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win1", 100, 100);
	cvShowImage("win1", img1);
	cvNamedWindow("win2", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win2", 150, 150);
	cvShowImage("win2", img2);
	*/

	cout << "RMS = " << funcs->rms(img1, img2) << endl << endl;

	//cvWaitKey(0);
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

	//return app.exec();
}
