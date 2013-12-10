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

using namespace std;

double rand_double() {
	return double(rand()) / double(RAND_MAX);
}

int depth = IPL_DEPTH_64F;
IplImage *I;
IplImage *Ih;
int w, h;

//vector<int> blacks;
//vector<int> whites;
//int b_size, w_size;

void TonePreserveInit() {
	//Ih = cvCloneImage(I);
	//cvZero(Ih);
	//// assume I is gray scale
	//CvScalar sumColor = cvSum(I);
	//double sumLum = sumColor.val[0];
	//int curLum = 0;

	//while (curLum < sumLum) {
	//	int idx1 = rand() % h;
	//	int idx2 = rand() % w;
	//	if (cvGet2D(Ih, idx1, idx2).val[0]) continue;
	//	cvSet2D(Ih, idx1, idx2, cvScalar(1));
	//	int idx = idx1*w + idx2;
	//	curLum += 1;
	//}
	Ih = OstromoukhovHalftone(I);
	
	cvSetImageROI(I, cvRect(1,1,w-2,h-2));
	cvSetImageROI(Ih, cvRect(1,1,w-2,h-2));
	IplImage *tmpI = cvCreateImage(cvSize(w-2,h-2),I->depth, I->nChannels);
	IplImage *tmpIh = cvCloneImage(tmpI);

	cvCopy(I, tmpI);
	cvCopy(Ih, tmpIh);

	w = w -2; h = h - 2;
	I = tmpI;
	Ih = tmpIh;

	tmpI = cvCloneImage(I);
	tmpIh = cvCloneImage(Ih);
	cvCvtScale(tmpI, tmpI, 255.0);
	cvCvtScale(tmpIh, tmpIh, 255.0);
	//cvSaveImage("I.png", tmpI);
	//cvSaveImage("Ih.png", tmpIh);
	//printf("%d %d %d %d\n", I->width, I->height, w, h);

	//Ih = dart_throw_halftone(I);

	/*for (int i = 0; i < w*h; i++) {
	if (cvGet1D(Ih, i).val[0] == 0.0) blacks.push_back(i);
	else whites.push_back(i);
	}


	b_size = blacks.size();
	w_size = whites.size();

	printf("%d %d\n", b_size, w_size);*/
	//printf("%lf\n", sumLum);
}


double objective() {
	double wg = .5, wt = .5;

	IplImage *ssim_map = ssim(I, Ih);
	CvScalar index_scalar = cvAvg( ssim_map );
	double mssim = index_scalar.val[0];

	int d = depth;

	IplImage *gI = cvCreateImage(cvGetSize(I), d, I->nChannels);
	IplImage *gIh = cvCloneImage(gI);
	IplImage *se = cvCloneImage(gI);

	cvSmooth(I, gI, CV_GAUSSIAN, 11, 11);
	cvSmooth(Ih, gIh, CV_GAUSSIAN, 11, 11);
	cvSub(gI, gIh, se);
	cvMul(se, se, se);
	double GIIH = cvAvg(se).val[0];

	//printf("%lf %lf\n", mssim, GIIH);

	cvReleaseImage(&ssim_map);
	cvReleaseImage(&gI);
	cvReleaseImage(&gIh);
	cvReleaseImage(&se);
	return wg * (GIIH) + wt * (1. - mssim);
}

void get_sah() {
	srand(0);
	std::uniform_int_distribution<int> distribution(0, w*h*100);
	std::mt19937 engine;
	auto randint = std::bind(distribution, engine);

	TonePreserveInit();
	double E_old = objective();
	double temperature = .2;
	double limit = .01;
	double AnnealFactor = .8;
	//int K = I->width * I->height;
	int block_size = 3;
	while (temperature > limit) {
		for (int block_i = 0; block_i < h; block_i += block_size) for (int block_j = 0; block_j < w; block_j += block_size) {
			//RandomSwap(Ih, r1, c1, r2, c2);
			// rand swap
			//int rand1 = randint() % b_size, rand2 = randint() % w_size;
			//int idx1 = blacks[rand1], idx2 = whites[rand2];
			//cvSet1D(Ih, idx1, cvScalar(1));
			//cvSet1D(Ih, idx2, cvScalar(0));

			vector<pair<int,int> > b_indices, w_indices;
			for (int ii = 0; ii < block_size && block_i + ii < h; ii++) {
				for (int jj = 0; jj < block_size && block_j + jj < w; jj++) {
					int i = block_i + ii, j = block_j + jj;
					if (cvGet2D(Ih, i, j).val[0] > 0) w_indices.push_back(pair<int,int>(i, j));
					else b_indices.push_back(pair<int,int>(i, j));
				}
			}

			printf("%d %d ", block_i, block_j);
			printf("%d %d\n", b_indices.size(), w_indices.size());
			if (b_indices.empty() || w_indices.empty()) continue;
			// else try block_size x block_size times of swap.

			for (int k = 0; k < block_size * block_size; k++){
				int rand1 = rand() % b_indices.size(), rand2 = rand() % w_indices.size();
				pair<int,int> idx1 = b_indices[rand1], idx2 = w_indices[rand2];
				cvSet2D(Ih, idx1.first, idx1.second, cvScalar(1));
				cvSet2D(Ih, idx2.first, idx2.second, cvScalar(0));
				double E_new = objective();
				double deltaE = E_new - E_old;
				//if (deltaE > 0) printf("%.2lf %.2lf %lf\n", E_old, E_new, exp(min(0.0, -deltaE / temperature * w*h)));
				//if (deltaE < 0) {
				if ( deltaE < 0. || rand_double() < exp( - deltaE / temperature * w*h ) ) {
					// accept
					E_old = E_new;
					b_indices[rand1] = idx2;
					w_indices[rand2] = idx1;
					//if (i%1000==0) printf("accept\n");
				}
				else {
					// reject and undo swap
					cvSet2D(Ih, idx1.first, idx1.second, cvScalar(0));
					cvSet2D(Ih, idx2.first, idx2.second, cvScalar(1));
					//if printf("reject\n");
				}
			}
			if (block_j == 0) {
				printf("begin show;");
				cvShowImage("ih", Ih);cvWaitKey();
				printf("end show\n");
			}
		}
		printf("%lf\n", temperature);
		temperature *= AnnealFactor;
	}
}

int main(int argc, char** argv)
{
	IplImage *img2 = cvLoadImage("e:/liubin/project/SAH/SAH/2.png", 0);
	IplImage *img = cvCreateImage(cvSize(img2->width / 1, img2->height / 1), IPL_DEPTH_8U, img2->nChannels);
	cvResize(img2, img);
	IplImage *fimg = cvCreateImage(cvGetSize(img), IPL_DEPTH_64F, img->nChannels);
	cvCvtScale(img, fimg, 1.0/255.);

	I = fimg;
	w = I->width;
	h = I->height;
	get_sah();
	//IplImage *ost = OstromoukhovHalftone(I);
	cvShowImage("cat", I);
	cvShowImage("halftone", Ih);
	cvWaitKey();
	
	return 0;
}
