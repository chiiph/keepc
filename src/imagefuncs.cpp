#include "imagefuncs.h"

bool ImageFuncs::VERBOSE = false;

void ImageFuncs::setVerbose(bool verbose){
    VERBOSE = verbose;
}

void ImageFuncs::crop(IplImage* &img, CvRect roi){
	IplImage *croppedImg;

	if (img->width <= 0 || img->height <= 0 || roi.width <= 0 || roi.height <= 0){
        cout << "Las dimensiones de la imagen a cortar y la región de interes deben ser distintas de 0." << endl << endl;
		exit(1);
	}
	if (img->depth != IPL_DEPTH_8U){
        cout << "La profundidad de la imagen a cortar debe ser IPL_DEPTH_8U (8)." << endl << endl;
		exit(1);
	}

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

    //En primer lugar se determina qué imagen debe ser redimendionada y en función de que factor,
    //dependiendo de las relaciones entre los lados de ambas imágenes.
	if(h1 > h2){
        if(w1 > w2){						//La imagen 1 es más alta y ancha que la imagen 2.
			resizeImg = 2;
			rate = (h1/h2 > w1/w2) ? h1/h2 : w1/w2;
		}
        else{								//La imagen 1 es más alta y la imagen 2 más ancha.
			if(h1/h2 > w2/w1){				//La diferencia de altura es mayor proporcionalmente.				
				resizeImg = 2;
				rate = h1/h2;
			}
			else{							//La diferencia de ancho es mayor proporcionalmente.				
				resizeImg = 1;
				rate = w2/w1;
			}
		}
	}
	else{
        if(w1 > w2){						//La imagen 2 es más alta y la imagen 1 más ancha.
			if(h2/h1 > w1/w2){				//La diferencia de altura es mayor proporcionalmente.				
				resizeImg = 1;
				rate = h2/h1;
			}
			else{							//La diferencia de ancho es mayor proporcionalmente.				
				resizeImg = 2;
				rate = w1/w2;
			}
		}
		else{								//La imagen 2 es tiene altura y ancho mayor igual que la imagen 1.
            if(h2 > h1 || w2 > w1){			//La imagen 2 es efectivamente más alta y/o ancha que la imagen 1.
				resizeImg = 1;
				rate = (h2/h1 > w2/w1) ? h2/h1 : w2/w1;
			}
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
                ImageFuncs::crop(img1, cvRect(0, 0, img2->width, img2->height));
                if(VERBOSE) cout << "Imagen 1 cortada a: " << img1->width << " x " << img1->height << endl << endl;
				break;

		case 2:	newImg = cvCreateImage(cvSize((int)(ceil(w2*rate)), (int)(ceil(h2*rate))), img2->depth, img2->nChannels);
				cvResize(img2, newImg);
				cvReleaseImage(&img2);
				img2 = newImg;
                if(VERBOSE) cout << "Imagen 2 redimensionada a: " << img2->width << " x " << img2->height << endl << endl;
                ImageFuncs::crop(img2, cvRect(0, 0, img1->width, img1->height));
                if(VERBOSE) cout << "Imagen 2 cortada a: " << img2->width << " x " << img2->height << endl << endl;
				break;

        default: if(VERBOSE) cout << "Las imagenes poseen las mismas dimensiones: " << img1->width << " x " << img1->height << endl << endl;
	}
}

double ImageFuncs::rms(IplImage* img1, IplImage* img2){
	uchar* data1 = (uchar*)img1->imageData;
	uchar* data2 = (uchar*)img2->imageData;
	int height = img1->height;
	int width = img1->width;
	int step = img1->widthStep;
	int channels = img1->nChannels;
    double dist;
    double chanAcc = 0;
    double pixAcc = 0;
    double rms = 0;
    double maxDist = 0;

    if(VERBOSE){
        cout << "Imagenes iniciando calculo RMS:" << endl;
		cout << "Imagen 1: " << img1->width << " x " << img1->height << endl;
		cout << "Imagen 2: " << img2->width << " x " << img2->height << endl << endl;
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			chanAcc = 0;
			for(int k=0; k<channels; k++){
				chanAcc += pow((data1[i*step + j*channels + k] - data2[i*step + j*channels + k]), 2);
			}
            dist = sqrt(chanAcc);	//Distancia euclí­dea entre pí­xeles: raíz((p1.R - p2.R)^2 + (p1.G - p2.G)^2 + (p1.B - p2.B)^2).
			if(dist > maxDist)
                maxDist = dist;		//Se actualiza la máxima distancia hallada entre píxeles.
			if(dist > 0)
                dist = log10(dist);	//Se considera el espacio logarítmico para favorecer el resultado de la normalización.
			pixAcc += pow(dist, 2);
		}
	}

    if(VERBOSE) cout << "Maxima distancia: " << maxDist << endl << endl;

	rms = sqrt(pixAcc / (width * height));	//Se promedia la distancia acumulada.
	if(maxDist > 0)
		rms = rms / log10(maxDist);			//Se normaliza el resultado para obtener el RMS final.
	return rms;
}

double ImageFuncs::rms(QImage* qImg1, QImage* qImg2, int img1x, int img1y){
    int objx = img1x;
    int objy = img1y;
    int objw = qImg1->width();
    int objh = qImg1->height();
    double dist;
    double chanAcc = 0;
    double pixAcc = 0;
    double rms = 1;
    double maxDist = 0;

    //En caso de que la imagen 1 se encuentre desplazada negativamente con respecto a la imagen 2,
    //la region sobrante de la primera es recortada.
    if(objx < 0){
        *qImg1 = qImg1->copy(objx*(-1), 0, qImg1->width()+objx, qImg1->height());
        objx = 0;
    }
    if(objy < 0){
        *qImg1 = qImg1->copy(0, objy*(-1), qImg1->width(), qImg1->height()+objy);
        objy = 0;
    }

    //Se realiza el calculo RMS teniendo en cuenta el desplazamiento en (x, y) de la imagen 1.
    int i=0, j=0;
    if(!(qImg2->width() < objx || qImg2->height() < objy)){
        for(i=objx; i<objx+objw; i++){
            for(j=objy; j<objy+objh; j++){
                if(i<qImg2->width() && j<qImg2->height()){
                    chanAcc = 0;
                    chanAcc += pow(qRed(qImg1->pixel(i-objx, j-objy)) - qRed(qImg2->pixel(i, j)), 2);
                    chanAcc += pow(qBlue(qImg1->pixel(i-objx, j-objy)) - qBlue(qImg2->pixel(i, j)), 2);
                    chanAcc += pow(qGreen(qImg1->pixel(i-objx, j-objy)) - qGreen(qImg2->pixel(i, j)), 2);

                    dist = sqrt(chanAcc);	//Distancia euclí­dea entre pí­xeles: raíz((p1.R - p2.R)^2 + (p1.G - p2.G)^2 + (p1.B - p2.B)^2).
                    if(dist > maxDist)
                        maxDist = dist;		//Se actualiza la máxima distancia hallada entre píxeles.
                    if(dist > 0)
                        dist = log10(dist);	//Se considera el espacio logarítmico para favorecer el resultado de la normalización.
                    pixAcc += pow(dist, 2);
                }
            }
        }
        rms = sqrt(pixAcc / (qImg1->width() * qImg1->height()));    //Se promedia la distancia acumulada.
        if(maxDist > 0)
            rms = rms / log10(maxDist);     //Se normaliza el resultado para obtener el RMS final.
    }
    if(VERBOSE)
        cout << "Dimension de la region analizada: " << i-objx << " x " << j-objy << endl << endl;
    return rms;
}

bool ImageFuncs::featuresBasedRMS(double &rms, IplImage* img1, IplImage* img2, IplImage* img1Color, IplImage* img2Color){
    IplImage* object = img1;
    IplImage* image = img2;
    IplImage* objColor = img1Color;
    IplImage* imgColor = img2Color;
    //Si es necesario, se invierte el orden de las imágenes para favorecer una transformación aumentativa.
    if(Utils::toggleRequired(img1, img2)){
        qDebug() << "TOOOOOGGLE";
        object = img2;
        image = img1;
        objColor = img2Color;
        imgColor = img1Color;
    }    

    cvNamedWindow("Imagen1", 1);
    cvMoveWindow("Imagen1", 100, 100);
    cvShowImage("Imagen1", object);
    cvNamedWindow("Imagen2", 1);
    cvMoveWindow("Imagen2", 150, 150);
    cvShowImage("Imagen2", image);

    QTransform transform;

    //Si no es posible la aplicación de una transformación, se retorna sin resultado.
    if(!Features::featuresBasedTransform(object, image, transform, objColor, imgColor))
        return false;

    QImage *qImg1 = Utils::cvToQt(objColor);
    QImage *qImg2 = Utils::cvToQt(imgColor);
    *qImg1 = qImg1->transformed(transform, Qt::SmoothTransformation);
    cout << "Transform: " << transform.dx() << ", " << transform.dy() << endl;

    rms = ImageFuncs::rms(qImg1, qImg2, transform.dx(), transform.dy());

    //ESCENA *********************************************************************
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    QGraphicsPixmapItem *pix = new QGraphicsPixmapItem(QPixmap::fromImage(*qImg2));
    pix->setPos(0,0);
    pix->setZValue(0);
    scene->addItem(pix);
    QGraphicsPixmapItem *pix2 = new QGraphicsPixmapItem(QPixmap::fromImage(*qImg1));
    pix2->setPos(0,0);
    //pix2->setTransform(transform, true);
    pix2->setZValue(1);
    scene->addItem(pix2);
    QGraphicsView *view = new QGraphicsView(scene);
    view->show(); 

    return true;
}
