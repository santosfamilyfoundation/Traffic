#ifndef MANHATTANMETRIC_H_
#define MANHATTANMETRIC_H_

#include "Metric.h"

#include "TrajectoryExceptions.h"

/**
 * ManhattanMetric class.
 *
 * The Manhattan metric measures the similarity between two trajectories.
 */
template<typename Tr, typename To>
class ManhattanMetric: public Metric<Tr, To>
{
public:
	/**
	 * Compute similarity between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result distance between two trajectories
	 */
  void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) 
	{
		result = To(0);

		if (nbOfPoints == std::numeric_limits<unsigned int>::max())
		  checkTrajectoryLength(a->size(), b->size());

		To diff = To(0);
		for (unsigned int i = 0; i < a->size() and i < nbOfPoints; ++i)
		{
			cv::Point3_<typeof(static_cast<Tr>(a->getPoint(i)).x)> p(a->getPoint(i) - b->getPoint(i));
			diff += abs(p.x) + abs(p.y) + abs(p.z);
		}

		result = diff;
	}

private:
	/**
	 * Check the length of the trajectories.
	 *
	 * @param[in] a the size of the trajectory
	 * @param[in] b the size of the trajectory
	 */
	void checkTrajectoryLength(size_t a, size_t b) const
	{
		if (a != b || a == 0)
		{
			throw TrajectoryLengthErrorException();
		}
	}
};

#endif /* MANHATTANMETRIC_H_ */
