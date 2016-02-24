#ifndef CVUTILS_HPP
#define CVUTILS_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

class CvCapture;
//template<typename T> class Point_<T>;

/// constant that indicates if the image should be flipped
//static const int flipImage = CV_CVTIMG_FLIP;

/** Projects a point with the homography matrix
 use perspectiveTransform for arrays of points. */
cv::Point2f project(const cv::Point2f& p, const cv::Mat& homography);

/** Loads a cv mat from a text file where the numbers are saved line by line separated by separator */
cv::Mat loadMat(const std::string& filename, const std::string& separator);

//template<typename T> 
//float scalarProduct(const cv::Point_<T>& v1, const cv::Point_<T>& v2) { return v1.x*v2.x+v1.y*v2.y;}

void keyPoints2Points(const std::vector<cv::KeyPoint>& kpts, std::vector<cv::Point2f>& pts, const bool& clearPts = true);

/** Allocates a new IplImage. */
// IplImage* allocateImage(const int& width, const int& height, const int& depth, const int& channels);

// IplImage* allocateImage(const CvSize& size, const int& depth, const int& channels);

/** Goes to the target frame number, by querying frame, 
    supposing the video input is currently at current frame number.
    Returns the frame number that was reached.*/
// int goToFrameNum(CvCapture* inputVideo, const int& currentFrameNum, const int& targetFrameNum);

/// Pre-defined colors
class Colors {
public:
  static const int nColors = 8;
  static const cv::Scalar color[];

  static cv::Scalar black(void);
  static cv::Scalar red(void);
  static cv::Scalar green(void);
  static cv::Scalar blue(void);
  static cv::Scalar white(void);
  static cv::Scalar magenta(void);
  static cv::Scalar cyan(void);
  static cv::Scalar yellow(void);

  /** Maps integers to primary colors. */
  static cv::Scalar color3(const int& num);
  static cv::Scalar color8(const int& num);
};

#endif
