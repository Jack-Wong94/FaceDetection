# FaceDetection
Multi-Processor System on Chip of Face Detection System

## How to run FaceDetection on linux
`cmake .` : configure the build using the CMakeList.txt. It is used to set up profiler and the compiler that is used in the project.

`make` : build the system based on the source file.

`./DisplayImage param` : run the system. The param is the directory of the face picture.

`gprof ./DisplayImage`: Start the profiler. Have to run `./DisplayImage` first.
