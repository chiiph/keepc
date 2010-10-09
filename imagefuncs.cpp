#include "imagefuncs.h"
#include "iostream"
#include "math.h"

using namespace std;

ImageFuncs::ImageFuncs(){}

void ImageFuncs::crop(IplImage* &img, CvRect roi){
	IplImage *croppedImg;

	if (img->width <= 0 || img->height <= 0 || roi.width <= 0 || roi.height <= 0)
		exit(1);
	if (img->depth != IPL_DEPTH_8U)
		exit(1);

	cvSetImageROI((IplImage*)img, roi);
	croppedImg = cvCreateImage(cvSize(roi.width, roi.height), img->depth, img->nChannels);
	cvCopy(img, croppedImg);
	cvResetImageROI(img);
	cvReleaseImage(&img);
	img = croppedImg;
}

void ImageFuncs::ratioResize(IplImage* &img1, IplImage* &img2){
	float h1 = img1->height;
	float h2 = img2->height;
	float w1 = img1->width;
	float w2 = img2->width;
	float rate = 0;
	int resizeImg = 0;
	IplImage* newImg;

	if(h1 > h2){
		if(w1 > w2){
			resizeImg = 2;
			rate = (h1/h2 > w1/w2) ? h1/h2 : w1/w2;
		}
		else{
			if(h1/h2 > w2/w1){
				resizeImg = 2;
				rate = h1/h2;
			}
			else{
				resizeImg = 1;
				rate = w2/w1;
			}
		}
	}
	else{
		if(w1 > w2){
			if(h2/h1 > w1/w2){
				resizeImg = 1;
				rate = h2/h1;
			}
			else{
				resizeImg = 2;
				rate = w1/w2;
			}
		}
		else{
			resizeImg = 1;
			rate = (h2/h1 > w2/w1) ? h2/h1 : w2/w1;
		}
	}

	switch(resizeImg){

		case 1:	newImg = cvCreateImage(cvSize((int)(ceil(w1*rate)), (int)(ceil(h1*rate))), img1->depth, img1->nChannels);
				cvResize(img1, newImg);
				cvReleaseImage(&img1);
				img1 = newImg;
				this->crop(img1, cvRect(0, 0, img2->width, img2->height));
				break;

		case 2:	newImg = cvCreateImage(cvSize((int)(ceil(w2*rate)), (int)(ceil(h2*rate))), img2->depth, img2->nChannels);
				cvResize(img2, newImg);
				cvReleaseImage(&img2);
				img2 = newImg;
				this->crop(img2, cvRect(0, 0, img1->width, img1->height));
				break;
	}
}

float ImageFuncs::rms(IplImage* img1, IplImage* img2){
	uchar* data1 = (uchar*)img1->imageData;
	uchar* data2 = (uchar*)img2->imageData;
	int height = img1->height;
	int width = img1->width;
	int step = img1->widthStep;
	int channels = img1->nChannels;
	float dist;
	float chanAcc = 0;
	float pixAcc = 0;
	float rms = 0;
	float maxDist = 0;

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			chanAcc = 0;
			for(int k=0; k<channels; k++){
				chanAcc += pow((data1[i*step + j*channels + k] - data2[i*step + j*channels + k]), 2);
			}
			dist = sqrt(chanAcc);
			if(dist > 0) dist = log10(dist);
			if(dist > maxDist) maxDist = dist;
			pixAcc += pow(dist, 2);
		}
	}
	rms = sqrt(pixAcc / (width * height));
	cout << "Máxima distancia: " << maxDist << endl;
	if(maxDist != 0) rms = rms / maxDist;
	return rms;

	//sqrt((distancia(pixel1.1,pixel2.1)^2 + distancia(pixel1.2,pixel2.2)^2 + ... + distancia(pixel1.N,pixel2.N)^2)/N)
}
