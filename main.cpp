#include <QApplication>
#include <QPushButton>
#include "iostream"
#include "cv.h"
#include "highgui.h"
#include "imagefuncs.h"

using namespace std;

int main(int argc, char *argv[]){

	QApplication app(argc, argv);

	ImageFuncs* funcs = new ImageFuncs();
	IplImage* img1 = cvLoadImage("sound_break.jpg", 1);
	IplImage* img2 = cvLoadImage("fall.jpg", 1);

	funcs->ratioResize(img1, img2);

	cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win1", 100, 100);
	cvShowImage("win1", img1);
	cvNamedWindow("win2", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("win2", 150, 150);
	cvShowImage("win2", img2);

	cout << "img1: " << img1->width << " x " << img1->height << endl;
	cout << "img2: " << img2->width << " x " << img2->height << endl;
	cout << "RMS = " << funcs->rms(img1, img2) << endl;

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

    return app.exec();
}
