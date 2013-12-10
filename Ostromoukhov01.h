#pragma  once

#include <cv.h>

IplImage *OstromoukhovHalftone(IplImage *I);

template <typename T>
T clamp(const T& value, const T& low, const T& high) {
	return value < low ? low : (value > high ? high : value);
}
