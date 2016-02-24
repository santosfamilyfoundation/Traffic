#ifndef HAUSDORFFMETRIC_H_
#define HAUSDORFFMETRIC_H_

#include "Metric.h"

#include "TrajectoryExceptions.h"

/**
 * HausdorffMetric class.
 *
 * The Hausdorff metric measures the similarity between two trajectories.
 */
template<typename Tr, typename To>
class HausdorffMetric: public Metric<Tr, To>
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

		checkTrajectoryLength(a->size(), b->size());

		To distAB = computeDistance(a, b);
		To distBA = computeDistance(b, a);

		result = std::max(distAB, distBA);
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
		if (a == 0 || b == 0)
		{
			throw TrajectoryLengthErrorException();
		}
	}

	/**
	 * Compute the maximum distance from the trajectory given as a first parameter to the other.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @return distance between two trajectories
	 */
	To computeDistance(const Trajectory<Tr> *a, const Trajectory<Tr> *b)
	{
		To maxDist = -1;
		for (unsigned ia = 0; ia < a->size(); ++ia)
		{
		  To minDist = (To)norm(a->getPoint(ia) - b->getPoint(0));
			for (unsigned ib = 1; ib < b->size(); ++ib)
			{
			  To dist = (To)norm(a->getPoint(ia) - b->getPoint(ib));
				if (dist < minDist)
				{
					minDist = dist;
				}
			}

			if (minDist > maxDist)
			{
				maxDist = minDist;
			}
		}

		return maxDist;
	}
};

#endif /* HAUSDORFFMETRIC_H_ */
