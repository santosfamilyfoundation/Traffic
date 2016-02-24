#include "cvutils.hpp"
#include "utils.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Point2f project(const Point2f& p, const Mat& homography) {
  //Mat homogeneous(3, 1, CV_32FC1);
  float x, y;
  float w = homography.at<float>(2,0)*p.x+homography.at<float>(2,1)*p.y+homography.at<float>(2,2);
  if (w != 0) {
    x = (homography.at<float>(0,0)*p.x+homography.at<float>(0,1)*p.y+homography.at<float>(0,2))/w;
    y = (homography.at<float>(1,0)*p.x+homography.at<float>(1,1)*p.y+homography.at<float>(1,2))/w;
  } else {
    x = 0;
    y = 0;
  }
  return Point2f(x, y);
}

Mat loadMat(const string& filename, const string& separator) {
  vector<vector<float> > numbers = ::loadNumbers(filename, separator);
  
  Mat mat;

  if (!numbers.empty()) {
    mat = Mat(numbers.size(),numbers[0].size(), CV_32FC1);
    for (unsigned int i=0; i<numbers.size(); i++)
      for (unsigned int j=0; j<numbers[0].size(); j++)
	mat.at<float>(i,j) = numbers[i][j];
  }

  return mat;
}

void keyPoints2Points(const vector<KeyPoint>& kpts, vector<Point2f>& pts, const bool& clearPts /* = true */) {
  if (clearPts)
    pts.clear();
  pts.reserve(kpts.size());

  for (unsigned int i=0; i<kpts.size(); i++)
    pts.push_back(kpts[i].pt);
}

// IplImage* allocateImage(const int& width, const int& height, const int& depth, const int& channels) { return ::allocateImage(cvSize(width, height), depth, channels);}

// IplImage* allocateImage(const CvSize& size, const int& depth, const int& channels) {
//   IplImage* image = cvCreateImage(size, depth, channels);

//   if (!image) {
//     cerr << "Error: Couldn't allocate image.  Out of memory?\n" << endl;
//     exit(-1);
//   }

//   return image;
// }

// int goToFrameNum(CvCapture* inputVideo, const int& currentFrameNum, const int& targetFrameNum) {
//   int frameNum = currentFrameNum;
//   if (currentFrameNum > targetFrameNum) {
//     cerr << "Current frame number " << currentFrameNum << " is after the target frame number " << targetFrameNum << "." << endl;
//   } else if (currentFrameNum < targetFrameNum) {
//     IplImage* frame = cvQueryFrame(inputVideo);
//     frameNum++;
//     while (frame && frameNum<targetFrameNum) {
//       frame = cvQueryFrame(inputVideo);
//       frameNum++;
//     }
//   }
  
//   return frameNum;
// }

const Scalar Colors::color[] = {Colors::red(),
				Colors::green(),
				Colors::blue(),
				Colors::cyan(), 
				Colors::magenta(), 
				Colors::yellow(), 
				Colors::white(), 
				Colors::black()};

// Blue, Green, Red
Scalar Colors::black(void) { return Scalar(0,0,0);}
Scalar Colors::blue(void) { return Scalar(255,0,0);}
Scalar Colors::green(void) { return Scalar(0,255,0);}
Scalar Colors::red(void) { return Scalar(0,0,255);}
Scalar Colors::white(void) { return Scalar(255,255,255);}
Scalar Colors::magenta(void) { return Scalar(255,0,255);}
Scalar Colors::yellow(void) { return Scalar(0,255,255);}
Scalar Colors::cyan(void) { return Scalar(255,255,0);}

Scalar Colors::color3(const int& num) { return Colors::color[num%3];}
Scalar Colors::color8(const int& num) { return Colors::color[num%Colors::nColors];}
