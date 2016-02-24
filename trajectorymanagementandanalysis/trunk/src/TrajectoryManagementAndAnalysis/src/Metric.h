#ifndef METRIC_H_
#define METRIC_H_

#include "Trajectory.h"

/**
 * Metric class.
 *
 * The Metric class is an abstract class. Every class computing similarity between two trajectories should inherit this class.
 */
template<typename Tr, typename To>
class Metric
{
public:
	/**
	 * Compute similarity between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result similarity between two trajectories
	 */
  virtual void similarity(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result) {
    result = To(0);
  }

	/**
	 * Compute distance (or normalized distance) between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[in] nbOfPoints is the limit to which the distance of trajectories is calculated. This is useful in the event of different sizes.
	 * @param[out] result distance between two trajectories
	 */
  virtual void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) = 0;

	

};

#endif /* METRIC_H_ */
