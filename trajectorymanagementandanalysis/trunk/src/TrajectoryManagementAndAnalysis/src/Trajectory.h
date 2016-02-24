#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "TrajectoryElement.h"
#include "TrajectoryExceptions.h"

#include "opencv2/core/core.hpp"

#include <ostream>
#include <vector>
#include <cassert>
#include <algorithm>

/**
 * Trajectory class.
 *
 * The Trajectory class is a container to keep information about a trajectory.
 * defined as a sequence of not necessarily consecutive trajectory elements (instand + point coordinates)
 * 
 * \todo check (and remove) stuff about ascending frame numbers
 */
template<typename Type>
class Trajectory
{
public:
  /// Constructor.
 Trajectory() : id(0), checkAscFrameNumber(false) {}

  /**
   * Constructor.
   *
   * @param trajectory trajectory
   */
  Trajectory(const Trajectory &itrajectory) {
    setId(itrajectory.getId());
    for (unsigned int i = 0; i < itrajectory.size(); ++i)
      trajectory.push_back(itrajectory.getTrajectoryElement(i)); // justified because we directly copy the first and last instants
    
    setCheckAscFrameNumber(itrajectory.getCheckAscFrameNumber());
  }

  /**
   * Constructor.
   *
   * @param trajectory trajectory
   */
 Trajectory(const int& id, const std::vector<TrajectoryElement<Type> >& trajectoryElements):
  id(id), checkAscFrameNumber(false) {
    for (typename std::vector<TrajectoryElement<Type> >::const_iterator iter = trajectoryElements.begin();
	 iter != trajectoryElements.end(); iter++)
      add(*iter);
  }

  /**
   * Get id of the trajectory.
   *
   * @return id
   */
  unsigned int getId(void) const { return id;}

  /**
   * Set id of the trajectory.
   *
   * @param[in] id id
   */
  void setId(const unsigned int& iid) { id = iid;}

  /**
   * Get element of the trajectory.
   *
   * @param[in] position index of the element of the trajectory
   * @return element of the trajectory#include "OpenCVPointTypeSupport.h"
   */
  const TrajectoryElement<Type> getTrajectoryElement(unsigned int position) const
  {
    return trajectory[position];
  }
  
  /**
   * Add an element to the trajectory.
   *
   * @param[in] frameNumber number of the frame of the new element of the trajectory
   * @param[in] point position of the new element of the trajectory
   */
  void add(unsigned int frameNumber, const Type &point) throw (TrajectoryFrameNumberErrorException)
  {
    TrajectoryElement<Type> trajectoryElement = TrajectoryElement<Type> (frameNumber, point);
    
    if (getCheckAscFrameNumber())
      {
	ascFrameNumberAddCheck(frameNumber);
      }
    trajectory.push_back(trajectoryElement);
  }
  
  /**
   * Add an element to the trajectory.
   *
   * @param[in] point position of the new element of the trajectory
   */
  void add(const Type &point)
  {
    unsigned int frameNumber = 1;
    add(frameNumber, point);
  }
  
  void add(const TrajectoryElement<Type> &trajectoryElement) throw (TrajectoryFrameNumberErrorException)
  {
    int frameNumber = trajectoryElement.getFrameNumber();
    if (getCheckAscFrameNumber())
      {
	ascFrameNumberAddCheck(frameNumber);
      }
    trajectory.push_back(trajectoryElement);
  }

  /**
   * Get number of the frame of the element of the trajectory.
   *
   * @param[in] position index of the elesize_t ment of the trajectory
   * @return number of the frame of the element of the trajectory.
   */
  unsigned int getFrameNumber(unsigned int position) const
  {
    return trajectory[position].getFrameNumber();
  }

  /**
   * Get position of the element of the trajectory.
   *
   * @param[in] position index of the element of the trajectory
   * @return position of the element of the trajectory
   */
  const Type &getPoint(unsigned int position) const
  {
    return trajectory[position].getPoint();
  }

  /** Get position of the element of the trajectory. */
  const Type& getPointAtInstant(const unsigned int& t) const throw (TrajectoryOutOfRangeErrorException) {
    typename std::vector<TrajectoryElement<Type> >::const_iterator iter = trajectory.begin();
    while (iter != trajectory.end() && iter->getFrameNumber() != t)      
      iter++;
    if (iter != trajectory.end())
      return iter->getPoint();
    else {
      throw TrajectoryOutOfRangeErrorException(t);
    }
  }

  bool getCheckAscFrameNumber() const
  {
    return checkAscFrameNumber;
  }

  void setCheckAscFrameNumber(bool icheckAscFrameNumber) throw (TrajectoryFrameNumberErrorException)
  {
    if (icheckAscFrameNumber)
      {
	ascFrameNumberCheck();
      }

    checkAscFrameNumber = icheckAscFrameNumber;
  }

  /**
   * Get number of elements in the trajectory.
   *
   * @return number of elements in the trajectory
   */
  unsigned int size() const
  {
    return trajectory.size();
  }

  bool empty() const
  {
    return trajectory.empty();
  }

  void insert(unsigned int position, unsigned int frameNumber, const Type &point) throw (TrajectoryOutOfRangeErrorException, TrajectoryFrameNumberErrorException)
  {
    trajectoryRangeLeEqCheck(position);

    if (getCheckAscFrameNumber())
      {
	ascFrameNumberInsertCheck(position, frameNumber);
      }

    trajectory.insert(trajectory.begin() + position, TrajectoryElement<Type> (frameNumber, point));
  }

  /**
   * Erase an element from the trajectory.
   *TrajectoryFrameNumberErrorException
   * @param[in] position index of the trajectory to be removed
   */
  void erase(unsigned int position) throw (TrajectoryOutOfRangeErrorException)
  {
    trajectoryRangeLeCheck(position);
    trajectory.erase(trajectory.begin() + position);
  }

  /**
   * Erase the last element from the trajectory.
   */
  void pop_back() throw (TrajectoryLengthErrorException)
  {
    trajectoryNotEmptyCheck();
    trajectory.pop_back();
  }

  /**
   * Clear the trajectory.
   */
  void clear()
  {
    trajectory.clear();
  }

  /**
   * Get position of the element of the trajectory.
   *
   * @param[in] i index of the element of the trajectory
   * @return position of the element of the trajectory
   */
  Type operator [](unsigned int i) const
  {
    return trajectory[i].getPoint();
  }

  /**
   * Get position of the element of the trajectory.
   *
   * @param[in] i index of the element of the trajectory
   * @return position of the element of the trajectory
   */
  const Type& at(unsigned int i) const throw (TrajectoryOutOfRangeErrorException)
  {
    trajectoryRangeLeCheck(i);
    return trajectory[i].getPoint();
  }

  /**
   * Shift each element of the trajectory.
   *
   * @param[in] t vector by which the position of each element of the trajectory should be shifted.
   */
  void shift(const Type& t)
  {
    for (unsigned int i = 0; i < trajectory.size(); ++i)
      {
	trajectory[i].shift(t);
      }
  }

   /** Computes the first and last instants */
  void computeInstants(unsigned int& firstInstant, unsigned int& lastInstant) {
    if (trajectory.empty()) {
      firstInstant = 0;
      lastInstant = 0;
    } else {
      typename std::vector<TrajectoryElement<Type> >::iterator iter = trajectory.begin();
      firstInstant = iter->getFrameNumber();
      lastInstant = iter->getFrameNumber();
      iter++;
      while (iter != trajectory.end()) {
	unsigned int frameNumber = iter->getFrameNumber();
	if (frameNumber < firstInstant)
	  firstInstant = frameNumber;
	else if (frameNumber > lastInstant)
	  lastInstant = frameNumber;
	iter++;
      }
    }
  }

  /** Smoothes the trajectory positions (parameter is the half-window used for moving average) */
  void movingAverage(const unsigned int& nFramesSmoothing) {
    if (!trajectory.empty()) {
      // todo check the frames are in increasing order
      std::vector<TrajectoryElement<Type> > smoothedTrajectory;
      unsigned int nPositions = trajectory.size();
      for(unsigned int i=0; i<nPositions; ++i) {
      	Type p(0,0);
      	unsigned int delta = std::min(nFramesSmoothing, std::min(i, nPositions-1-i));
      	for (unsigned int j=i-delta; j<=i+delta; ++j)
      	  p = p+getPoint(j);
      	smoothedTrajectory.push_back(TrajectoryElement<Type>(trajectory[i].getFrameNumber(), p*(1./(1.+2.*static_cast<float>(delta)))));
      }
      trajectory = smoothedTrajectory;
    }
  }

protected:
  /**
   * Check the length of the trajectories.
   *
   * @param[in] a the size of the trajectory
   * @param[in] b the size of the trajectory
   */
  void trajectoryNotEmptyCheck() const throw (TrajectoryLengthErrorException)
  {
    if (trajectory.size() == 0)
      {
	throw TrajectoryLengthErrorException();
      }
  }

  /**
   * Check the range of the trajectory.
   *
   * @param[in] n index of the element of the trajectory
   */
  void trajectoryRangeLeEqCheck(unsigned int n) const throw (TrajectoryOutOfRangeErrorException)
  {
    if (n > trajectory.size())
      {
	throw TrajectoryOutOfRangeErrorException();
      }
  }

  /**
   * Check the range of the trajectory.
   *
   * @param[in] n index of the element of the trajectory
   */
  void trajectoryRangeLeCheck(unsigned int n) const throw (TrajectoryOutOfRangeErrorException)
  {
    if (n >= trajectory.size())
      {
	throw TrajectoryOutOfRangeErrorException();
      }
  }

  void ascFrameNumberCheck(unsigned int prevFrameNumber, unsigned int currFrameNumber) const throw (TrajectoryFrameNumberErrorException)
  {
    if (prevFrameNumber >= currFrameNumber)
      {
	throw TrajectoryFrameNumberErrorException();
      }
  }

  void ascFrameNumberCheck() const throw (TrajectoryFrameNumberErrorException)
  {
    for (unsigned int i = 1; i < trajectory.size(); ++i)
      {
	ascFrameNumberCheck(trajectory[i - 1].getFrameNumber(), trajectory[i].getFrameNumber());
      }
  }

  void ascFrameNumberAddCheck(unsigned int frameNumber) const throw (TrajectoryFrameNumberErrorException)
  {
    if (!trajectory.empty())
      {
	ascFrameNumberCheck(trajectory.back().getFrameNumber(), frameNumber);
      }
  }

  void ascFrameNumberInsertCheck(unsigned int position, unsigned int frameNumber) const throw (TrajectoryFrameNumberErrorException)
  {
    if (position > 0)
      {
	ascFrameNumberCheck(trajectory[position - 1].getFrameNumber(), frameNumber);
      }

    if (position < trajectory.size())
      {
	ascFrameNumberCheck(frameNumber, trajectory[position].getFrameNumber());
      }
  }

 private:

  /// Id of the trajectory.
  unsigned int id;

  /**
   * Trajectory.
   */
  std::vector<TrajectoryElement<Type> > trajectory;

  bool checkAscFrameNumber;
};

typedef Trajectory<cv::Point_<int> > TrajectoryPoint2i;
typedef TrajectoryPoint2i TrajectoryPoint;
typedef Trajectory<cv::Point_<float> > TrajectoryPoint2f;
typedef Trajectory<cv::Point_<double> > TrajectoryPoint2d;
typedef Trajectory<cv::Point3_<int> > TrajectoryPoint3i;
typedef Trajectory<cv::Point3_<float> > TrajectoryPoint3f;
typedef Trajectory<cv::Point3_<double> > TrajectoryPoint3d;

/**
 * Compare two trajectories.
 *
 * @param[in] t trajectory trajectory
 * @return information whether trajectories are equal or notPoint_<_Tp>
 */
template<typename T>
static inline bool operator ==(const Trajectory<T>& a, const Trajectory<T>& b)
{
  if (a.getId() != b.getId() || a.getCheckAscFrameNumber() != b.getCheckAscFrameNumber() || a.size() != b.size())
    {
      return false;
    }

  for (unsigned int i = 0; i < a.size(); ++i)
    {
      if (a.getTrajectoryElement(i) != b.getTrajectoryElement(i))
	{
	  return false;
	}
    }

  return true;
}

template<typename T>
static inline Trajectory<T>& operator +=(Trajectory<T>& a, const T& b)
{
  a.add(b);
  return a;
}

template<typename T>
static inline Trajectory<T>& operator +=(Trajectory<T>& a, const TrajectoryElement<T>& b)
{
  a.add(b);
  return a;
}

template<typename Ti, typename Tr>
  std::basic_istream<Ti>& operator>>(std::basic_istream<Ti>& in, Trajectory<Tr>& trajectory)
{
  unsigned int id;
  if (in >> id)
    {
      trajectory.setId(id);
      TrajectoryElement<Tr> trajectoryElement;
      while (in >> trajectoryElement)
	{
	  trajectory.add(trajectoryElement);
	}
    }
  return in;
}

template<typename Tr>
std::ostream& operator<<(std::ostream& out, const Trajectory<Tr>& trajectory)
{
  out << trajectory.getId();
  for (unsigned int i = 0; i < trajectory.size(); ++i)
    {
      out << " ";
      out << trajectory.getTrajectoryElement(i);
    }
  return out;
}

/**
 * Compute the minimum and the maximum position of the trajectory.
 *
 * @param[out] _min minimum position of the trajectory
 * @param[out] _max maximum position of the trajectory
 */
template<typename T>
static inline T min(const Trajectory<T>& trajectory) throw (TrajectoryLengthErrorException)
{
  if (trajectory.empty())
    {
      throw TrajectoryLengthErrorException();
    }

  T point = trajectory.getPoint(0);

  for (unsigned int i = 1; i < trajectory.size(); ++i)
    {
      point = min(point, trajectory.getPoint(i));
    }

  return point;
}

template<typename T>
static inline T max(const Trajectory<T>& trajectory) throw (TrajectoryLengthErrorException)
{
  if (trajectory.empty())
    {
      throw TrajectoryLengthErrorException();
    }

  T point = trajectory.getPoint(0);

  for (unsigned int i = 1; i < trajectory.size(); ++i)
    {
      point = max(point, trajectory.getPoint(i));
    }

  return point;
}

#endif /* TRAJECTORY_H_ */
