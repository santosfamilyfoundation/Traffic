#ifndef EUCLIDIANMETRIC_H_
#define EUCLIDIANMETRIC_H_

#include "SquaredEuclideanMetric.h"
#include "TrajectoryExceptions.h"

/**
 * EuclideanMetric class.
 *
 * The Euclidean metric measures the distance between two trajectories.
 */
template<typename Tr, typename To>
class EuclideanMetric: public Metric<Tr, To>
{
public:
	/**
	 * Constructor.
	 */
	EuclideanMetric()
	{
	  squaredEuclideanMetric = new SquaredEuclideanMetric<Tr, To> ();
	}

	/**
	 * Destructor.
	 */
	~EuclideanMetric()
	{
		delete squaredEuclideanMetric;
	}

	/**
	 * Compute distance between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result distance between two trajectories
	 */
	void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) 
	{
		To diff = To(0);

		squaredEuclideanMetric->distance(a, b, diff, nbOfPoints);

		diff = sqrt(diff);

		result = diff;
	}

private:
	/**
	 * Pointer to a SquaredEuclideanMetric class.
	 */
	Metric<Tr, To> *squaredEuclideanMetric;
};

#endif /* EUCLIDIANMETRIC_H_ */
