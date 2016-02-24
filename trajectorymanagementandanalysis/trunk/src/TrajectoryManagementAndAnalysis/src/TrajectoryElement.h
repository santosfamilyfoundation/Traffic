#ifndef TRAJECTORYELEMENT_H_
#define TRAJECTORYELEMENT_H_

#include <istream>
#include <ostream>

#include "PointOperations.h"

/**
 * TrajectoryElement class.
 *
 * The Trajectory class is composed of components called TrajectoryElement.
 */
template<typename T>
class TrajectoryElement {
 public:
  /**
   * Constructor.
   */
  TrajectoryElement() {}
  
  /**
   * Constructor.
   *
   * @param[in] frameNumber number of a frame
   * @param[in] position position
   */
  TrajectoryElement(unsigned int frameNumber, const T& point) {
    setFrameNumber(frameNumber);
    setPoint(point);
  }
  
  /**
   * Constructor.
   *
   * @param[in] trajectoryElement element of the trajectory
   */
  TrajectoryElement(const TrajectoryElement& trajectoryElement) {
    setFrameNumber(trajectoryElement.getFrameNumber());
    setPoint(trajectoryElement.getPoint());
  }
  
  /**
   * Get number of a frame.
   *
   * @return number of a frame
   */
  unsigned int getFrameNumber() const {
    return frameNumber;
  }
  
  /**
   * Set number of a frame.
   *
   * @param[in] frameNumber number of a frame
   */
  void setFrameNumber(unsigned int iframeNumber) {
    frameNumber = iframeNumber;
  }
  
  /**
   * Get position.
   *
   * @return position
   */
  const T &getPoint() const {
    return point;
  }
  
  /**
   * Set position.
   *
   * @param[in] position position
   */
  void setPoint(const T& ipoint) {
    point = ipoint;
  }
  
  /**
   * Shift position.
   *
   * @param[in] t vector by which the position of the element should be shifted.
   */
  void shift(const T& t) {
    point += t;
  }

 private:
  /**
   * Frame number.
   */
  unsigned int frameNumber;
  
  /**
   * Position of the element.
   */
  T point;
};

template<typename T>
static inline bool operator==(const TrajectoryElement<T>& a, const TrajectoryElement<T>& b) {
  return (a.getFrameNumber() == b.getFrameNumber()) && (a.getPoint() == b.getPoint());
}

template<typename Ti, typename Tr>
  std::basic_istream<Ti>& operator>>(std::basic_istream<Ti>& in, TrajectoryElement<Tr>& trajectoryElement) {
  unsigned int frameNumber;
  Tr point;
  if (in >> frameNumber >> point) {
    trajectoryElement.setFrameNumber(frameNumber);
    trajectoryElement.setPoint(point);
  }
  return in;
}

template<typename Tr>
std::ostream& operator<<(std::ostream& out, const TrajectoryElement<Tr>& trajectoryElement) {
  out << trajectoryElement.getFrameNumber() << " " << trajectoryElement.getPoint();
  return out;
}

#endif /* TRAJECTORYELEMENT_H_ */
