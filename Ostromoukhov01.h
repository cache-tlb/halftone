#pragma  once

#include <cv.h>

// input type: 32fc1, output type: 8uc1
IplImage *OstromoukhovHalftone(IplImage *I);

cv::Mat OstromoukhovHalftone(cv::Mat I);


