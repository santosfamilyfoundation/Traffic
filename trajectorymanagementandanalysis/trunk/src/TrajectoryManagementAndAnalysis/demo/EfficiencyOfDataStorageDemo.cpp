#include <iostream>
#include <ctime>
#include "ReadNumber.h"
#include "../src/Trajectory.h"
#include "../src/TrajectoryDBAccessList.h"
#include "../src/TrajectoryDBAccessBlob.h"
#include "TrajectoryGenerator.h"
#include "../src/TrajectoryWriter.h"
using namespace std;

void addFixedTrajectories(vector<TrajectoryPoint3d> *trajectories, const unsigned quantity, const unsigned length, const double valueX, const double valueY, const double valueZ)
{
	assert(trajectories != NULL);

	for (unsigned int i = 0; i < quantity; ++i)
	{
		TrajectoryPoint3d *t = generateFixedTrajectory(i + 1, length, valueX, valueY, valueZ);
		trajectories->push_back(*t);
	}
}

double saveTrajectories(TrajectoryDBAccess<cv::Point3_<double> > *db, const char* database, const vector<TrajectoryPoint3d>* trajectories)
{
	db->connect(database);
	db->deleteTable();
	db->createTable();

	std::clock_t start, stop;

	start = std::clock();

	for (unsigned int i = 0; i < trajectories->size(); ++i)
	{
		db->write((*trajectories)[i]);
	}

	stop = std::clock();

	db->disconnect();

	double nbOfSec = double(stop - start) / double(CLOCKS_PER_SEC);
	return nbOfSec;
}

TrajectoryPoint3d *experiment(TrajectoryDBAccess<cv::Point3_<double> > *db)
{
	const unsigned int nbOfTrajectories[] =
	{ 10, 50, 100, 250, 500 };
	const unsigned int nbOfPoints[] =
	{ 10, 50, 100, 250, 500 };

	TrajectoryPoint3d *efficiencyTrajectory = new TrajectoryPoint3d();

	for (unsigned int i = 0; i < 5; ++i)
	{
		for (unsigned int j = 0; j < 5; ++j)
		{
			vector<TrajectoryPoint3d> *trajectories = new vector<TrajectoryPoint3d> ();

			addFixedTrajectories(trajectories, nbOfTrajectories[i], nbOfPoints[j], double(1.1), double(2.2), double(3.3));

			cout << "Saving " << trajectories->size() << " trajectories containing " << nbOfPoints[j] << " points each.. ";
			cout.flush();
			double nbOfSec = saveTrajectories(db, "efficiencydemo.sqlite", trajectories);
			cout << "done in " << nbOfSec << " seconds." << endl;

			efficiencyTrajectory->add(cv::Point3_<double>(nbOfTrajectories[i], nbOfPoints[j], nbOfSec));

			delete trajectories;
		}
	}

	return efficiencyTrajectory;
}

int main()
{
	cout << "This demo shows the efficiency of saving trajectories into databases.. " << endl;

	vector<TrajectoryPoint3d> *efficiencyTrajectories = new vector<TrajectoryPoint3d> ();

	{
		cout << "\nUsing blob representation.." << endl;
		TrajectoryDBAccess<cv::Point3_<double> > *db = new TrajectoryDBAccessBlob<cv::Point3_<double> > ();
		TrajectoryPoint3d *efficiencyTrajectory = experiment(db);
		efficiencyTrajectories->push_back(*efficiencyTrajectory);
		delete db;
	}

	{
		cout << "\nUsing list representation.." << endl;
		TrajectoryDBAccess<cv::Point3_<double> > *db = new TrajectoryDBAccessList<cv::Point3_<double> > ();
		TrajectoryPoint3d *efficiencyTrajectory = experiment(db);
		efficiencyTrajectories->push_back(*efficiencyTrajectory);
		delete db;
	}

	TrajectoryWriter trajectoryWriter;
	trajectoryWriter.saveInMatlabFormat("efficiency.m", efficiencyTrajectories, false);

	delete efficiencyTrajectories;

	return 0;
}
