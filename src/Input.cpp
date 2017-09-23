#include <stdio.h>
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
#include "FaceDetection.h"

/**
 ** Name: Jack Wong
 ** Module: Input module
 ** Feed the input image to the face detection system with 2 different modules:
 ** Pixel-based and feature-based face detection module.
**/

using namespace cv;
using namespace std;

//Input module.
Mat ImageInput(int argc, char** argv)
{
	//Choose if showing picture.
	bool showImage;
	if (argc > 3)
	{
		showImage = true;
	}else
	{
		showImage = false;
	}

	//traverse the directory to read the image file.
	Mat image;
	DIR *dir;
	struct dirent *ent;

	//Check the directory exists
	if ((dir=opendir(argv[1]))!=NULL)
	{
		//loop through the directory
		while((ent = readdir(dir)) != NULL)
		{
			//concatenate the path to the input image file
			string fileName = ent->d_name;
			string dirName = argv[1];
			string delimiter = "/";
			string filePath = dirName+delimiter+fileName;
			cout << filePath << endl;
			image = imread(filePath,1);

			//Check if it is an image file.
			if (image.data)
			{
				ImageProcess(image,argv[2],showImage,filePath);
				
			}else
			{
				cout << "No image data" << endl;			
			}
			
		}
		closedir(dir);
	}else
	{
		image = imread(argv[1],1);
		//cvtColor(image,image,COLOR_BGR2RGB);
		ImageProcess(image,argv[2],showImage,argv[1]);
	}
	
	return image;

}
