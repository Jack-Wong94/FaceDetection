#include <stdio.h>
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

//Combine filter
Mat Filter(Mat imgOriginal, int* array)
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
			//constraint of HSV
			if (19 < H && H < 240)
			{
				imgHSV.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i*(j+1) + j] = 0;
			}

			//YCrCb filter
			Vec3b YCrCbColor = imgYCrCb.at<Vec3b>(Point(j,i));
			int Cr = YCrCbColor.val[1];
			int Cb = YCrCbColor.val[2];
			if (Cr < 125 || Cr > 160 || Cb < 102 || Cb > 128)
			{
				imgYCrCb.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i * (j+1) + j] = 0;
			}

			//RGB filter
			Vec3b RGBColor = imgRGB.at<Vec3b>(Point(j,i));
			int R = RGBColor.val[0];
			int G = RGBColor.val[1];
			int B = RGBColor.val[2];

			if (B > 0.836*G + 44 || B < 0.836*G - 14 || B > 0.78*G + 42 || B < 0.79*G - 67)
			{
				imgRGB.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i * (j+1) + j] = 0;
			}
			
		}
	}
	return imgResult;
}

//HSV filter
Mat HSVFilter(Mat imgOriginal, Mat imgResult, int* array)
{
	Mat imgHSV = imgOriginal.clone();
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < imgHSV.rows; i++)
	{
		for (int j = 0; j < imgHSV.cols; j++)
		{
			Vec3b HSVColor = imgHSV.at<Vec3b>(Point(j, i));
			int H = HSVColor.val[0];
			//constraint of HSV
			if (19 < H && H < 240)
			{
				imgHSV.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i*(j+1) + j] = 0;
			}
			
		}
	}
	PrintArray(array, imgHSV.rows, imgHSV.cols, "HSV.txt");
	return imgResult;
}

//YCrCb filter
Mat YCrCbFilter(Mat imgOriginal, Mat imgResult, int* array)
{
	Mat imgYCrCb = imgOriginal.clone();
	cvtColor(imgOriginal, imgYCrCb, COLOR_BGR2YCrCb);
	for (int i = 0; i < imgYCrCb.rows; i++)
	{
		for (int j = 0; j < imgYCrCb.cols; j++)
		{
			Vec3b YCrCbColor = imgYCrCb.at<Vec3b>(Point(j,i));
			int Cr = YCrCbColor.val[1];
			int Cb = YCrCbColor.val[2];
			if (Cr < 125 || Cr > 160 || Cb < 102 || Cb > 128)
			{
				imgYCrCb.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i * (j+1) + j] = 0;
			}
		}
	}
	PrintArray(array, imgYCrCb.rows, imgYCrCb.cols, "YCrCb.txt");
	return imgResult;
}

//RGB filter
Mat RGBFilter(Mat imgOriginal, Mat imgResult, int* array)
{
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

			if (B > 0.836*G + 44 || B < 0.836*G - 14 || B > 0.78*G + 42 || B < 0.79*G - 67)
			{
				imgRGB.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				imgResult.at<Vec3b>(Point(j,i)) = Vec3b(0,0,0);
				array[i * (j+1) + j] = 0;
			}
		}
	}
	PrintArray(array, imgRGB.rows, imgRGB.cols, "RGB.txt");
	return imgResult;
}
//Use skin color to locate the possible location of the face in the picture.
Mat SkinColorDetection(Mat imgOriginal, int* array)
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
				imgResult = HSVFilter(imgOriginal, imgResult, array);
				imwrite("HSV.jpg",imgResult,param);
				exit(0);
			}
		}else if (i==1)
		{
			if (fork()==0)
			{
				imgResult = YCrCbFilter(imgOriginal, imgResult, array);
				imwrite("YCrCb.jpg",imgResult,param);
				exit(0);
			}
		}else if (i==2)
		{
			if (fork()==0)
			{
				imgResult = RGBFilter(imgOriginal, imgResult, array);
				imwrite("RGB.jpg",imgResult,param);
				exit(0);
			}
		}
	}
	
	//HSV filter
	/*std::thread t1{[&imgOriginal, &imgResult, &array]{
		imgResult = HSVFilter(imgOriginal, imgResult, array);	
	}};*/
	
	
	

	//YCrCb filter
	/*std::thread t2{[&imgOriginal, &imgResult, &array]{
		imgResult = YCrCbFilter(imgOriginal, imgResult, array);
	}};*/
	
	


	//RGB filter
	/*std::thread t3{[&imgOriginal, &imgResult, &array]{
		imgResult = RGBFilter(imgOriginal, imgResult, array);
	}};

	t1.join();
	t2.join();
	t3.join();*/
	
	
	while (wait(NULL) > 0){};
	Mat imgHSV = imread("HSV.jpg",1);
	Mat imgYCrCb = imread("YCrCb.jpg",1);
	Mat imgRGB = imread("RGB.jpg",1);
	
	//Do a bitwise operation to cancel out the unnecessary pixel.
	cv::bitwise_and(imgHSV,imgYCrCb,imgResult);
	cv::bitwise_and(imgResult,imgRGB,imgResult);

	//Testing
	/*
	Mat imgResult = imgOriginal.clone();
	imgResult = HSVFilter(imgOriginal, imgResult, array);
	imgResult = YCrCbFilter(imgOriginal, imgResult, array);
	imgResult = RGBFilter(imgOriginal, imgResult, array);*/
	return imgResult;
}

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


//Use eye cascade classifier to detect the eye feature in the picture
Mat EyeDetection(Mat imgOriginal)
{
	vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
	DrawFeatureBox(imgOriginal, eyes);
	return imgOriginal;
}

//Use face cascade classifier to detect face feature in the picture
Mat FaceDetection(Mat imgOriginal)
{
	vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
	DrawFeatureBox(imgOriginal, faces);
	return imgOriginal;
	
}
void foo(){while(true){cout<<"foo"<<endl;}}
void dum(){while(true){cout<<"dum"<<endl;}}

//Multi Process
Mat MultiProcess(Mat imgOriginal, int* array)
{
	Mat imgSkinColorResult = imgOriginal.clone();
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;
	for (int i = 0;i < 3;i++)
	{
		if (i==0)
		{
			if (fork()==0)
			{
				vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
				std::ofstream output_file;
				output_file.open("Eye.txt");
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
				output_file.open("Face.txt");
				for(int j = 0; j < faces.size(); j++)
				{
					Rect r = faces[j];
					output_file << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
				}
				output_file.close();
				exit(0);
			}
		}
		if (i==2)
		{
			if (fork()==0)
			{
				imgSkinColorResult = Filter(imgOriginal, array);
				//imwrite("finalResult.jpg",imgSkinColorResult,param);
				
				exit(0);
			}
			
		}
	}
	
	while(wait(NULL)>0){}
	
	int x,y,width,height;
	std::ifstream face_input("Face.txt");
	while (face_input >> x >> y >> width >> height)
	{
		rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
	}
	face_input.close();
	std::ifstream eye_input("Eye.txt");
	while (eye_input >> x >> y >> width >> height)
	{
		rectangle(imgSkinColorResult, Point(x, y), Point(x + width, y + height), CV_RGB(0,255,0));
	}
	eye_input.close();
	//imwrite("finalResult.jpg",imgSkinColorResult,param);
	return imgSkinColorResult;
}
//Multi Process
/*Mat MultiProcess(Mat imgOriginal, int* array)
{
	Mat imgSkinColorResult;
	int pid;
	vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 95;
	
				
	for (int i = 0; i < 2; i++)
	{
		if (i==0)
		{
			if (fork()==0)
			{
				vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
				std::ofstream output_file("Eye.txt");
				for(int j = 0; j < eyes.size(); j++)
				{
					Rect r = eyes[j];
					printf("fdsadfdasfassa\n");
					output_file << "fdsfsdfsdfsdafsadasf" << "\n";
				}
				exit(0);
			}
			
		}
	}
	imgSkinColorResult = SkinColorDetection(imgOriginal, array);
	imwrite("SkinColor.jpg",imgSkinColorResult,param);
	while (wait(NULL) > 0)
	{}
	return imgSkinColorResult;
				
}*/
//Combine haar cascade classifier with skin color segmentation algorithm
Mat Combine(Mat imgOriginal, int* array)
{
	Mat imgSkinColorResult = Filter(imgOriginal, array);
	vector<Rect> eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
	vector<Rect> faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
	DrawFeatureBox(imgSkinColorResult, eyes);
	DrawFeatureBox(imgSkinColorResult, faces);
	return imgSkinColorResult;

	/*std::thread t1(foo);
	t1.detach();
	std::thread t2(dum);
	t2.detach();
	while(true){}

	Mat imgSkinColorResult;
	vector<Rect> eyes, faces;
	bool SkinFlag, FaceFlag, EyeFlag = false;
	

	/*std::thread t_SkinColor{[&imgSkinColorResult, &imgOriginal, &array, &SkinFlag]{
		clock_t t1,t2;
		t1 = clock();
		imgSkinColorResult = SkinColorDetection(imgOriginal, array);
		
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Skin Color: ";
		cout << diff << endl;
		SkinFlag = true;
	}};
	
	//t_SkinColor.join();
	std::thread t_EyeCascade{[&eyes, &imgOriginal, &EyeFlag]{
		clock_t t1,t2;
		t1 = clock();
		eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
		
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Eye: ";
		cout << diff << endl;
		EyeFlag = true;
	}};
	//t_EyeCascade.join();

	std::thread t_FaceCascade{[&faces, &imgOriginal, &FaceFlag]{
		clock_t t1,t2;
		t1 = clock();
		faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Face: ";
		cout << diff << endl;
		FaceFlag = true;
	}};
	while (true){}
	t_SkinColor.join();
	t_EyeCascade.join();
	t_FaceCascade.join();

	/*while (!SkinFlag)
	{
		
	}
	while (!EyeFlag){}
	while (!FaceFlag){}
	
	if (t_SkinColor.joinable())
	{
		t_SkinColor.join();
	}
	
	if (t_EyeCascade.joinable())
	{
		t_EyeCascade.join();
	}
	if (t_FaceCascade.joinable())
	{
		t_FaceCascade.join();
	}
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	//t_FaceCascade.join();*/
	
	/*Mat imgSkinColorResult;
	vector<Rect> eyes, faces;
	
	std::thread t_SkinColor{[&imgSkinColorResult, &imgOriginal, &array]{
		clock_t t1,t2;
		t1 = clock();
		imgSkinColorResult = SkinColorDetection(imgOriginal, array);
		
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Skin Color: ";
		cout << diff << endl;
	}};
	
	std::thread t_EyeCascade{[&eyes, &imgOriginal]{
		clock_t t1,t2;
		t1 = clock();
		eyes = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_eye.xml");
		
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Eye: ";
		cout << diff << endl;
		
	}};

	std::thread t_FaceCascade{[&faces, &imgOriginal]{
		clock_t t1,t2;
		t1 = clock();
		faces = LocateFeatureBox(imgOriginal, "CascadeClassifier/haarcascade_frontalface_alt.xml");
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << "Face: ";
		cout << diff << endl;
	}};
	t_SkinColor.join();
	t_EyeCascade.join();
	t_FaceCascade.join();
	
	DrawFeatureBox(imgSkinColorResult, eyes);
	DrawFeatureBox(imgSkinColorResult, faces);
	return imgSkinColorResult;*/
}

int main(int argc, char** argv)
{
	//Choose the algorithm
	string command = "mp";
	Mat image;

	if (argc != 2 )
	{
		//printf("usage:DisplayImage.out <Image_Path>\n");
		image = imread(argv[1],1);
		command = argv[2];
		if ( !image.data )
		{
			printf("No image data\n");
			return -1;
		}
	}else
	{
		image = imread(argv[1],1);
		command = argv[2];
		if ( !image.data )
		{
			printf("No image data\n");
			return -1;
		}
		//cout << "Enter mode: ss(Skin Color segmentation), sp(skin color parallelization), e(eye cascade), f(face cascade), c(combine), tf(alternate skin filter), mp(multi-process): ";
		//cin >> command; 
	}

	//initialize binary array
	int* array;
	int frameSize = image.rows * image.cols;
	array = (int *)malloc(sizeof(int)*frameSize);
	for (int i = 0; i < frameSize; i++)
	{
		array[i] = 1;
	}
	
	/*namedWindow("Display Image",WINDOW_AUTOSIZE);
	imshow("Display Image", image);
	waitKey(0);*/
	Mat imgResult;
	if (command == "ss")
	{
		for (int i = 0; i<1; i++){
			clock_t t1,t2;
			t1 = clock();
			imgResult = Filter(image, array);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
		}
		
		
	}else if (command == "sp")
	{
		for (int i = 0; i<1; i++){
			clock_t t1,t2;
			t1 = clock();
			imgResult = SkinColorDetection(image, array);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
		}
	}else if (command == "e")
	{
		clock_t t1,t2;
		t1 = clock();
		imgResult = EyeDetection(image);
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << diff << endl;

	}else if (command == "f")
	{
		clock_t t1,t2;
		t1 = clock();
		imgResult = FaceDetection(image);
		t2 = clock();
		float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
		cout << diff << endl;

	}else if (command == "c")
	{
		for (int i = 0; i < 1; i++)
		{
			clock_t t1,t2;
			t1 = clock();
			imgResult = Combine(image, array);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
			cout << " " << endl;
		}
		

	}else if (command == "tf")
	{
		for (int i = 0; i < 1; i++)
		{
			clock_t t1,t2;
			t1 = clock();
			imgResult = Filter(image, array);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
			cout << " " << endl;
		}
	}else if (command == "mp")
	{
		for (int i = 0; i < 1; i++)
		{
			clock_t t1,t2;
			t1 = clock();
			imgResult = MultiProcess(image, array);
			t2 = clock();
			float diff = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			cout << diff << endl;
			cout << " " << endl;
		}
	}
	//PrintArray(array,imgResult.rows, imgResult.cols, "Result.txt");
	/*Mat imgTransform;
	cv::transform(imgResult, imgTransform, cv::Matx13f(1,1,1));
	std::ofstream out;
	out.open("Result.txt");
	out << imgTransform << endl;
	out.close();*/

	/*for (int i = 0; i < imgResult.rows; i++)
	{
		for (int j = 0; j < imgResult.cols; j++)
		{
			Vec3b BGRColor = imgResult.at<Vec3b>(Point(j, i));
			int B = BGRColor.val[0];
			int G = BGRColor.val[1];
			int R = BGRColor.val[2];
			
			//constraint of HSV
			if (B==0 && G==0 && R==0)
			{
				
				array[i*(j+1) + j] = 0;
			}
			
		}
	}
	PrintArray(array,imgResult.rows,imgResult.cols);*/

	/*Mat imgGray(imgResult.size(),CV_8U);
	cvtColor(imgResult, imgGray, CV_BGR2GRAY);
	Mat imgBinary(imgGray.size(),imgGray.type());
	threshold(imgGray,imgBinary,100,255,cv::THRESH_BINARY);
	std::ofstream out;
	out.open("Result.txt");
	out << imgBinary << endl;
	out.close();*/

	/*std::ofstream out;
	out.open("Result.txt");
	Mat imgGray;
	cvtColor(imgResult, imgGray,CV_BGR2GRAY);
	out << imgGray << endl;
	out.close();*/

	/*std::ofstream out;
	out.open("Result.txt");
	out << imgResult << endl;
	out.close();*/

	if (argc == 4)
	{
		imshow("Original",image);
		imshow("Result", imgResult);
		waitKey(0);
	}
	
	return 0;
}
