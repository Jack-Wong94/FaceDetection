# Instruction
This file contains instruction of how to build and use our face detection system.

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

`pf` : Partial skin color segmentation filter which use skin color segmentation on the facial feature box only but not the entire image.

To show the output-image, type `y` in the `[show-image]` option or leave it empty to hide the image.

To time the processing time, use `time` command. E.g. `time ./FaceDetection FaceDataset/ mp`

`gprof FaceDetection`: Show the profiling result.

E.g. `./FaceDetection FaceDataset/ mp y`: Use multi-process to handle all input images in the FaceDataset directory using both pixel-based and feature-based face detection module.

## Future Work
1. Implement communication protocol between HPS and FPGA fabric (AXI bridge protocol)

2. Improving the face detection algorithm by reduce processing time.

3. Implement face recognition algorithm base on the face detection system.