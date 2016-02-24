#ifndef SQUAREDEUCLIDIANMETRIC_H_
#define SQUAREDEUCLIDIANMETRIC_H_
#include "Metric.h"
#include "TrajectoryExceptions.h"

/**
 * SquaredEuclideanMetric class.
 *
 * The Squared Euclidean metric measures the similarity between two trajectories.
 */
template<typename Tr, typename To>
class SquaredEuclideanMetric: public Metric<Tr, To>
{
public:
	/**
	 * Compute distance between two trajectories.
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
			diff += pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2);
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

#endif /* SQUAREDEUCLIDIANMETRIC_H_ */
