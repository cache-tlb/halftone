#include <cv.h>	
#include <highgui.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <functional>
#include <vector>
#include "SSIM.h"
#include "Ostromoukhov01.h"
#include "SAHalftoner.h"

using namespace std;

int main(int argc, char** argv)
{
	cv::Mat im = cv::imread("E:\\liubin\\projects\\SAH\\SAH\\images_legacy\\bat.png", 0);
    SAHer saher(im);
    saher.ComputeSAH();
    cv::imshow("sah", saher.GetResult());
    cv::waitKey();
	return 0;
}
