# Multi-Processor System on Chip of real time Face Detection System.
This project is to implement the real-time face detection system on embedded system. It is designed to run on the Linux operating system. It is implemented based on the OpenCV library. The face detection system is consisted of 3 modules: input module, pixel-based face detection module, and feature-based face detection module. Input module feed input images to the face detection system for further processing. Pixel-based face detection module is implemented basedon the skin color segmentation algorithm while feature-based face detection module is implemented based on the Haar's cascade classifier.

## Content:
1. Poster of Face Detection System

2. Reports on software component and hardware component of the system.

3. Source code for software and hardware modules.

4. Configuration file to build the face detection system.

5. Directories that contains different Face Dataset for testing purpose.

6. Seminar slide.

7. Instruction of using the face detection system (see below)

8. Workload distribution (see below)

## Installation of OpenCV
OpenCV (Open source computer vision library) is designed for computer vision in real-time application. It contains many algorithms that optimize face detection algorithm.

For the installation guide, please check http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html.

This project is tested on Altera DE1-SoC FPGA development board. Install Linux Ubuntu microSD card image from https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=167&No=836&PartNo=4. Boot the FPGA using the microSD card. Install the required packages of OpenCV by following the installation tutorial. 

## Build the face detecion system.
`cmake .` : configure the build using the CMakeList.txt. It is used to set up profiler and the compiler that is used in the project.

`make` : build the system based on the source file.

## How to run the face detection system on linux
`./FaceDetection [path-to-image-dir or image_destination] [option] [show-image]` : The command to run the face detection system.

There are 7 options:

`ss` : Sequential-Process pixel-based face detection

`sp` : multi-process pixel-based face detection

`e` : eye cascade classifier

`f` : face cascade classifier

`c` : sequential process to handle all face detection modules

`mp` : multi process to handle all face detection modules.

`pf` : Partial skin color segmentation filter which Use skin color segmentation on the feature box only but not the entire image.

To show the output-image, type `y` in the `[show-image]` option.

To time the processing time, use `time` command.

`gprof FaceDetection`: Show the profiling result.

E.g. `./FaceDetection FaceDataset/ mp y`: Use multi process to process all input images in the FaceDataset directory using pixel-based and feature-based face detection module.

## Version Control
We are using github for version control. If required, please sent an email to lwon229@aucklanduni.ac.nz to get access to the repository.

## Work distribution
We applies hardware-software codesign method to implement the face detection system. We have seperate the system into hardware and software components.

Jack Wong:

1. Research on the pixel-based and feature-based face detection algorithm.

2. Implementation of the skin color segmentation algorithm.

3. Implementation of cascade classifier algorithm.

4. Combine the pixel-based and feature-based face detection algorithm to improve accuracy of the system.

5. Install OpenCV packages and integrate the software component of our Face Detection System into embedded platform.

6. Research on the communication protocol (AXI bridge) between HPS and FPGA. (not implemented)

7. Implement software component of the face detection system on windows machine using camera feed instead of face dataset. (initial stage, not included in the project)

8. Use multi-processing technique to concurrently handle face detection modules. Also investigate the performance difference between sequential and concurrent programming.

9. Test the software component of the face detection system including accuracy and processing time in each modules.

10. Compare the result from software component to the hardware component to test if hardware component accelerate parts of the face detection algorithm.

Lin Yang:

