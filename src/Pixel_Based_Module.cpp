#include <stdio.h>
#include "FaceDetection.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <time.h>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>

/**
 ** Name: Jack Wong
 ** Module: Pixel-Based Face detection module
 ** The pixel-based face detection module is implemented based on the 
 ** skin color segmentation algorithm. This module searches every pixels
 ** in the input image and determine if the pixel is in human skin tone.
**/

using namespace cv;
using namespace std;

//Sequential skin color filters
Mat Filter(Mat imgOriginal)
{
	Mat imgResult = imgOriginal.clone();
	
	Mat imgHSV = imgOriginal.clone();
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	Mat imgYCrCb = imgOriginal.clone();
	cvtColor(imgOriginal, imgYCrCb, COLOR_BGR2YCrCb);

	Mat imgRGB = imgOriginal.clone();
	cvtColor(imgOriginal, imgRGB, COLOR_BGR2RGB);

	for (int i = 0; i < imgOriginal.rows; i++)
	{
		for (int j = 0; j < imgOriginal.cols; j++)
		{
			//HSV filter
			Vec3b HSVColor = imgHSV.at<Vec3b>(Point(j, i));
			int H = HSVColor.val[0];

			//human skin tone threshold in HSV color space
			if (19 < H && H < 240)
			{
				imgHSV.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
			}

			//YCrCb filter
			Vec3b YCrCbColor = imgYCrCb.at<Vec3b>(Point(j,i));
			int Cr = YCrCbColor.val[1];
			int Cb = YCrCbColor.val[2];

			//human skin tone threshold in YCrCb color space
			if (Cr < 125 || Cr > 160 || Cb < 102 || Cb > 128)
			{
				imgYCrCb.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
			}

			//RGB filter
			Vec3b RGBColor = imgRGB.at<Vec3b>(Point(j,i));
			int R = RGBColor.val[0];
			int G = RGBColor.val[1];
			int B = RGBColor.val[2];

			//humsn skin tone in RGB color space.
			if (B > 0.836*G + 44 || B < 0.836*G - 14 || B > 0.78*G + 42 || B < 0.79*G - 67)
			{
				imgRGB.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				//array[i * (j+1) + j] = 0;
			}
			
		}
	}
	return imgResult;
}

//HSV filter
Mat HSVFilter(Mat imgOriginal, Mat imgResult)
{
	//convert the image to HSV color space.
	Mat imgHSV = imgOriginal.clone();
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < imgHSV.rows; i++)
	{
		for (int j = 0; j < imgHSV.cols; j++)
		{
			Vec3b HSVColor = imgHSV.at<Vec3b>(Point(j, i));
			int H = HSVColor.val[0];
			
			//human skin tone threshold in HSV color space
			if (19 < H && H < 240)
			{
				imgHSV.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				
			}
			
		}
	}
	
	return imgResult;
}

//YCrCb filter
Mat YCrCbFilter(Mat imgOriginal, Mat imgResult)
{

	//convert the image to YCrCb color space.
	Mat imgYCrCb = imgOriginal.clone();
	cvtColor(imgOriginal, imgYCrCb, COLOR_BGR2YCrCb);

	for (int i = 0; i < imgYCrCb.rows; i++)
	{
		for (int j = 0; j < imgYCrCb.cols; j++)
		{
			Vec3b YCrCbColor = imgYCrCb.at<Vec3b>(Point(j,i));
			int Cr = YCrCbColor.val[1];
			int Cb = YCrCbColor.val[2];

			//human skin tone threshold in YCrCb color space
			if (Cr < 125 || Cr > 160 || Cb < 102 || Cb > 128)
			{
				imgYCrCb.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				
			}
		}
	}
	
	return imgResult;
}

//RGB filter
Mat RGBFilter(Mat imgOriginal, Mat imgResult)
{
	//convert image to RGB color space.
	Mat imgRGB = imgOriginal.clone();
	cvtColor(imgOriginal, imgRGB, COLOR_BGR2RGB);

	for (int i = 0; i < imgRGB.rows; i++)
	{
		for (int j = 0; j < imgRGB.cols; j++)
		{
			Vec3b RGBColor = imgRGB.at<Vec3b>(Point(j,i));
			int R = RGBColor.val[0];
			int G = RGBColor.val[1];
			int B = RGBColor.val[2];

			//human skin tone threshold in RGB color space
			if (B > 0.836*G + 44 || B < 0.836*G - 14 || B > 0.78*G + 42 || B < 0.79*G - 67)
			{
				imgRGB.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				
			}
		}
	}
	
	return imgResult;
}

//Multi-process skin color filters. Use skin color to locate the possible location of the face in the picture.
Mat SkinColorDetection(Mat imgOriginal)
{
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	//initialize image container for the result image.
	Mat imgResult;
	imgResult = imgOriginal.clone();
	
	for (int i = 0; i < 3; i++)
	{
		if (i==0)
		{
			if (fork()==0)
			{
				imgResult = HSVFilter(imgOriginal, imgResult);
				imwrite("HSV.jpg",imgResult,param);
				exit(0);
			}
		}else if (i==1)
		{
			if (fork()==0)
			{
				imgResult = YCrCbFilter(imgOriginal, imgResult);
				imwrite("YCrCb.jpg",imgResult,param);
				exit(0);
			}
		}else if (i==2)
		{
			if (fork()==0)
			{
				imgResult = RGBFilter(imgOriginal, imgResult);
				imwrite("RGB.jpg",imgResult,param);
				exit(0);
			}
		}
	}
	
	
	
	
	while (wait(NULL) > 0){};

	//Read in the output image from these color space filters.
	Mat imgHSV = imread("HSV.jpg",1);
	Mat imgYCrCb = imread("YCrCb.jpg",1);
	Mat imgRGB = imread("RGB.jpg",1);
	
	//Do a bitwise operation to filter the unnecessary pixel.
	cv::bitwise_and(imgHSV,imgYCrCb,imgResult);
	cv::bitwise_and(imgResult,imgRGB,imgResult);
	
	return imgResult;
}
