#ifndef LCSMETRIC_H_
#define LCSMETRIC_H_

#include "Metric.h"

/**
 * LCSMetric class.
 *
 * The Longest Common Subsequence metric
 * This class mesures :
 * 1) the similarity (number of points in "common", ie relative to distance similarity threshold) ; 
 * 2) the normalized distance between two trajectories
 */

template<typename Tr, typename To>
class LCSMetric: public Metric<Tr, To>
{
public:
	/**
	 * Constructor.
	 */
 LCSMetric() : similarityThreshold(0.0), eps(1e-6)
	{
	}

	/**
	 * Set similarity threshold between two points.
	 *
	 * @param[in] similarityThreshold similarity threshold
	 */
	bool setSimilarityThreshold(double similarityThreshold)
	{
		if (similarityThreshold >= 0.0)
		{
			this->similarityThreshold = similarityThreshold;
			return true;
		}
		return false;
	}

	/**
	 * Set machine epsilon.
	 *
	 * @param[in] eps machine epsilon
	 */
	bool setEps(double eps)
	{
		if (eps >= 0.0)
		{
			this->eps = eps;
			return true;
		}
		return false;
	}

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
	  unsigned int LCSS = 0;
	  similarity(a,b, LCSS); 
	  unsigned int min_size = min(a->size(),b->size());
	  result = 1 - double(LCSS/min_size);
	}

	/**
	 * Compute similarity between two trajectories.
	 *
	 * @param[in] a input trajectory
	 * @param[in] b input trajectory
	 * @param[out] result similarity between two trajectories
	 */
	void similarity(const Trajectory<Tr> *a, const Trajectory<Tr> *b, unsigned int &result)
	{
		unsigned int LCS[a->size() + 1][b->size() + 1];

		{ //initialisation
			for (unsigned int i = 0; i <= a->size(); ++i)
			{
				LCS[i][0] = 0;
			}

			for (unsigned int j = 0; j <= b->size(); ++j)
			{
				LCS[0][j] = 0;
			}
		}

		{ //algorithm
			for (unsigned int i = 1; i <= a->size(); ++i)
			{
				for (unsigned int j = 1; j <= b->size(); ++j)
				{
					cv::Point3_<typeof(static_cast<Tr>(a->getPoint(i-1)).x)> p(a->getPoint(i - 1) - b->getPoint(j - 1));
					double distance = sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2)); // il faudrait généraliser
					if (distance <= similarityThreshold + eps)
					{ //a[i] == b[j]
						LCS[i][j] = LCS[i - 1][j - 1] + 1;
					}
					else
					{
					  LCS[i][j] = std::max(LCS[i - 1][j], LCS[i][j - 1]);
					}
				}
			}
		}

		result = LCS[a->size()][b->size()];
	}

private:
	/**
	 * Similarity threshold between two points.
	 */
	double similarityThreshold;

	/**
	 * Machine epsilon.
	 */
	double eps;

};

#endif /* LCSMETRIC_H_ */
