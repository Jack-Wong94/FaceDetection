# Multi-Processor System on Chip of real time Face Detection System.
This project is to implement the real-time face detection system on embedded system. It is designed to run on the Linux operating system. It is implemented based on the OpenCV library. The face detection system is consisted of 3 modules: input module, pixel-based face detection module, and feature-based face detection module. Input module feed input images to the face detection system for further processing. Pixel-based face detection module is implemented basedon the skin color segmentation algorithm while feature-based face detection module is implemented based on the Haar's cascade classifier.

## Content:
1. Poster of Face Detection System

2. Reports on software component and hardware component of the system.

3. Source code for software and hardware modules.

4. Configuration file to build the face detection system.

5. Cascade classifier data directory.

6. Directories that contains different Face Dataset for testing purpose.

7. Seminar slide.

8. Instruction of using the face detection system (see below)

9. Workload distribution (see below)

10. Instruction file to guide user how to use the system.

## Instruction
Please read the instruction file of how to build and use our face detection system.

## Version Control
We are using github for version control. If required, please sent an email to lwon229@aucklanduni.ac.nz to gain access to the repository.

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

