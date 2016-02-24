#ifndef TRAJECTORIESREADER_H_
#define TRAJECTORIESREADER_H_

/** TODO 
 * LA FONCTION SUIVANTE EST UTILE MAIS PAS PARAMÉTRÉ
 */

bool readAllTrajectories(const char* database,
			 std::vector<TrajectoryPoint*> *trajectories, std::vector<int> &trajectoryGroupId) 
{
	TrajectoryDBAccess<cv::Point_<int> > *db = new TrajectoryDBAccessList<
	  cv::Point_<int> > (); 

	{
		std::cout << "Connecting... ";
		bool success = db->connect(database);
		std::cout << (success ? "OK" : "ERROR") << std::endl;
		if (!success)
		{
			return false;
		}
	}

	{
		std::cout << "Reading trajectories from DB... ";
		bool success = db->read(trajectories);
		std::cout << (success ? "OK" : "ERROR") << " (read " << trajectories->size()
				<< " trajectories)" << std::endl;
	}

	{
		std::cout << "Obtaining group id for each trajectory... " << std::endl;

		std::map<std::string, int> M;
		M.clear();
		trajectoryGroupId.clear();
		for (unsigned i = 0; i < trajectories->size(); ++i)
		{
			unsigned trajectoryId = (*trajectories)[i]->getId();
			std::string trajectoryLabel;
			std::cout << "TrajectoryId=" << trajectoryId;
			db->getTrajectoryLabel(trajectoryId, trajectoryLabel);
			std::cout << "\t\tTrajectoryLabel=" << trajectoryLabel;

			unsigned nb = M.count(trajectoryLabel);

			if (nb == 0)
			{
				M[trajectoryLabel] = M.size() - 1;
			}

			trajectoryGroupId.push_back(M[trajectoryLabel]);
			std::cout << "\t\tGroupId=" << M[trajectoryLabel] << std::endl;
		}
	}

	{
		std::cout << "Disconnecting... ";
		bool success = db->disconnect();
		std::cout << (success ? "OK" : "ERROR") << std::endl;
	}

	delete db;

	return true;
}

#endif /* TRAJECTORIESREADER_H_ */
