#include "../src/TrajectoryDBAccessBlob.h"
#include "../src/TrajectoryDBAccessList.h"
#include "../src/TrajectoryWriter.h"
#include "../src/EuclideanMetric.h"
#include "../src/DTWMetric.h"
#include "../src/LCSMetric.h"
#include "../src/HausdorffMetric.h"
#include "TrajectoryGenerator.h"
#include "TrajectoryWriter.h"
#include "ReadNumber.h"

#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cstdlib>

using namespace std;

vector<TrajectoryPoint*> *readTrajectoriesFromDB(const char *database)
{
	TrajectoryDBAccess<cv::Point_<int> > *db = new TrajectoryDBAccessList<cv::Point_<int> > ();

	{
		cout << "Connecting... ";
		bool success = db->connect(database);
		cout << (success ? "OK" : "ERROR") << endl;
	}

	{
		cout << "Number of trajectories in DB... ";
		unsigned int size;
		bool success = db->size(size);
		if (success)
		{
			cout << size;
		}
		else
		{
			cout << "ERROR";
		}
		cout << endl;
	}

	vector<TrajectoryPoint*> *trajectories = NULL;
	{
		cout << "Reading trajectories from DB... ";
		trajectories = new vector<TrajectoryPoint*> ();
		bool success = db->read(trajectories);
		cout << (success ? "OK" : "ERROR") << " (read " << trajectories->size() << " trajectories)" << endl;
	}

	/*
	 {
	 cout << "Deleting a table... ";
	 bool success = db->deleteTable();
	 cout << (success ? "OK" : "ERROR") << endl;
	 }
	 */

	{
		cout << "Disconnecting... ";
		bool success = db->disconnect();
		cout << (success ? "OK" : "ERROR") << endl;
	}

	delete db;

	return trajectories;
}

template<typename To>
int chooseTheMostSimilar(TrajectoryPoint *trajectory, vector<TrajectoryPoint> *trajectories, Metric<cv::Point_<int>, To> *metric, bool smallest = true)
{
	To minDiff = std::numeric_limits<To>::max();
	int minDiffIndex = -1;
	for (unsigned int i = 0; i < trajectories->size(); ++i)
	{
		To diff = 0;
		metric->distance(trajectory, &(*trajectories)[i], diff);
		if (!smallest)
		{
			diff = std::numeric_limits<To>::max() - diff;
		}
		if (minDiffIndex == -1 || diff < minDiff)
		{
			minDiff = diff;
			minDiffIndex = i;
		}
	}
	return minDiffIndex;
}

template<typename T>
void writeTrajectoryOnImage(const char *filename, vector<Trajectory<T> >*trajectories, Trajectory<T> *compTrajectory)
{
	cout << "Saving the most similar trajectory on image... ";

	TrajectoryWriter tw;

	//option 1
	//IplImage *img = 0;
	//img = tw.write(trajectory, img);

	//option 2
	//IplImage *img = cvCreateImage(cvSize(200, 200), 8, 3);
	//cvZero(img);
	//img = tw.write(trajectory, img);

	//option 3
	const CvScalar colorsCvScalar[5] =
	{ CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), CV_RGB(128, 0, 128), CV_RGB(255, 255, 0) };
	const string colorsString[5] =
	{ "red", "green", "blue", "purple", "yellow" };

	T _min = min(*compTrajectory);
	T _max = max(*compTrajectory);
	CvSize imgSize = cvSize(_max.x, _max.y);
	for (unsigned int i = 0; i < trajectories->size(); ++i)
	{
		_min = min((*trajectories)[i]);
		_max = max((*trajectories)[i]);
		imgSize.width = max(imgSize.width, _max.x);
		imgSize.height = max(imgSize.height, _max.y);
	}

	imgSize.width += 20;
	imgSize.height += 20;
	cv::Mat img = cv::Mat::zeros(imgSize, CV_8UC3);
	for (unsigned int i = 0; i < trajectories->size(); ++i)
	{
	  tw.write((*trajectories)[i], colorsCvScalar[i % 4], img);
	  cout << "\n\t" << "Trajectory " << i << ": color " << colorsString[i % 4];
	}

	tw.write(*compTrajectory, colorsCvScalar[4], img);
	cout << "\n\t" << "Trajectory used for comparison" << ": color " << colorsString[4] << "\n\t";

	bool success = !img.empty();
	if (!success)
	{
		cout << "ERROR (during drawing the trajectory on an image)";
	}
	else
	{
		assert(filename != 0 && img != 0);
		success &= imwrite(filename, img);
		cout << (success ? "OK" : "ERROR (during saving an image)") << endl;
		//cvReleaseImage(&img);
	}
}

int main()
{
	unsigned nbOfPoints = readUnsignedInt("Set number of points of trajectories: ");

	//const char *database = "test.sqlite";
	const char *database = "/home/francois/Unison/École/12Été/CRSNG/Data/NGSIM.sqlite";

	//srand(time(NULL));

	{
		vector<TrajectoryPoint> *trajectories = generateTrajectories(4, nbOfPoints, 50);
		writeTrajectoriesInDB(database, trajectories);
	}

	{
		vector<TrajectoryPoint*> *_trajectories = readTrajectoriesFromDB(database);
		vector<TrajectoryPoint> *trajectories = new vector<TrajectoryPoint> ();
		for (unsigned i = 0; i < _trajectories->size(); ++i)
		{
			trajectories->push_back(*(*_trajectories)[i]);
		}
		delete _trajectories;

		TrajectoryPoint *trajectory = generateTrajectory(trajectories->back().getId() + 1, nbOfPoints, 50);

		{ //Choosing the most similar trajectory
			{ //EuclideanMetric
				cout << "Choosing the most similar trajectory using Euclidean metric... ";
				int index = chooseTheMostSimilar(trajectory, trajectories, new EuclideanMetric<cv::Point_<int>,double > ());
				bool success = index >= 0 && index < int(trajectories->size());
				cout << (success ? "OK" : "ERROR") << " (index of the trajectory: " << index << ")" << endl;
			}

			{ //DTWMetric
				cout << "Choosing the most similar trajectory using Dynamic Time Warping (DTW) metric... ";
				int index = chooseTheMostSimilar(trajectory, trajectories, new DTWMetric<cv::Point_<int>,double > ());
				bool success = index >= 0 && index < int(trajectories->size());
				cout << (success ? "OK" : "ERROR") << " (index of the trajectory: " << index << ")" << endl;
			}

			{ //LCSMetric
				cout << "Choosing the most similar trajectory using Longest Common Subsequence (LCS) metric... ";
				LCSMetric<cv::Point_<int>, unsigned int > *metric = new LCSMetric<cv::Point_<int>, unsigned int > ();
				metric->setSimilarityThreshold(16.0);
				int index = chooseTheMostSimilar(trajectory, trajectories, metric, false);
				bool success = index >= 0 && index < int(trajectories->size());
				cout << (success ? "OK" : "ERROR") << " (index of the trajectory: " << index << ")" << endl;
			}

			{ //HausdorffMetric
				cout << "Choosing the most similar trajectory using Hausdorff metric... ";
				HausdorffMetric<cv::Point_<int>,double > *metric = new HausdorffMetric<cv::Point_<int>,double > ();
				int index = chooseTheMostSimilar(trajectory, trajectories, metric);
				bool success = index >= 0 && index < int(trajectories->size());
				cout << (success ? "OK" : "ERROR") << " (index of the trajectory: " << index << ")" << endl;
			}
		}

		writeTrajectoryOnImage("img.jpg", trajectories, trajectory);
	}

	return 0;
}
