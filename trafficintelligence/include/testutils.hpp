#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "Motion.hpp"

#include "opencv2/core/core.hpp"

inline std::shared_ptr<FeatureTrajectory> createFeatureTrajectory(const unsigned int& id, const unsigned int& firstInstant, const unsigned int& lastInstant, const cv::Point2f& firstPosition, const cv::Point2f& velocity) {
  cv::Mat emptyHomography;
  std::shared_ptr<FeatureTrajectory> t = std::shared_ptr<FeatureTrajectory>(new FeatureTrajectory(firstInstant, firstPosition, emptyHomography));
  cv::Point2f p = firstPosition;
  for (unsigned int i=firstInstant+1; i<=lastInstant; ++i) {
    p = p+velocity;
    t->addPoint(i, p, emptyHomography);
  }
  t->setId(id);
  return t;
}

#endif
