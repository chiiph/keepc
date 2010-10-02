#ifndef IMAGEFUNCS_H
#define IMAGEFUNCS_H
#include "cv.h"
#include "highgui.h"

class ImageFuncs{

public:

    ImageFuncs();

	void crop(IplImage* &img, CvRect roi);

	void ratioResize(IplImage* &img1, IplImage* &img2);

	float rms(IplImage* img1, IplImage* img2);
};

#endif // IMAGEFUNCS_H
