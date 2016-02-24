#ifndef DTWMETRIC_H_
#define DTWMETRIC_H_

#include "Metric.h"

/**
 * DTWMetric class.
 *
 * The Dynamic Time Warping metric measures the similarity between two trajectories.
 */
template<typename Tr, typename To>
class DTWMetric: public Metric<Tr, To>
{
public:
	/**
	 * Constructor.
	 */
	DTWMetric()
	{
		maxDTWValue = (std::numeric_limits<To>::max()) / To(2);
	}

	/**
	 * Set maximum value that occurs during the calculation process.
	 *
	 * @param[in] maxDTWValue maximum value
	 */
	bool setMaxDTWValue(To maxDTWValue)
	{
		if (maxDTWValue >= 0.0)
		{
			this->maxDTWValue = maxDTWValue;
			return true;
		}
		return false;
	}

	/**
	 * Set maximum value that occurs during the calculation process.
	 *
	 * @param[in] imgSize image size
	 * @param[in] eps machine epsilon
	 */
	bool setMaxDTWValue(CvSize &imgSize, double eps)
	{
		if (imgSize.width > 0 && imgSize.height > 0 && eps > 0)
		{
			cv::Point_<int> p(imgSize.width, imgSize.height);
			maxDTWValue = std::numeric_limits<To>::max() - norm(p) - eps;
			return true;
		}
		return false;
	}

	/**
	 * Compute similarity between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result distance between two trajectories.
	 */
	void distance(const Trajectory<Tr> *a, const Trajectory<Tr> *b, To &result, unsigned int nbOfPoints = std::numeric_limits<unsigned int>::max()) 
	{
		To DTW[a->size() + 1][b->size() + 1];

		{ //initialization
			for (unsigned int i = 0; i <= a->size(); ++i)
			{
				DTW[i][0] = maxDTWValue;
			}

			for (unsigned int j = 0; j <= b->size(); ++j)
			{
				DTW[0][j] = maxDTWValue;
			}

			DTW[0][0] = 0.0;
		}

		{ //algorithm
			for (unsigned int i = 1; i <= a->size(); ++i)
			{
				for (unsigned int j = 1; j <= b->size(); ++j)
				{
					To smallest = min(DTW[i - 1][j], DTW[i][j - 1], DTW[i - 1][j - 1]);
					cv::Point3_<typeof(static_cast<Tr>(a->getPoint(i-1)).x)> p(a->getPoint(i - 1) - b->getPoint(j - 1));
					To distance = std::sqrt(std::pow(p.x, 2) + std::pow(p.y, 2) + std::pow(p.z, 2));
					DTW[i][j] = std::min(smallest + distance, maxDTWValue);
				}
			}
		}

		result = DTW[a->size()][b->size()];
	}

private:
	/**
	 * Maximum value that occurs during the calculation process.
	 */
	To maxDTWValue;

	/**
	 * Compute the minimum of \a a, \a b and \c c.
	 *
	 * @param[in] a input parameter
	 * @param[in] b input parameter
	 * @param[in] c input parameter
	 */
	To min(To a, To b, To c)
	{
		return std::min(std::min(a, b), c);
	}
};

#endif /* DTWMETRIC_H_ */
