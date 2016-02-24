#ifndef HUMETRIC_H_
#define HUMETRIC_H_

#include "EuclideanMetric.h"

#include "TrajectoryExceptions.h"

/**
 * HuMetric class.
 *
 * The Hu metric measures the normalized distance between two trajectories.
 */
template<typename Tr, typename To>
class HuMetric: public Metric<Tr, To>
{
public:
	/**
	 * Constructor.
	 */
	HuMetric()
	{
	  euclideanMetric = new EuclideanMetric<Tr,To> ();
	}

	/**
	 * Destructor.
	 */
	~HuMetric()
	{
		delete euclideanMetric;
	}

	/**
	 * Compute normalized distance between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result distance between two trajectories
	 */
	void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) 
	{
		To diff = To(0);

		euclideanMetric->distance(a, b, diff, nbOfPoints);

		diff /= To(a->size() * 2);

		result = diff;
	}

private:
	/**
	 * Pointer to EuclideanMetric class.
	 */
	Metric<Tr, To> *euclideanMetric;
};

#endif /* HUMETRIC_H_ */
