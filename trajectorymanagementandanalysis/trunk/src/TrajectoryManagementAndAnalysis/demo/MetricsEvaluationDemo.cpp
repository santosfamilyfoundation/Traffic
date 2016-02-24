
#include "../src/TrajectoryDBAccessList.h"
#include "../src/HausdorffMetric.h"
#include "../src/LCSMetric.h"
#include "../src/DTWMetric.h"
#include "../src/EuclideanMetric.h"
#include "../src/SquaredEuclideanMetric.h"
#include "../src/HuMetric.h"
#include "../src/ManhattanMetric.h"
#include "../src/ChebyshevMetric.h"
#include "../src/MinimumMetric.h"
#include "../samples/TrajectoriesReader.h"
using namespace std;

//CONFIGURATION BEGIN
typedef int PointCoordinateType;
typedef CvPoint PointType;
string database = "data/dataset.sqlite";
const unsigned nbOfGroups = 10;
const unsigned truncateToNbOfPoints = 20;
//CONFIGURATION END

template<typename TdiffType>
void test(vector<TrajectoryPoint*> *trajectories, vector<int> &trajectoryGroupId, Metric<cv::Point_<int>, TdiffType> *metric)
{
	assert( trajectories != NULL );
	assert( trajectories->size() > 1 );

	unsigned resultPositive[nbOfGroups];
	unsigned resultNegative[nbOfGroups];

	for (unsigned i = 0; i < nbOfGroups; ++i)
	{
		resultPositive[i] = 0;
		resultNegative[i] = 0;
	}

	for (unsigned i = 0; i < trajectories->size(); ++i)
	{
		TdiffType minDiff = std::numeric_limits<TdiffType>::max();
		unsigned minDiffIndex = i == 0 ? 1 : 0;

		for (unsigned j = 0; j < trajectories->size(); ++j)
		{
			if (i != j)
			{
				TdiffType diff = 0;
				metric->distance((*trajectories)[i], (*trajectories)[j], diff);

				if (diff < minDiff)
				{
					minDiff = diff;
					minDiffIndex = j;
				}
			}
		}

		unsigned trajectoryGroupI = trajectoryGroupId[i];
		unsigned trajectoryGroupJ = trajectoryGroupId[minDiffIndex];

		if (trajectoryGroupI == trajectoryGroupJ)
		{
			resultPositive[trajectoryGroupI]++;
		}
		else
		{
			resultNegative[trajectoryGroupI]++;
		}
	}

	for (unsigned i = 0; i < nbOfGroups; ++i)
	{
		cout << "Group " << i << " NbOfPossitives=" << resultPositive[i] << " NbOfNegatives=" << resultNegative[i] << endl;
	}
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
	  database = string(argv[1]);
	}

	vector<TrajectoryPoint*> *trajectories = new vector<TrajectoryPoint*> ();
	vector<int> trajectoryGroupId;
	readAllTrajectories(database.c_str(), trajectories, trajectoryGroupId);

	{
		cout << endl;
		cout << "HausdorffMetric:" << endl;
		test<double> (trajectories, trajectoryGroupId, new HausdorffMetric<cv::Point_<int>,double > ());
	}

	{
		cout << endl;
		cout << "LCSMetric (with similarity threshold set to 10):" << endl;
		LCSMetric<cv::Point_<int>, unsigned int > *metric = new LCSMetric<cv::Point_<int>, unsigned int > ();
		metric->setSimilarityThreshold(10.0);
		test<unsigned> (trajectories, trajectoryGroupId, metric);
	}

	{
		cout << endl;
		cout << "DTWMetric:" << endl;
		test<double> (trajectories, trajectoryGroupId, new DTWMetric<cv::Point_<int>,double > ());
	}

	for (unsigned i = 0; i < trajectories->size(); ++i)
	{
		while ((*trajectories)[i]->size() > truncateToNbOfPoints)
		{
			(*trajectories)[i]->pop_back();
		}
	}

	{
		cout << endl;
		cout << "EuclideanMetric (on truncated up to 20 points trajectories):" << endl;
		test<double> (trajectories, trajectoryGroupId, new EuclideanMetric<cv::Point_<int>,double > ());
	}

	{
		cout << endl;
		cout << "SquaredEuclideanMetric (on truncated up to 20 points trajectories):" << endl;
		test<int> (trajectories, trajectoryGroupId, new SquaredEuclideanMetric<cv::Point_<int>, int> ());
	}

	{
		cout << endl;
		cout << "HuMetric (on truncated up to 20 points trajectories):" << endl;
		test<double> (trajectories, trajectoryGroupId, new HuMetric<cv::Point_<int>,double > ());
	}

	{
		cout << endl;
		cout << "ManhattanMetric (on truncated up to 20 points trajectories):" << endl;
		test<int> (trajectories, trajectoryGroupId, new ManhattanMetric<cv::Point_<int>, int> ());
	}

	{
		cout << endl;
		cout << "ChebyshevMetric (on truncated up to 20 points trajectories):" << endl;
		test<int> (trajectories, trajectoryGroupId, new ChebyshevMetric<cv::Point_<int>, int> ());
	}

	{
		cout << endl;
		cout << "MinimumMetric (on truncated up to 20 points trajectories):" << endl;
		test<int> (trajectories, trajectoryGroupId, new MinimumMetric<cv::Point_<int>, int> ());
	}

	delete trajectories;

	return 0;
}
