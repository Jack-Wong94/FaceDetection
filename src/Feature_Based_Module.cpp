#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <time.h>
#include "FaceDetection.h"

/**
 ** Name: Jack Wong
 ** Module: Feature-Based Face detection module
 ** The feature-based face detection module is implemented based on haar's cascade classifier. 
 ** This module extracts the likely region of human facial features from the input image.
**/

using namespace cv;
using namespace std;

//Draw box around face features such as eyes.
void DrawFeatureBox(Mat imgOriginal, vector<Rect> features)
{
	for (int i = 0; i < features.size(); i++)
	{
		Rect r = features[i];
		rectangle(imgOriginal, Point(r.x, r.y), Point(r.x + r.width, r.y + r.height), CV_RGB(0,255,0));
	}
}

//Locate the position of the face features
vector<Rect> LocateFeatureBox(Mat imgOriginal,string cascadeClassifier)
{
	CascadeClassifier cascade;
	vector<Rect> features;
	cascade.load(cascadeClassifier);
	cascade.detectMultiScale(imgOriginal, features, 1.2 , 3);
	return features;
}


//Use eye cascade classifier to detect and draw the eye feature in the picture
Mat EyeDetection(Mat imgOriginal)
{
	vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
	DrawFeatureBox(imgOriginal, eyes);
	return imgOriginal;
}

//Use face cascade classifier to detect and draw the face feature in the picture
Mat FaceDetection(Mat imgOriginal)
{
	vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
	DrawFeatureBox(imgOriginal, faces);
	return imgOriginal;
	
}
