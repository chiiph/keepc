#include "imagefuncs.h"
#include "iostream"
#include "math.h"

using namespace std;

//Constructor de la clase
//El argumento verbose indica si las funciones deben imprimir datos en pantalla.
ImageFuncs::ImageFuncs(int verbose){
	VERBOSE = verbose;
}

//Recorta la imagen img según la región de interés expresada (en caso de ser posible).
//La región de interés (roi) es determinada mediante un rectángulo de la forma (X, Y, Ancho, Alto).
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

//Recibe dos imágenes y adapta una de ellas para que sus tamaños resulten iguales.
void ImageFuncs::ratioResize(IplImage* &img1, IplImage* &img2){
	float h1 = img1->height;
	float h2 = img2->height;
	float w1 = img1->width;
	float w2 = img2->width;
	float rate = 0;
	int resizeImg = 0;
	IplImage* newImg;

	//En primer lugar se determina qué imagen debe ser redimendionada y en función de que factor,
	//dependiendo de las relaciones entre los lados de ambas imágenes.
	if(h1 > h2){
		if(w1 > w2){						//La imagen 1 es más alta y ancha que la imagen 2.
			cout << "Caso 1" << endl;
			resizeImg = 2;
			rate = (h1/h2 > w1/w2) ? h1/h2 : w1/w2;
		}
		else{								//La imagen 1 es más alta y la imagen 2 más ancha.
			if(h1/h2 > w2/w1){				//La diferencia de altura es mayor proporcionalmente.
				cout << "Caso 2" << endl;
				resizeImg = 2;
				rate = h1/h2;
			}
			else{							//La diferencia de ancho es mayor proporcionalmente.
				cout << "Caso 3" << endl;
				resizeImg = 1;
				rate = w2/w1;
			}
		}
	}
	else{
		if(w1 > w2){						//La imagen 2 es más alta y la imagen 1 más ancha.
			if(h2/h1 > w1/w2){				//La diferencia de altura es mayor proporcionalmente.
				cout << "Caso 4" << endl;
				resizeImg = 1;
				rate = h2/h1;
			}
			else{							//La diferencia de ancho es mayor proporcionalmente.
				cout << "Caso 5" << endl;
				resizeImg = 2;
				rate = w1/w2;
			}
		}
		else{								//La imagen 2 es más alta y ancha que la imagen 1.
			cout << "Caso 6" << endl;
			resizeImg = 1;
			rate = (h2/h1 > w2/w1) ? h2/h1 : w2/w1;
		}
	}

	//La imagen determinada es redimensionada manteniendo su aspect ratio.
	//Posteriormente, es recortada para que sus dimensiones resulten iguales a las de la otra imagen.
	//La región de interés utilizada es de la forma (0, 0, Ancho, Alto).
	switch(resizeImg){

		case 1:	newImg = cvCreateImage(cvSize((int)(ceil(w1*rate)), (int)(ceil(h1*rate))), img1->depth, img1->nChannels);
				cvResize(img1, newImg);
				cvReleaseImage(&img1);
				img1 = newImg;
				if(VERBOSE) cout << "Imagen 1 redimensionada a: " << img1->width << " x " << img1->height << endl << endl;
				this->crop(img1, cvRect(0, 0, img2->width, img2->height));
				if(VERBOSE) cout << "Imagen 1 cortada a: " << img1->width << " x " << img1->height << endl << endl;
				break;

		case 2:	newImg = cvCreateImage(cvSize((int)(ceil(w2*rate)), (int)(ceil(h2*rate))), img2->depth, img2->nChannels);
				cvResize(img2, newImg);
				cvReleaseImage(&img2);
				img2 = newImg;
				if(VERBOSE) cout << "Imagen 2 redimensionada a: " << img2->width << " x " << img2->height << endl << endl;
				this->crop(img2, cvRect(0, 0, img1->width, img1->height));
				if(VERBOSE) cout << "Imagen 2 cortada a: " << img2->width << " x " << img2->height << endl << endl;
				break;
	}
}

//Recibe dos imágenes y determina el error RMS que existe entre ellas.
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

	if(VERBOSE){
		cout << "Imágenes iniciando cálculo RMS:" << endl;
		cout << "Imagen 1: " << img1->width << " x " << img1->height << endl;
		cout << "Imagen 2: " << img2->width << " x " << img2->height << endl << endl;
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			chanAcc = 0;
			for(int k=0; k<channels; k++){
				chanAcc += pow((data1[i*step + j*channels + k] - data2[i*step + j*channels + k]), 2);
			}
			dist = sqrt(chanAcc);	//Distancia euclídea entre píxeles: raíz((p1.R - p2.R)^2 + (p1.G - p2.G)^2 + (p1.B - p2.B)^2).
			if(dist > maxDist)
				maxDist = dist;		//Se actualiza la máxima distancia hallada entre píxeles.
			if(dist > 0)
				dist = log10(dist);	//Se considera el espacio logarítmico para favorecer el resultado de la normalización.
			pixAcc += pow(dist, 2);
		}
	}

	if(VERBOSE) cout << "Máxima distancia: " << maxDist << endl << endl;

	rms = sqrt(pixAcc / (width * height));	//Se promedia la distancia entre los píxeles de ambas imágenes.
	if(maxDist > 0)
		rms = rms / log10(maxDist);			//Se normaliza el resultado para obtener el RMS final.
	return rms;
}
