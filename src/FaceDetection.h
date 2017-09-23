#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

/**
 ** Name: Jack Wong
 ** Face detection header
 ** Method signature of modules in the face detection system.
**/

using namespace cv;
using namespace std;

//Input module
Mat ImageInput(int argc, char** argv);
void ImageProcess(Mat image,string command,bool showImage,string fileName);

//Pixel-based face detection module;
Mat Filter(Mat imgOriginal);
Mat HSVFilter(Mat imgOriginal, Mat imgResult);
Mat YCrCbFilter(Mat imgOriginal, Mat imgResult);
Mat RGBFilter(Mat imgOriginal, Mat imgResult);
Mat SkinColorDetection(Mat imgOriginal);

//Feature-based face detection module;
void DrawFeatureBox(Mat imgOriginal, vector<Rect> features);
vector<Rect> LocateFeatureBox(Mat imgOriginal,string cascadeClassifier);
Mat EyeDetection(Mat imgOriginal);
Mat FaceDetection(Mat imgOriginal);
