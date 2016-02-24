#include "../src/TrajectoryDBAccessList.h"
#include <fstream>
using namespace std;

//Files can be big so assumption was done.
//Assumption: Trajectories are ordered by id and sorted (asc) by frame number.
//To read NGSIM datasets and WEIZMANN dataset

//CONFIGURE BEGIN
const unsigned int NB_OF_COLUMNS = 18;
const unsigned int TRAJECTORY_ID = 0;
const unsigned int FRAME_NUMBER = 1;
const unsigned int X_COORDINATE = 4;
const unsigned int Y_COORDINATE = 5;
typedef double PointCoordinateType;
typedef cv::Point2d PointType;
const char* database = "trajectories.sqlite";
//CONFIGURE END

template<typename T> T convertString(string s)
{
	istringstream is(s);
	T x;
	is >> x;
	return x;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout
				<< "Error: Run application with one parameter (with path to a file containing trajectories)."
				<< endl;
		return 0;
	}

	const char* trajectoriesFileName = argv[1];

	ifstream trajectoriesFile;
	trajectoriesFile.open(trajectoriesFileName, ios::in);
	if (!trajectoriesFile.is_open())
	{
		cout << "Error: Cannot open input file." << endl;
		trajectoriesFile.close();
		return 0;
	}

	TrajectoryDBAccess<PointType> *db =
			new TrajectoryDBAccessList<PointType> ();

	string s[NB_OF_COLUMNS];

	{
		cout << "Connecting... ";
		bool success = db->connect(database);
		cout << (success ? "OK" : "ERROR") << endl;
		if (!success)
		{
			return 0;
		}
	}

	{
		db->deleteTable();

		cout << "Creating a table... ";
		bool success = db->createTable();
		cout << (success ? "OK" : "ERROR") << endl;
		if (!success)
		{
			return 0;
		}
	}

	uint nTrajectories = 0;
	uint prevId = -1;
	Trajectory<PointType> *trajectory = new Trajectory<PointType> ();

	while (1)
	{
		uint i = 0;
		while (i < NB_OF_COLUMNS && trajectoriesFile >> s[i])
		{
			i++;
		}

		if (i != 0 && i != NB_OF_COLUMNS)
		{
			cout << "READING.. ERROR\n";
			return 0;
		}

		uint _id = 0;
		uint _frame = 0;
		PointCoordinateType _x = 0;
		PointCoordinateType _y = 0;

		if (i == NB_OF_COLUMNS)
		{
			_id = convertString<uint> (s[TRAJECTORY_ID]);
			_frame = convertString<uint> (s[FRAME_NUMBER]);
			_x = convertString<PointCoordinateType> (s[X_COORDINATE]);
			_y = convertString<PointCoordinateType> (s[Y_COORDINATE]);
		}

		if (trajectory->size() > 0 && (i == 0 || (i == NB_OF_COLUMNS && prevId
				!= _id)))
		{
			cout << "Trajectory: writing.. id=" << prevId << " length="
					<< trajectory->size() << ".. ";
			bool success = db->write(*trajectory);
			cout << (success ? "OK" : "ERROR") << endl;
			if (!success)
			{
				return 0;
			}

			nTrajectories++;

			trajectory->clear();
		}

		if (trajectory->size() == 0)
		{
			prevId = _id;
			trajectory->setId(prevId);
		}

		if (i == 0)
		{
			break;
		}

		PointType point(_x, _y);
		trajectory->add(_frame, point);
	}

	cout << nTrajectories << " was saved.\n";

	{
		cout << "Disconnecting... ";
		bool success = db->disconnect();
		cout << (success ? "OK" : "ERROR") << endl;
	}

	trajectoriesFile.close();

	delete db;

	return 0;
}
