#include <iostream>
#include <iterator>
#include <set>
#include <algorithm>

#include "../src/TrajectoryDBAccessList.h"
#include "../src/EuclideanMetric.h"
#include "../src/DTWMetric.h"
#include "../samples/TrajectoriesReader.h"

using namespace std;

//CONFIGURATION BEGIN
typedef int PointCoordinateType;
typedef CvPoint PointType;
string database = "data/dataset.sqlite";
const unsigned truncateToNbOfPoints = 10;
const unsigned nbOfGroups = 10;
const unsigned k_knn = 5;
//CONFIGURATION END

template<typename TdiffType>
void simpleKMeans(const unsigned int k_kmeans,
		const vector<TrajectoryPoint*> *trajectories,
		const unsigned int maxIterations,
		Metric<cv::Point_<int>, TdiffType> *metric,
		vector<TrajectoryPoint*> *clusters, vector<int> &trajectoryBelongsTo)
{
	assert( clusters != NULL );
	assert( clusters->size() == k_kmeans );
	assert( clusters->size() > 0 );
	assert( trajectoryBelongsTo.size() == trajectories->size() );
	assert( trajectories->size() >= clusters->size() );

	for (unsigned int i = 1; i < trajectories->size(); ++i)
	{
		assert( (*trajectories)[i]->size() == (*trajectories)[i - 1]->size() );
	}

	srand(time(NULL));
	int copyTrajectoryId = 0;
	for (unsigned int i = 0; i < k_kmeans; ++i)
	{
		copyTrajectoryId = rand() % trajectories->size();
		(*clusters)[i]
				= new TrajectoryPoint(*(*trajectories)[copyTrajectoryId]);

		TrajectoryPoint* trajectory = (*clusters)[i];
		for (unsigned int j = 0; j < trajectory->size(); ++j)
		{
			(*trajectory)[j].x = 0;
			(*trajectory)[j].y = 0;
		}
	}

	int nbOfElementsInCluster[clusters->size()];

	for (unsigned int xIter = 0; xIter < maxIterations; ++xIter)
	{
		for (unsigned int i = 0; i < trajectories->size(); ++i) //for each trajectory find a closest cluster
		{
			trajectoryBelongsTo[i] = 0;

			TdiffType bestDistToCenter = 0;
			metric->distance((*trajectories)[i], (*clusters)[0],
					bestDistToCenter);

			for (unsigned int j = 1; j < clusters->size(); ++j)
			{
				TdiffType distToCenter = 0;
				metric->distance((*trajectories)[i], (*clusters)[j],
						distToCenter);

				if (distToCenter < bestDistToCenter)
				{
					trajectoryBelongsTo[i] = j;
				}
			}
		}

		for (unsigned int i = 0; i < k_kmeans; ++i)
		{
			TrajectoryPoint* trajectory = (*clusters)[i];
			for (unsigned int j = 0; j < trajectory->size(); ++j)
			{
				(*trajectory)[j].x = 0;
				(*trajectory)[j].y = 0;
			}

			nbOfElementsInCluster[i] = 0;
		}

		for (unsigned int i = 0; i < trajectories->size(); ++i)
		{
			unsigned int clusterId = trajectoryBelongsTo[i];
			nbOfElementsInCluster[clusterId]++;

			TrajectoryPoint* trajectory = (*trajectories)[i];
			for (unsigned int j = 0; j < trajectory->size(); ++j)
			{
				(*(*clusters)[clusterId])[j].x += (*trajectory)[j].x;
				(*(*clusters)[clusterId])[j].y += (*trajectory)[j].y;
			}
		}

		for (unsigned int i = 0; i < k_kmeans; ++i)
		{
			TrajectoryPoint* trajectory = (*clusters)[i];
			for (unsigned int j = 0; j < trajectory->size(); ++j)
			{
				(*trajectory)[j].x /= (nbOfElementsInCluster[i] + 1);//xIter == 0 ? 0 : 1);
				(*trajectory)[j].y /= (nbOfElementsInCluster[i] + 1);//xIter == 0 ? 0 : 1);
			}
		}
	}

	for (unsigned int i = 0; i < k_kmeans; ++i)
	{
		cout << "Set " << i << "\tsize=" << nbOfElementsInCluster[i] << endl;
	}
}

void kmeansDemo(const vector<TrajectoryPoint*> *trajectories,
		vector<int> &trajectoryGroupId)
{
	const unsigned int k_kmeans = 10;
	const unsigned int maxIterations = 100;
	Metric<cv::Point_<int>, double> *metric = new EuclideanMetric<cv::Point_<int>,double > ();
	vector<TrajectoryPoint*> *clusters =
			new vector<TrajectoryPoint*> (k_kmeans);
	//vector<unsigned int> *trajectoryBelongsTo = new vector<unsigned int> (trajectories->size());

	simpleKMeans(k_kmeans, trajectories, maxIterations, metric, clusters,
			trajectoryGroupId);
}

enum HierarchicalClusteringMethod
{
	CompleteLinkageClustering,
	SingleLinkageClustering,
	AverageLinkageClustering
};

/*
 * Calculate distance between two sets of trajectories.
 */
template<typename TdiffType>
TdiffType simpleHierarchicalClustering_CalculateDistance(const vector<
		TrajectoryPoint*> *trajectories, const set<unsigned int> *s1,
		const set<unsigned int> *s2, const HierarchicalClusteringMethod method,
		Metric<cv::Point_<int>, TdiffType> *metric)
{
	assert( trajectories != NULL );
	assert( s1 != NULL && s1->size() > 0 );
	assert( s2 != NULL && s2->size() > 0 );
	assert( metric != NULL );

	TdiffType result;
	bool firstComparision = true;
	for (unsigned int i = 0; i < s1->size(); ++i)
	{
		const TrajectoryPoint *t1 = (*trajectories).at(i);

		for (unsigned int j = 0; j < s2->size(); ++j)
		{
			const TrajectoryPoint *t2 = (*trajectories).at(j);

			TdiffType distance = 0;
			metric->distance(t1, t2, distance);

			if (firstComparision)
			{
				firstComparision = false;
				result = distance;
			}
			else
			{
				switch (method)
				{
				case CompleteLinkageClustering:
					result = max(result, distance);
					break;
				case SingleLinkageClustering:
					result = min(result, distance);
					break;
				case AverageLinkageClustering:
					result += distance;
					break;
				}
			}
		}
	}

	if (method == AverageLinkageClustering)
	{
		result /= (s1->size() * s2->size());
	}

	return result;
}

/*
 * Calculate which two sets should be merged. Choose two closest sets.
 */
template<typename TdiffType>
pair<unsigned int, unsigned int> simpleHierarchicalClustering_CalculateClosest(
		const vector<TrajectoryPoint*> *trajectories, const vector<set<
				unsigned int>*> *sets,
		Metric<cv::Point_<int>, TdiffType> *metric,
		const HierarchicalClusteringMethod method)
{
	assert( trajectories != NULL );
	assert( sets != NULL && sets->size() > 1 );
	assert( metric != NULL );

	pair<unsigned int, unsigned int> bestPair;
	TdiffType bestDistance;
	bool firstComparision = true;
	for (unsigned int i = 0; i < sets->size(); ++i)
	{
		for (unsigned int j = i + 1; j < sets->size(); ++j)
		{
			TdiffType currentDistance =
					simpleHierarchicalClustering_CalculateDistance(
							trajectories, (*sets)[i], (*sets)[j], method,
							metric);

			if (firstComparision || currentDistance < bestDistance)
			{
				firstComparision = false;

				bestDistance = currentDistance;

				bestPair.first = i;
				bestPair.second = j;
			}
		}
	}

	return bestPair;
}

/*
 * Hierarchical Clustering Algorithm.
 */
template<typename TdiffType>
vector<set<unsigned int>*> *simpleHierarchicalClustering(const vector<
		TrajectoryPoint*> *trajectories,
		Metric<cv::Point_<int>, TdiffType> *metric, unsigned int nbOfGroups,
		const HierarchicalClusteringMethod method)
{
	assert(trajectories != NULL);
	assert(metric != NULL);
	assert(nbOfGroups > 0);

	vector<set<unsigned int>*> *sets = new vector<set<unsigned int>*> ();

	for (unsigned int i = 0; i < trajectories->size(); ++i) //create N one-element sets
	{
		set<unsigned int> *newSet = new set<unsigned int> ();
		newSet->insert(newSet->begin(), i);

		sets->insert(sets->begin(), newSet);
	}

	for (unsigned int i = trajectories->size(); i > nbOfGroups; --i) //merge till get nbOfGroups groups
	{
		pair<unsigned int, unsigned int> bestPair =
				simpleHierarchicalClustering_CalculateClosest(trajectories,
						sets, metric, method);

		unsigned int a = bestPair.first;
		assert( a >= 0 && a < sets->size() );
		unsigned int b = bestPair.second;
		assert( b >= 0 && b < sets->size() );

		(*sets)[a]->insert((*sets)[b]->begin(), (*sets)[b]->end());

		delete (*sets)[b];

		sets->erase(sets->begin() + b);
	}

	return sets;
}

vector<unsigned int> *assignIdForEachGroup(vector<set<unsigned int>*> *sets,
		const HierarchicalClusteringMethod method,
		vector<int> &realTrajectoryGroupId)
{
	assert( sets != NULL );
	assert( realTrajectoryGroupId.size() > 0 );

	vector<unsigned int> *setId = new vector<unsigned int> ();
	const unsigned int nbOfGroups = sets->size();
	const unsigned int nbOfTrajectories = realTrajectoryGroupId.size();
	unsigned int cnt[nbOfGroups];
	for (unsigned int i = 0; i < nbOfGroups; ++i)
	{
		set<unsigned int>::iterator itCurr = (*sets)[i]->begin();
		const set<unsigned int>::iterator itEnd = (*sets)[i]->end();

		for (unsigned int j = 0; j < nbOfGroups; ++j)
		{
			cnt[j] = 0;
		}

		pair<unsigned int, unsigned int> best = make_pair(0, 0);
		bool firstComparision = true;
		for (; itCurr != itEnd; ++itCurr)
		{
			const unsigned int trajectoryIdx = *itCurr;
			assert(trajectoryIdx >= 0 && trajectoryIdx < nbOfTrajectories);

			const unsigned int trajectoryId =
					realTrajectoryGroupId[trajectoryIdx];
			assert( trajectoryId >= 0 && trajectoryId < nbOfGroups );

			cnt[trajectoryId]++;

			if (firstComparision || cnt[trajectoryId] > best.first)
			{
				firstComparision = false;

				best.first = cnt[trajectoryId];
				best.second = trajectoryId;
			}
		}

		assert( best.second >= 0 && best.second < nbOfGroups );
		setId->push_back(best.second);
	}

	return setId;
}

template<typename TdiffType>
bool simpleKNNPairComparisonSmaller(const pair<unsigned int, TdiffType> &pairA,
		const pair<unsigned int, TdiffType> &pairB)
{
	return pairA.second < pairB.second;
}

template<typename TdiffType>
unsigned int simpleKNN(unsigned int k_knn,
		const vector<TrajectoryPoint*> *trajectories,
		const unsigned int nbOfGroups,
		vector<unsigned int> *trajectoriesGroupId,
		const unsigned int idOfTrajectory,
		Metric<cv::Point_<int>, TdiffType> *metric)
{
	assert(trajectories != NULL);
	assert( k_knn <= trajectories->size() );
	assert(metric != NULL);

	pair<unsigned int, TdiffType> distances[trajectories->size() - 1];
	for (unsigned int i = 0, j = 0; i < trajectories->size(); ++i)
	{
		if (i != idOfTrajectory)
		{
			distances[j].first = (*trajectoriesGroupId)[i];
			metric->distance((*trajectories)[idOfTrajectory],
					(*trajectories)[i], distances[j].second);
			j++;
		}
	}

	sort(distances, distances + trajectories->size() - 1,
			simpleKNNPairComparisonSmaller<TdiffType> );

	unsigned int cnt[nbOfGroups];

	for (unsigned int i = 0; i < nbOfGroups; ++i)
	{
		cnt[i] = 0;
	}

	pair<unsigned int, unsigned int> best;
	bool firstComparision = true;
	for (unsigned int i = 0; i < k_knn; ++i)
	{
		unsigned int j = distances[i].first;
		assert( j >= 0 && j < nbOfGroups );

		cnt[j]++;

		if (firstComparision || cnt[j] > best.second)
		{
			firstComparision = false;

			best.first = j;
			best.second = cnt[j];
		}
	}

	return best.first;
}

template<typename TdiffType>
void testClassification(unsigned int k_knn,
		const vector<TrajectoryPoint*> *trajectories, const vector<set<
				unsigned int>*> *sets, vector<unsigned int> *setId,
		vector<int> &realTrajectoryGroupId,
		Metric<cv::Point_<int>, TdiffType> *metric)
{
	cout << "Testing.." << endl;
	assert( trajectories != NULL );
	assert( sets != NULL );
	assert( setId != NULL );
	assert( sets->size() == setId->size() );
	assert( trajectories->size() == realTrajectoryGroupId.size() );
	assert( metric != NULL );

	unsigned int nTrajectories = trajectories->size();
	vector<unsigned int> *trajectoriesGroupId = new vector<unsigned int> (
			nTrajectories);

	for (unsigned int i = 0; i < sets->size(); ++i)
	{
		assert( (*sets)[i] != NULL );
		set<unsigned int>::iterator itCurr = (*sets)[i]->begin();
		const set<unsigned int>::iterator itEnd = (*sets)[i]->end();
		for (; itCurr != itEnd; ++itCurr)
		{
			unsigned int j = *itCurr;
			assert(j >= 0 && j < trajectoriesGroupId->size());
			assert( i >= 0 && i < setId->size() );
			(*trajectoriesGroupId)[j] = (*setId)[i];
		}
	}

	const unsigned int nbOfGroups = sets->size();
	unsigned int resultPositive[nbOfGroups];
	unsigned int resultNegative[nbOfGroups];

	for (unsigned int i = 0; i < nbOfGroups; ++i)
	{
		resultPositive[i] = 0;
		resultNegative[i] = 0;
	}

	for (unsigned int i = 0; i < trajectories->size(); ++i)
	{
		unsigned int groupId = simpleKNN(k_knn, trajectories, nbOfGroups,
				trajectoriesGroupId, i, metric);
		assert( groupId >= 0 && groupId < nbOfGroups );

		unsigned int trajectoryGroupId = (*trajectoriesGroupId)[i];
		assert( trajectoryGroupId >= 0 && trajectoryGroupId < nbOfGroups );

		if (groupId == trajectoryGroupId)
		{
			resultPositive[trajectoryGroupId]++;
		}
		else
		{
			resultNegative[trajectoryGroupId]++;
		}
	}

	for (unsigned i = 0; i < nbOfGroups; ++i)
	{
		cout << "Group " << i << " NbOfPossitive=" << resultPositive[i]
				<< " NbOfNegative=" << resultNegative[i] << endl;
	}
}

void test(vector<TrajectoryPoint*> *trajectories,
		vector<int> &trajectoryGroupId,
		Metric<cv::Point_<int>, double> *metric,
		HierarchicalClusteringMethod hierarchicalClusteringMethod)
{
	cout << "Clustering trajectories.. ";
	cout.flush();
	vector<set<unsigned int>*> *sets = simpleHierarchicalClustering(
			trajectories, metric, nbOfGroups, hierarchicalClusteringMethod);
	cout << "done" << endl;
	assert( sets != NULL && sets->size() == nbOfGroups );

	for (unsigned int i = 0; i < sets->size(); ++i)
	{
		cout << "Set " << i << ": ";

		set<unsigned int>::iterator itCurr = (*sets)[i]->begin();
		set<unsigned int>::iterator itEnd = (*sets)[i]->end();

		for (; itCurr != itEnd; ++itCurr)
		{
			cout << *itCurr << " ";
		}

		cout << endl;
	}

	vector<unsigned int> *setId = assignIdForEachGroup(sets,
			hierarchicalClusteringMethod, trajectoryGroupId);

	testClassification(k_knn, trajectories, sets, setId, trajectoryGroupId,
			metric);

	delete setId;
	delete sets;
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
	  database = string(argv[1]);
	}

	vector<TrajectoryPoint*> *trajectories = new vector<TrajectoryPoint*> (); //trajectories
	vector<int> trajectoryGroupId; //id of activity of trajectory
	readAllTrajectories(database.c_str(), trajectories, trajectoryGroupId); //read trajectories and ids

	assert( trajectories != NULL );
	assert( trajectories->size() > 0 );

	{
		cout << "Truncating trajectories up to " << truncateToNbOfPoints
				<< " points.. ";
		cout.flush();

		for (unsigned i = 0; i < trajectories->size(); ++i)
		{
			while ((*trajectories)[i]->size() > truncateToNbOfPoints)
			{
				(*trajectories)[i]->pop_back();
			}
		}

		cout << "done" << endl;
	}

	//kmeansDemo(trajectories, trajectoryGroupId);
	//return 0;

	{
		cout << "\n---- Euclidean Metric + Complete Linkage Clustering ----\n";
		Metric<cv::Point_<int>, double> *metric = new EuclideanMetric<
		  cv::Point_<int>,double > ();
		test(trajectories, trajectoryGroupId, metric, CompleteLinkageClustering);
		delete metric;
	}

	{
		cout << "\n---- DTW Metric + Average Linkage Clustering ----\n";
		Metric<cv::Point_<int>, double> *metric = new DTWMetric<cv::Point_<int>,double > ();
		test(trajectories, trajectoryGroupId, metric, AverageLinkageClustering);
		delete metric;
	}

	return 0;
}
