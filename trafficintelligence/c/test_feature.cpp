#define CATCH_CONFIG_MAIN

#include "Motion.hpp"
#include "testutils.hpp"

#include "opencv2/core/core.hpp"

#include "catch.hpp"

using namespace std;
using namespace cv;

TEST_CASE("trajectory/smoothing", "test trajectory smoothing (from trajectory management library)") {
  TrajectoryPoint2f t1;
  for(int i=0; i<20;++i)
    t1.add(i, cv::Point2f(1+i, 1+0.5*i));

  TrajectoryPoint2f t2(t1);
  t2.movingAverage(3);
  for(int i=0; i<20;++i) 
    REQUIRE(t1.getPoint(i) == t2.getPoint(i));
  t1.clear();
  cv::Point2f p0(1,1);
  cv::Point2f p1(2,2);
  cv::Point2f p2(2.4,3);
  cv::Point2f p3(3.1,3.4);
  cv::Point2f p4(3.4,4);
  cv::Point2f p5(3.6,4.5);
  
  t1.add(0, p0);
  t1.add(1, p1);
  t1.add(2, p2);
  t1.add(3, p3);
  t1.add(4, p4);
  t1.add(5, p5);
  t1.movingAverage(2);
  REQUIRE(t1.getPoint(0) == p0);
  REQUIRE(t1.getPoint(1) == (p0+p1+p2)*(1./3.));
  REQUIRE(t1.getPoint(2) == (p0+p1+p2+p3+p4)*(1./5.));
  REQUIRE(t1.getPoint(3) == (p1+p2+p3+p4+p5)*(1./5.));
  REQUIRE(t1.getPoint(4) == (p3+p4+p5)*(1./3.));
  REQUIRE(t1.getPoint(5) == p5);
}

TEST_CASE("features/similarity", "test feature similarity measure") {
  FeatureTrajectoryPtr ft1 = createFeatureTrajectory(1, 10, 20, Point2f(1,1), Point2f(0, 1));
  FeatureTrajectoryPtr ft2 = createFeatureTrajectory(2, 10, 20, Point2f(2,1), Point2f(0, 1));

  REQUIRE_FALSE(ft1->minMaxSimilarity(*ft2, 10, 20, 0.5, 0.1));
  REQUIRE(ft1->minMaxSimilarity(*ft2, 10, 20, 1, 0.1));

  ft2 = createFeatureTrajectory(2, 10, 19, Point2f(1,1), Point2f(0, 1));
  Mat homography;
  ft2->addPoint(20, Point2f(1,11.5), homography);
  
  REQUIRE_FALSE(ft1->minMaxSimilarity(*ft2, 10, 20, 0, 0.4));
  REQUIRE(ft1->minMaxSimilarity(*ft2, 10, 20, 0, 0.5));
}
