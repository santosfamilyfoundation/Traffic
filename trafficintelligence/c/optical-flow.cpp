#include "cvutils.hpp"
#include "utils.hpp"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <iostream>
#include <ctime>

/* MSVC does not have lrintf */
#ifdef _MSC_VER
static inline long lrintf(float f){
/* x64 does not supported embedded assembly */
#ifdef _M_X64
    return (long)((f>0.0f) ? (f + 0.5f):(f -0.5f));
#else
    int i;
 
    _asm{
        fld f
        fistp i
    };
 
    return i;
#endif
}
#endif

using namespace std;

void videoTiming(CvCapture* inputVideo) {
  IplImage* frame = cvQueryFrame(inputVideo);
  //IplImage* bwFrame = allocateImage(frame->width, frame->height, IPL_DEPTH_8U, 1);

  int frameNum = 0;
  time_t seconds;
  time_t t0 = time(NULL);
  while (frame) {
    frameNum = ::goToFrameNum(inputVideo, frameNum, frameNum+1000);
    seconds = time(NULL)-t0;

    cout << frameNum << " " << seconds << endl;
  }
}

int main(int argc, char *argv[]) {
  //cout << "Hello World" << endl;

  CvCapture *inputVideo = 0;
  if (argc == 1)
    inputVideo = cvCreateCameraCapture(-1);
  else
    inputVideo = cvCaptureFromFile(argv[1]);

  int frameNum = 0;
  cvNamedWindow("Optical Flow", CV_WINDOW_AUTOSIZE);

  // allocate space for pyramids
  IplImage* frame = cvQueryFrame(inputVideo);
  CvSize frameSize = cvSize(frame->width, frame->height);

  IplImage* frame1_1C = ::allocateImage(frameSize, IPL_DEPTH_8U, 1);
  cvConvertImage(frame, frame1_1C);

  IplImage *frame1 = ::allocateImage(frameSize, IPL_DEPTH_8U, 3);
  cvConvertImage(frame, frame1);

  frame = cvQueryFrame(inputVideo);

  IplImage* frame2_1C = ::allocateImage(frameSize, IPL_DEPTH_8U, 1);
  cvConvertImage(frame, frame2_1C);

  IplImage *eig_image = ::allocateImage(frameSize, IPL_DEPTH_32F, 1);
  IplImage *temp_image = ::allocateImage(frameSize, IPL_DEPTH_32F, 1);

  int nFeatures = 1000;
  CvPoint2D32f frame1_features[1000];

  CvPoint2D32f frame2_features[1000];

  char optical_flow_found_feature[1000];
  float optical_flow_feature_error[1000];

  CvSize optical_flow_window = cvSize(3,3);

  CvTermCriteria optical_flow_termination_criteria = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );
  IplImage* pyramid1 = ::allocateImage(frameSize, IPL_DEPTH_8U, 1);
  IplImage* pyramid2 = ::allocateImage(frameSize, IPL_DEPTH_8U, 1);

  int pressedKey = '?';
  while (frame && !::interruptionKey(pressedKey)) {
    cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image, frame1_features, &nFeatures, 0.05 /*quality*/, 1/* min dist*/, NULL);
    cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, frame1_features, frame2_features, nFeatures, optical_flow_window, 5, optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );

    for(int i = 0; i < nFeatures; i++) {
      if (optical_flow_found_feature[i] != 0) {
	int line_thickness = 1;
	CvScalar line_color = CV_RGB(255,0,0);
      
	CvPoint p,q;
	p.x = lrintf(frame1_features[i].x);
	p.y = lrintf(frame1_features[i].y);
	q.x = lrintf(frame2_features[i].x);
	q.y = lrintf(frame2_features[i].y);
	
	double dx = frame2_features[i].x-frame1_features[i].x;
	double dy = frame2_features[i].y-frame1_features[i].y;

	double angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
	double dist = sqrt(square(dx) + square(dy));

	if (dist > 2.) { // min motion distance
	  q.x = lrintf(frame1_features[i].x+dx*3); //(p.x - 3 * hypotenuse * cos(angle));
	  q.y = lrintf(frame1_features[i].y+dy*3); //(p.y - 3 * hypotenuse * sin(angle));
      
	  /* Now we draw the main line of the arrow. */
	  cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
	  /* Now draw the tips of the arrow.  I do some scaling so that the
	   * tips look proportional to the main line of the arrow.
	   */
	  float arrowSize = 6;
	  p.x = lrintf(q.x + arrowSize * cos(angle + pi / 4));
	  p.y = lrintf(q.y + arrowSize * sin(angle + pi / 4));
	  cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
	  p.x = lrintf(q.x + arrowSize * cos(angle - pi / 4));
	  p.y = lrintf(q.y + arrowSize * sin(angle - pi / 4));
	  cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
	}
      }
    }
    cvShowImage("Optical Flow", frame1);
    pressedKey = cvWaitKey(5);
    frame = cvQueryFrame(inputVideo);
    frameNum++;
    cout << frameNum << endl;

    cvCopy(frame2_1C, frame1_1C);
    cvCopy(pyramid2, pyramid1);
    cvConvertImage(frame, frame2_1C);
    cvConvertImage(frame, frame1);
  }

  return 1;
}
