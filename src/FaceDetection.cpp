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
 ** Face Detection System
 ** The real-time face detection system is consisted of pixel-based and feature-based
 ** face detection module. The goal of our project is to implement the face detection system
 ** on the embedded platform and investigate the performance difference between hardware component
 ** and software component. The processing time of each input image is tested using different algorithms
 ** 
**/

using namespace cv;
using namespace std;

//Print the binary array output from skin color detection
void PrintArray(int* array, int rows, int cols, string fileName)
{
	std::ofstream output_file;
	output_file.open(fileName);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//cout << array[i * (j+1) + j];	
			output_file << array[i * (j+1) + j];		
		}
		output_file << "\n";
	}
	output_file.close();
}



//Partial skin filter: Locate the facial feature boxes. For each feature box, check if it contains more than 50% black pixels. If true, the box is invalid.
Mat PartialSkinFilter(Mat imgOriginal)
{
	Mat imgSkinColorResult = imgOriginal.clone();
	
	//jpeg file param
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	//multi-process: each process handles eye cascade classifier and face cascade classifier.
	for (int i = 0;i < 2;i++)
	{
		if (i==0)
		{
			if (fork()==0)
			{
				vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
				std::ofstream output_file;
				output_file.open("FeatureLocation/Eye.txt");

				//Store the coordinate to Eye.txt
				for(int j = 0; j < eyes.size(); j++)
				{
					Rect r = eyes[j];
					output_file << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
				}
				output_file.close();
				exit(0);
			}
		}
		if (i==1)
		{
			if (fork()==0)
			{
				vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
				std::ofstream output_file;
				output_file.open("FeatureLocation/Face.txt");

				//store the coordinate to Face.txt.
				for(int j = 0; j < faces.size(); j++)
				{
					Rect r = faces[j];
					output_file << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
				}
				output_file.close();
				exit(0);
			}
		}
		
	}
	while (wait(NULL)>0){}
	int x,y,width,height;
	std::ifstream face_input("FeatureLocation/Face.txt");
	
	//draw the rectangle using the stored coordinate 
	while (face_input >> x >> y >> width >> height)
	{
		rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
		
	}
	face_input.close();
	std::ifstream eye_input("FeatureLocation/Eye.txt");

	while (eye_input >> x >> y >> width >> height)
	{
		
		Rect r1(x,y,width,height);
		Mat approxFace = imgSkinColorResult(r1).clone();
		approxFace = Filter(approxFace);
		cvtColor(approxFace,approxFace,CV_BGR2GRAY);
		float PercentageZeroPixel = float(float((width*height) - countNonZero(approxFace))/(width*height));	
			
		//If the feature box has less than 50% black pixels, it is a valid box.
		if (PercentageZeroPixel<0.5)
		{
			rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
		}
	}
	eye_input.close();
	return imgSkinColorResult;

}

//Multi Process: Fork the parent process so each child process handle one face detection module.
Mat MultiProcess(Mat imgOriginal)
{
	Mat imgSkinColorResult = imgOriginal.clone();
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	//Create 2 child process to handle the Featue-based face detection module.
	for (int i = 0;i < 2;i++)
	{
		if (i==0)
		{
			if (fork()==0)
			{
				vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
				std::ofstream output_file;
				output_file.open("FeatureLocation/Eye.txt");
				for(int j = 0; j < eyes.size(); j++)
				{
					Rect r = eyes[j];
					output_file << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
				}
				output_file.close();
				exit(0);
			}
		}
		if (i==1)
		{
			if (fork()==0)
			{
				vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
				std::ofstream output_file;
				output_file.open("FeatureLocation/Face.txt");
				for(int j = 0; j < faces.size(); j++)
				{
					Rect r = faces[j];
					output_file << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
				}
				output_file.close();
				exit(0);
			}
		}
		
		
	}
	//Parent process: handles the pixel-based face detection module.
	imgSkinColorResult = SkinColorDetection(imgOriginal);
	while(wait(NULL)>0){}

	//Draw the feature box on the output of pixel-based face detection module.
	int x,y,width,height;
	std::ifstream face_input("FeatureLocation/Face.txt");
	while (face_input >> x >> y >> width >> height)
	{
		rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
		
	}
	face_input.close();
	std::ifstream eye_input("FeatureLocation/Eye.txt");
	while (eye_input >> x >> y >> width >> height)
	{
		
		Rect r1(x,y,width,height);
		Mat approxFace = imgSkinColorResult(r1).clone();
		cvtColor(approxFace,approxFace,CV_BGR2GRAY);
		float PercentageZeroPixel = float(float((width*height) - countNonZero(approxFace))/(width*height));	
		
		//If black pixel in the feature box has less than 50% black pixels, then the feature box is valid
		if (PercentageZeroPixel<=0.5)
		{
			rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
			
		}
	}
	eye_input.close();
	

	return imgSkinColorResult;
}

//Sequential Process: Process the face detecion modules sequentially.
Mat SequentialProcess(Mat imgOriginal)
{
	clock_t t1,t2,t3,t4;

	//Pixel-based face detection module.
	t1=clock();
	Mat imgSkinColorResult = Filter(imgOriginal);

	//Feature-based face detection module.
	t2=clock();
	vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
	t3=clock();
	vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
	t4=clock();
	DrawFeatureBox(imgSkinColorResult, eyes);
	DrawFeatureBox(imgSkinColorResult, faces);
	float diffFilter = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
	float diffEyeCascade = ((float)t3 - (float)t2)/CLOCKS_PER_SEC;
	float diffFaceCascade = ((float)t4 - (float)t3)/CLOCKS_PER_SEC;
	cout << "The Skin color segmentation takes " << diffFilter << "s to complete." << endl;
	cout << "The EyeCascade segmentation takes " << diffEyeCascade << "s to complete." << endl;
	cout << "The FaceCascade segmentation takes " << diffFaceCascade << "s to complete." << endl;
	return imgSkinColorResult;
}

//Process the image using different face detection module including pixel-based, feature-based, multiprocess, sequential process
void ImageProcess(Mat image,string command,bool showImage,string fileName)
{
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;

	
	int frameSize = image.rows * image.cols;
	
	Mat imgResult = image.clone();

	//Sequential-Process pixel-based face detection
	if (command == "ss")
	{
		for (int i = 0; i<1; i++){
			clock_t t1,t2;
			t1 = clock();
			imgResult = Filter(image);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
		}
		
		
	}
	//multi-process pixel-based face detection
	else if (command == "sp")
	{
		for (int i = 0; i<1; i++){
			clock_t t1,t2;
			t1 = clock();
			imgResult = SkinColorDetection(image);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
		}
	}
	//eye cascade classifier
	else if (command == "e")
	{
		clock_t t1,t2;
		t1 = clock();
		imgResult = EyeDetection(image);
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << diff << endl;

	}
	//face cascade classifier
	else if (command == "f")
	{
		clock_t t1,t2;
		t1 = clock();
		imgResult = FaceDetection(image);
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << diff << endl;

	}
	//sequential process to handle all face detection modules	
	else if (command == "c")
	{
		for (int i = 0; i < 1; i++)
		{
			clock_t t1,t2;
			t1 = clock();
			imgResult = SequentialProcess(image);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			
		}
		

	}
	//multi process to handle all face detection modules.
	else if (command == "mp")
	{
		for (int i = 0; i < 1; i++)
		{
			clock_t t1,t2;
			t1 = clock();
			imgResult = MultiProcess(image);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			
		}
	}
	//Partial skin color segmentation filter: Use skin color segmentation on the feature box only but not the entire image.
	else if (command == "pf")
	{
		imgResult = PartialSkinFilter(image);
	}else
	{
		cout << "Invalid command" << endl;
	}
	
	
	
	if (showImage)
	{
		imshow(fileName,image);
		imshow(fileName+"_Result", imgResult);
		imwrite("Result_"+fileName,imgResult,param);
		
		moveWindow(fileName,500,0);
		waitKey(0);
		destroyAllWindows();
		
	}
	
	imgResult.release();
	image.release();
	
}

int main(int argc, char** argv)
{
	//Feed the input image to the face detection system.
	Mat image = ImageInput(argc, argv);
	return 0;
}
