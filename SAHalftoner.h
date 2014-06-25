#pragma once

#include <cv.h>
#include <highgui.h>
#include <string>

class SAHer {
public:
    SAHer(cv::Mat &im8uc1);
    ~SAHer() {}

    void ComputeSAH(const cv::Mat &sal = cv::Mat());
    cv::Mat GetResult();

private:
    static const int IMG_TYPE;
    cv::Mat src_image_, halftone_image_;
    int w_, h_;
    void HalfToneInit();
    float Objective(const cv::Rect &roi);
};
