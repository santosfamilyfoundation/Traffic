#ifndef TRAJECTORYDBACCESSBLOB_H_
#define TRAJECTORYDBACCESSBLOB_H_

#include "TrajectoryDBAccess.h"

/**
 * TrajectoryDBAccessBlob class.
 *
 * The TrajectoryDBAccessBlob class allows to perform basic operations on a database representing trajectory as a Blob.
 */
template<typename T>
class TrajectoryDBAccessBlob: public TrajectoryDBAccess<T>
{
public:
	/**
	 * Create a table.
	 *
	 * @return information whether the operation was successful
	 */
	bool createTable(const std::string& tableName = "trajectories")
	{
		if (!TrajectoryDBAccess<T>::db->isConnected())
		{
			return false;
		}

		std::string statementString = "create table "+tableName+" ( trajectory_id INTEGER, trajectory BLOB );";
		const char *statement = statementString.c_str();

		bool success = TrajectoryDBAccess<T>::db->executeStatement(statement);
		return success;
	}

	/**
	 * Read trajectories from a database.
	 *
	 * @param[out] trajectories trajectories
	 * @param[in] limit maximum number of trajectories, which will be read
	 * @param[in] offset offset
	 * @return information whether the operation was successful
	 */
	bool read(std::vector<Trajectory<T>*>* &trajectories, int limit = -1, int offset = -1, const std::string& tableName = "trajectories")
	{
		if (!TrajectoryDBAccess<T>::db->isConnected())
		{
			return false;
		}

		std::string statementString = "select * from "+tableName;
		if (limit > 0)
		{
			statementString += " limit " + TrajectoryDBAccess<T>::toString(limit);
		}
		if (offset > 0)
		{
			statementString += " offset " + TrajectoryDBAccess<T>::toString(offset);
		}
		const char *statement = statementString.c_str();

		std::vector<std::vector<std::string> > result;
		bool success = TrajectoryDBAccess<T>::db->executeStatementGetMatrix(statement, result);
		if (success)
		{
			if (trajectories == 0)
			{
				trajectories = new std::vector<Trajectory<T>*> ();
			}

			for (unsigned int i = 0; i < result.size(); ++i)
			{
				std::string s = "";
				for (unsigned int j = 0; j < result[i].size(); ++j)
				{
					s += result[i][j];

					if (j == 0)
					{
						s += " ";
					}
				}

				std::istringstream is(s);
				Trajectory<T> *trajectory = new Trajectory<T> ();
				is >> *trajectory;
				trajectories->push_back(trajectory);
			}
		}

		return success;
	}

	/**
	 * Write the trajectory to the database.
	 *
	 * @param[in] trajectory trajectory
	 * @return information whether the operation was successful
	 */
	bool write(const Trajectory<T> &trajectory, const std::string& tableName = "trajectories")
	{
	  std::stringstream ss;
		ss << trajectory;
		std::string s = ss.str();

		unsigned int sIdLen = TrajectoryDBAccess<T>::toString(trajectory.getId()).length();
		s[sIdLen] = ',';
		s.insert(sIdLen, "\"");
		s.insert(sIdLen + 2, "\"");

		std::string statementString = "insert into "+tableName+" (trajectory_id, trajectory) values (\"" + s + "\")";
		const char *statement = statementString.c_str();

		bool success = TrajectoryDBAccess<T>::db->executeStatement(statement);
		return success;
	}
};

#endif /* TRAJECTORYDBACCESSBLOB_H_ */
