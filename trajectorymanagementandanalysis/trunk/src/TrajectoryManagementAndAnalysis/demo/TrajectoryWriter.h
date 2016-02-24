#ifndef DEMOTRAJECTORYWRITER_H_
#define DEMOTRAJECTORYWRITER_H_

#include "../src/TrajectoryDBAccessList.h"

void writeTrajectoriesInDB(const char *database, const std::vector<TrajectoryPoint> *trajectories)
{
	TrajectoryDBAccess<cv::Point_<int> > *db = new TrajectoryDBAccessList<cv::Point_<int> > ();

	{
		std::cout << "Connecting... ";
		bool success = db->connect(database);
		std::cout << (success ? "OK" : "ERROR") << std::endl;
	}

	{
		std::cout << "Deleting a table... ";
		bool success = db->deleteTable();
		std::cout << (success ? "OK" : "ERROR");

		if (!success)
		{
			bool exists;
			bool success = db->tableExists(exists);
			if (success && !exists)
			{
				std::cout << " (reason - table does not exists)";
			}
		}
		std::cout << std::endl;
	}

	{
		std::cout << "Creating a table... ";
		bool success = db->createTable();
		std::cout << (success ? "OK" : "ERROR") << std::endl;
	}

	{
		for (unsigned i = 0; i < trajectories->size(); ++i)
		{
			std::cout << "Writing trajectory " << (i + 1) << " of " << trajectories->size() << " into DB... ";
			bool success = db->write((*trajectories)[i]);
			std::cout << (success ? "OK" : "ERROR") << std::endl;
		}
	}

	{
		std::cout << "Disconnecting... ";
		bool success = db->disconnect();
		std::cout << (success ? "OK" : "ERROR") << std::endl;
	}

	delete db;
}

#endif /* DEMOTRAJECTORYWRITER_H_ */
