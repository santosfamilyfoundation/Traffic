#ifndef CANBERRAMETRIC_H_
#define CANBERRAMETRIC_H_

#include "Metric.h"

#include "TrajectoryExceptions.h"

/**
 * CanberraMetric class.
 *
 * The Canberra metric measures the similarity between two trajectories.
 */
template<typename Tr>
class CanberraMetric: public Metric<Tr, double>
{
public:
	/**
	 * Compute distance between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result distance between two trajectories
	 */
  void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, double &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) 
	{
		result = double(0);
		
		if (nbOfPoints == std::numeric_limits<unsigned int>::max())
		  checkTrajectoryLength(a->size(), b->size());

		double diff = double(0);
		for (unsigned int i = 0; i < a->size() and i < nbOfPoints; ++i)
		{
			cv::Point3_<typeof(static_cast<Tr>(a->getPoint(i)).x)> A(a->getPoint(i));
			cv::Point3_<typeof(static_cast<Tr>(a->getPoint(i)).x)> B(b->getPoint(i));
			diff += calculate(A.x, B.x);
			diff += calculate(A.y, B.y);
			diff += calculate(A.z, B.z);
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

	/**
	 * Compute similarity between two values.
	 *
	 * @param a first value
	 * @param b second value
	 * @return result
	 */
	double calculate(double a, double b)
	{
		double result = std::abs(a - b);

		a = std::fabs(a);
		b = std::fabs(b);

		a += b;

		if (a == 0)
		{
			return 0;
		}

		result /= a;

		return result;
	}
};

#endif /* CANBERRAMETRIC_H_ */
