#include "cvutils.hpp"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <iostream>

using namespace std;


int main(int argc, char *argv[]) {
  //cout << "Hello World" << endl;

  CvCapture *inputVideo = cvCaptureFromFile(argv[1]);

  IplImage* frame = cvQueryFrame(inputVideo);
  IplImage* bwFrame = allocateImage(frame->width, frame->height, IPL_DEPTH_8U, 1);

  int frameNum = 0;
  while (frame) {
    if (frameNum%10 == 0)
      cout << frameNum << endl;
    cvConvertImage(frame, bwFrame);
    
    for (int i=0; i<frame->height; ++i)
      for (int j=0; j<frame->width; ++j)
	int gray = cvGetReal2D(bwFrame, i, j);

    frame = cvQueryFrame(inputVideo);
    frameNum++;
  }

  return 1;
}
