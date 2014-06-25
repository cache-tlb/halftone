#pragma once

#include <cv.h>

// will process float point input image
IplImage *ssim(IplImage *input1, IplImage *input2);

cv::Mat ssim(const cv::Mat &input1, const cv::Mat &input2);
