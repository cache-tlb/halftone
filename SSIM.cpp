/*
 * The equivalent of Zhou Wang's SSIM matlab code using OpenCV.
 * from http://www.cns.nyu.edu/~zwang/files/research/ssim/index.html
 * The measure is described in :
 * "Image quality assessment: From error measurement to structural similarity"
 * C++ code by Rabah Mehdi. http://mehdi.rabah.free.fr/SSIM
 *
 * This implementation is under the public domain.
 * @see http://creativecommons.org/licenses/publicdomain/
 * The original work may be under copyrights. 
 */

#include <cv.h>
#include <highgui.h>

IplImage *ssim(IplImage *input1, IplImage *input2) {
	assert(input1 && input2);
	assert(input1->width == input2->width && input1->height == input2->height);

	double C1 = 6.5025/255.0/255.0, C2 = 58.5225/255.0/255.0;
	IplImage
		*img1=NULL, *img2=NULL, *img1_img2=NULL,
		*img1_sq=NULL, *img2_sq=NULL,
		*mu1=NULL, *mu2=NULL,
		*mu1_sq=NULL, *mu2_sq=NULL, *mu1_mu2=NULL,
		*sigma1_sq=NULL, *sigma2_sq=NULL, *sigma12=NULL,
		*ssim_map=NULL, *temp1=NULL, *temp2=NULL, *temp3=NULL;

	int x = input1->width, y = input2->height;
	int nChan = input1->nChannels, d = IPL_DEPTH_32F;

	CvSize size = cvSize(x,y);
	img1 = input1;
	img2 = input2;

	img1_sq = cvCreateImage( size, d, nChan);
	img2_sq = cvCreateImage( size, d, nChan);
	img1_img2 = cvCreateImage( size, d, nChan);

	cvPow( img1, img1_sq, 2 );
	cvPow( img2, img2_sq, 2 );
	cvMul( img1, img2, img1_img2, 1 );

	mu1 = cvCreateImage( size, d, nChan);
	mu2 = cvCreateImage( size, d, nChan);

	mu1_sq = cvCreateImage( size, d, nChan);
	mu2_sq = cvCreateImage( size, d, nChan);
	mu1_mu2 = cvCreateImage( size, d, nChan);


	sigma1_sq = cvCreateImage( size, d, nChan);
	sigma2_sq = cvCreateImage( size, d, nChan);
	sigma12 = cvCreateImage( size, d, nChan);

	temp1 = cvCreateImage( size, d, nChan);
	temp2 = cvCreateImage( size, d, nChan);
	temp3 = cvCreateImage( size, d, nChan);

	ssim_map = cvCreateImage( size, d, nChan);
	/*************************** END INITS **********************************/


	//////////////////////////////////////////////////////////////////////////
	// PRELIMINARY COMPUTING
	cvSmooth( img1, mu1, CV_GAUSSIAN, 11, 11, 1.5 );
	cvSmooth( img2, mu2, CV_GAUSSIAN, 11, 11, 1.5 );

	cvPow( mu1, mu1_sq, 2 );
	cvPow( mu2, mu2_sq, 2 );
	cvMul( mu1, mu2, mu1_mu2, 1 );


	cvSmooth( img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq );

	cvSmooth( img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq );

	cvSmooth( img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma12, 1, mu1_mu2, -1, 0, sigma12 );


	//////////////////////////////////////////////////////////////////////////
	// FORMULA

	// (2*mu1_mu2 + C1)
	cvScale( mu1_mu2, temp1, 2 );
	cvAddS( temp1, cvScalarAll(C1), temp1 );

	// (2*sigma12 + C2)
	cvScale( sigma12, temp2, 2 );
	cvAddS( temp2, cvScalarAll(C2), temp2 );

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	cvMul( temp1, temp2, temp3, 1 );

	// (mu1_sq + mu2_sq + C1)
	cvAdd( mu1_sq, mu2_sq, temp1 );
	cvAddS( temp1, cvScalarAll(C1), temp1 );

	// (sigma1_sq + sigma2_sq + C2)
	cvAdd( sigma1_sq, sigma2_sq, temp2 );
	cvAddS( temp2, cvScalarAll(C2), temp2 );

	// ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvMul( temp1, temp2, temp1, 1 );

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvDiv( temp3, temp1, ssim_map, 1 );

	// release the temp images
	//cvReleaseImage(&img1); cvReleaseImage(&img2); 
	cvReleaseImage(&img1_img2);
	cvReleaseImage(&img1_sq); cvReleaseImage(&img2_sq); 
	cvReleaseImage(&mu1); cvReleaseImage(&mu2); 
	cvReleaseImage(&mu1_sq); cvReleaseImage(&mu2_sq); cvReleaseImage(&mu1_mu2);
	cvReleaseImage(&sigma1_sq); cvReleaseImage(&sigma2_sq); cvReleaseImage(&sigma12);
	cvReleaseImage(&temp1); cvReleaseImage(&temp2); cvReleaseImage(&temp3);
	
	return ssim_map;
}

cv::Mat ssim(const cv::Mat &input1, const cv::Mat &input2) {
    /*CV_Assert(input1.cols == input2.cols && input1.rows == input2.rows);
    double C1 = 6.5025/255.0/255.0, C2 = 58.5225/255.0/255.0;
    cv::Mat
        img1, img2, img1_img2,
        img1_sq, img2_sq,
        mu1, mu2,
        mu1_sq, mu2_sq, mu1_mu2,
        sigma1_sq, sigma2_sq, sigma12,
        ssim_map, temp1, temp2, temp3;

    int x = input1.cols, y = input2.rows;
    int nChan = input1.channels(), d = IPL_DEPTH_64F;
    int mType = CV_MAKE_TYPE(d, nChan);
    cv::Size size = cv::Size(x,y);

    img1 = input1;
    img2 = input2;

    img1_sq.create(size, mType);
    img2_sq.create(size, mType);
    img1_img2.create(size, mType);

    cv::pow(img1, 2, img1_sq);
    cv::pow(img2, 2, img2_sq);
    cv::multiply(img1, img2, img1_img2, 1);

    mu1.create( size, mType);
    mu2.create( size, mType);

    mu1_sq.create( size, mType);
    mu2_sq.create( size, mType);
    mu1_mu2.create( size, mType);


    sigma1_sq.create( size, mType);
    sigma2_sq.create( size, mType);
    sigma12.create( size, mType);

    temp1.create( size, mType);
    temp2.create( size, mType);
    temp3.create( size, mType);

    ssim_map.create( size, mType);
    /////////////////////////////// END INITS ////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // PRELIMINARY COMPUTING
    
    cv::GaussianBlur( img1, mu1, cv::Size(11,11), 1.5);
    cv::GaussianBlur( img2, mu2, cv::Size(11,11), 1.5);
    //cvSmooth( img1, mu1, CV_GAUSSIAN, 11, 11, 1.5 );
    //cvSmooth( img2, mu2, CV_GAUSSIAN, 11, 11, 1.5 );

    cv::pow(mu1, 2, mu1_sq);
    cv::pow(mu2, 2, mu2_sq);
    cv::multiply(mu1, mu2, mu1_mu2, 1);


    //cvSmooth( img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5 );
    cv::GaussianBlur(img1_sq, sigma1_sq, cv::Size(11,11), 1.5);
    //cvAddWeighted( sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq );
    cv::addWeighted(sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq);

    //cvSmooth( img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5 );
    cv::GaussianBlur(img2_sq, sigma2_sq, cv::Size(11,11), 1.5);
    //cvAddWeighted( sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq );
    cv::addWeighted(sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq);

    //cvSmooth( img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5 );
    cv::GaussianBlur(img1_img2, sigma12, cv::Size(11,11), 1.5);
    //cvAddWeighted( sigma12, 1, mu1_mu2, -1, 0, sigma12 );
    cv::addWeighted(sigma12, 1, mu1_mu2, -1, 0, sigma12);


    //////////////////////////////////////////////////////////////////////////
    // FORMULA

    // (2*mu1_mu2 + C1)
    //cvScale( mu1_mu2, temp1, 2 );
    //cvAddS( temp1, cvScalarAll(C1), temp1 );

    temp1 = mu1_mu2 * 2.0;
    cv::add(temp1, cv::Scalar(C1,C1,C1), temp1);

    // (2*sigma12 + C2)
    //cvScale( sigma12, temp2, 2 );
    //cvAddS( temp2, cvScalarAll(C2), temp2 );
    temp2 = sigma12 * 2.0;
    cv::add(temp2, cv::Scalar(C2,C2,C2), temp2);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    //cvMul( temp1, temp2, temp3, 1 );
    cv::multiply(temp1, temp2, temp3, 1);

    // (mu1_sq + mu2_sq + C1)
    //cvAdd( mu1_sq, mu2_sq, temp1 );
    //cvAddS( temp1, cvScalarAll(C1), temp1 );
    cv::add( mu1_sq, mu2_sq, temp1);
    cv::add(temp1, cv::Scalar(C1,C1,C1), temp1);

    // (sigma1_sq + sigma2_sq + C
    //cvAdd( sigma1_sq, sigma2_sq, temp2 );
    //cvAddS( temp2, cvScalarAll(C2), temp2 );
    cv::add(sigma1_sq, sigma2_sq, temp2);
    cv::add(temp2, cv::Scalar(C2,C2,C2), temp2);

    // ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    //cvMul( temp1, temp2, temp1, 1 );
    cv::multiply(temp1, temp2, temp1, 1);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    //cvDiv( temp3, temp1, ssim_map, 1 );
    cv::divide(temp3, temp1, ssim_map);
    */
    IplImage img1 = input1, img2 = input2;
    IplImage *temp_ssim = ssim(&img1, &img2);
    cv::Mat ssim_map = cv::cvarrToMat(temp_ssim).clone();
    cvReleaseImage(&temp_ssim);
    return ssim_map;
}
