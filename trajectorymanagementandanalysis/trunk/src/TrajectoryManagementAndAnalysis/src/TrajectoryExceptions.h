#ifndef TRAJECTORY_EXCEPTIONS_H
#define TRAJECTORY_EXCEPTIONS_H

#include <exception>
#include <sstream>

/**
 * TrajectoryFrameNumberErrorException class.
 */
class TrajectoryFrameNumberErrorException: public std::exception {
	/**
	 * Return the name of the exception.
	 *
	 * @return The name of the exception
	 */
	virtual const char* what() const throw ()
	{
		return "TrajectoryFrameNumberErrorException";
	}
};

/**
 * TrajectoryLengthErrorException class.
 *
 * The TrajectoryLengthError exception should be invoked when a length of a trajectory is incorrect.
 */
class TrajectoryLengthErrorException: public std::exception {
	/**
	 * Return the name of the exception.
	 *
	 * @return The name of the exception
	 */
	virtual const char* what() const throw ()
	{
		return "TrajectoryLengthErrorException";
	}
};

/**
 * TrajectoryOutOfRangeErrorException class.
 */
class TrajectoryOutOfRangeErrorException: public std::exception {
 public:
 TrajectoryOutOfRangeErrorException(const unsigned int& _i = -1)
   : i(_i) {}
  
 protected:
  int i;
  
  /**
   * Return the name of the exception.
   *
   * @return The name of the exception
   */
  virtual const char* what() const throw ()
  {
    std::stringstream ss;
    ss << "TrajectoryOutOfRangeErrorException: " << i;
    return  ss.str().c_str();
  }
};

#endif
