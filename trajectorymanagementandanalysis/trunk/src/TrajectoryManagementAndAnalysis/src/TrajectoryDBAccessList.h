#ifndef TRAJECTORYDBACCESSLIST_H_
#define TRAJECTORYDBACCESSLIST_H_

#include "TrajectoryDBAccess.h"

#include <boost/foreach.hpp>

/**
 * TrajectoryDBAccessList class.
 *
 * The TrajectoryDBAccessList class allows to perform basic operations on a database representing trajectory as a List.
 */
template<typename T>
class TrajectoryDBAccessList: public TrajectoryDBAccess<T> {
 protected:
  /** Get ids of trajectories starting or ending at frameNum
      (specific to list version of db) */
  bool trajectoryIdStartingEndingAt(std::vector<int>& ids, const int& frameNum, const std::string& firstOrLast) {
    if (!TrajectoryDBAccess<T>::db->isConnected())
      return false;
    
    std::stringstream stmtSS;
    stmtSS << "SELECT trajectory_id from trajectory_instants WHERE "<< firstOrLast <<"_instant=" << frameNum;
    return TrajectoryDBAccess<T>::db->executeStatementSelectIntegers(ids, stmtSS.str().c_str());
  }

 public:
  /** Create single index for a table. */
  bool createIndex(const std::string& tableName, const std::string& columnName, const bool& unique = false) {
    if (!TrajectoryDBAccess<T>::db->isConnected())
      return false;
    
    std::string stmtStr = "CREATE ";
    if (unique)
      stmtStr += "UNIQUE ";
    stmtStr += "INDEX "+tableName+"_"+columnName+"_index ON "+tableName+"("+columnName+")";
    return TrajectoryDBAccess<T>::db->executeStatement(stmtStr.c_str());
  }

	/**
	 * Create a Trajectory table.
	 *
	 * @return information whether the operation was successful
	 */
	bool createTable(const std::string& tableName = "trajectories")
	{
		if (!TrajectoryDBAccess<T>::db->isConnected())
		{
			return false;
		}

		std::string statementString = "create table "+tableName+" ( trajectory_id INTEGER, frame_number INTEGER, x_coordinate REAL, y_coordinate REAL, PRIMARY KEY( trajectory_id, frame_number ) );";
		  //"create table trajectories ( trajectory_id INTEGER, frame_number INTEGER, x_coordinate REAL, y_coordinate REAL, z_coordinate REAL, PRIMARY KEY( trajectory_id, frame_number ) );";
		bool success = TrajectoryDBAccess<T>::db->executeStatement(statementString.c_str());
		return success;
	}

	/** Create view of first and last frame numbers of trajectories.
	 * (specific to list version of db) */
	bool createViewInstants(const std::string& firstOrLast) {
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	    return false;

	  std::string minOrMax = (firstOrLast=="first")?"min":"max";
	  std::string stmtStr = "CREATE VIEW IF NOT EXISTS "+getViewName(firstOrLast)+" AS select trajectory_id, "+minOrMax+"(frame_number) as frame_number from positions group by trajectory_id";
	  bool success = TrajectoryDBAccess<T>::db->executeStatement(stmtStr.c_str());
	  return success;
	}

	/** Create view or temporary table of first and last instants, 
	    as well as length of each trajectory
	    (specific to list version of db) */
	bool createInstants(const std::string& view) {
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	    return false;

	  std::string viewOrTable = (view=="view")?"VIEW":"TEMP TABLE";
	  std::string stmtStr = "CREATE "+viewOrTable+" IF NOT EXISTS trajectory_instants AS select trajectory_id, min(frame_number) as first_instant, max(frame_number) as last_instant, max(frame_number)-min(frame_number)+1 as length from positions group by trajectory_id";
	  // alternative for trajectory length: SELECT count(*) as length FROM trajectories GROUP BY trajectory_id

	  bool success = TrajectoryDBAccess<T>::db->executeStatement(stmtStr.c_str());

	  //success = success && createIndex("trajectory_instants", "first_instant");
	  success = success && createIndex("trajectory_instants", "last_instant");
	  success = success && createIndex("trajectory_instants", "trajectory_id", true);

	  return success;
	}

	/** Get ids of trajectories starting at frameNum
	    (specific to list version of db) */
	bool trajectoryIdStartingAt(std::vector<int>& ids, const int& frameNum) { return trajectoryIdStartingEndingAt(ids, frameNum, "first");}
	/** Get ids of trajectories ending at frameNum
	    (specific to list version of db) */
	bool trajectoryIdEndingAt(std::vector<int>& ids, const int& frameNum) { return trajectoryIdStartingEndingAt(ids, frameNum, "last");}

	/** Get ids of trajectories with one instant between first and last instant */
	bool trajectoryIdInInterval(std::vector<int>& ids, const unsigned int& firstInstant, const unsigned int& lastInstant) {
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	    return false;
	  
	  std::stringstream stmtSS;
	  stmtSS << "SELECT trajectory_id from positions WHERE frame_number BETWEEN " << firstInstant << " and " << lastInstant;
	  return TrajectoryDBAccess<T>::db->executeStatementSelectIntegers(ids, stmtSS.str().c_str());
	}

	/** Returns the maximum trajectory length
	 * \TODO if trajectory_instants does not exist, get it from positions table
	 * \TODO provide a mechanism to handle empty results */
	bool maxTrajectoryLength(unsigned int& length) {
	  std::string stmtStr = "select max(length) from trajectory_instants";
	  std::vector<int> result;
	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectIntegers(result, stmtStr.c_str());
	  if (!result.empty())
	    length = result[0];
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
	bool read(std::vector<std::shared_ptr<Trajectory<T> > > &trajectories, const std::string& tableName = "trajectories")
	{
		if (!TrajectoryDBAccess<T>::db->isConnected())
		{
			return false;
		}

		std::string statement = "select * from "+tableName+" order by trajectory_id, frame_number;";

		std::vector<std::vector<std::string> > result;
		bool success = TrajectoryDBAccess<T>::db->executeStatementGetMatrix(statement.c_str(), result);
		if (success)
		{
		  if (trajectories.empty())
			{
			  trajectories = std::vector<std::shared_ptr<Trajectory<T> > >();
			}

			bool firstId = true;
			unsigned int prev = 0;
			for (unsigned int i = 0; i < result.size(); ++i)
			{
				unsigned int trajectoryId = convertString<unsigned int> (result[i][0]);

				std::string s = "";
				for (unsigned int j = 1; j < result[i].size(); ++j)
				{
					s += result[i][j] + " ";
				}

				std::istringstream is(s);
				TrajectoryElement<T> t;
				is >> t;

				if (firstId || trajectoryId != prev)
				{
					firstId = false;
					trajectories.push_back(std::shared_ptr<Trajectory<T> >(new Trajectory<T> ()));
					trajectories.back()->setId(trajectoryId);
					prev = trajectoryId;
				}

				trajectories.back()->add(t);
			}
		}

		return success;
	}

	/**
	 * Read prototypes from a database and matching trajectory Ids.
	 *
	 * @param[out] multimap of prototype ids and trajectory ids
	 * @retur boolean : if  the operation was successful or not
	 * 
	 */
	bool read(std::multimap<int,int>& matches, const std::string& tableName = "prototypes") { 
	  if (TrajectoryDBAccess<T>::db->isConnected())
	      return false;

	  std::string statement = "select * from "+tableName+" order by trajectory_id, trajectory_id_matched;";

	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectPrototypeMatches(matches, statement.c_str());
	  return success;
	}

	/// Reads trajectory with specific number
	bool read(std::shared_ptr<Trajectory<cv::Point2f> >& trajectory, const int& trajectoryId, const std::string& tableName = "trajectories") {
	  if (!TrajectoryDBAccess<cv::Point2f>::db->isConnected())
	      return false;

	  std::string statement = "select * from "+tableName+" where trajectory_id = " +TrajectoryDBAccess<T>::toString(trajectoryId)+ " order by frame_number;";
	  std::map<int, std::vector<TrajectoryElement<cv::Point2f> > > trajectoryElements;
	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectTrajectoryElements(trajectoryElements, statement.c_str());
	  if (success) {
	    assert(trajectoryElements.count(trajectoryId) == 1);
	    assert(trajectoryElements.size() == 1);
	    trajectory = std::shared_ptr<Trajectory<cv::Point2f> >(new Trajectory<cv::Point2f>(trajectoryId, trajectoryElements[trajectoryId]));
	  }
	  
	  return success;
	}

	/// Reads trajectory with specific number
	bool read(std::vector<std::shared_ptr<Trajectory<cv::Point2f> > >& trajectories, const std::vector<int>& trajectoryIds, const std::string& tableName = "trajectories") {
	  if (!TrajectoryDBAccess<cv::Point2f>::db->isConnected())
	      return false;
	  bool success = false;
	  if (!trajectoryIds.empty()) {
	    std::stringstream trajectoryIdsSS;
	    BOOST_FOREACH(int id, trajectoryIds)
	      trajectoryIdsSS << id << ", ";
	    std::string statement = "select * from "+tableName+" where trajectory_id in (" +trajectoryIdsSS.str()+ ") order by frame_number;";
	    std::map<int, std::vector<TrajectoryElement<cv::Point2f> > > trajectoryElements;
	    success = TrajectoryDBAccess<T>::db->executeStatementSelectTrajectoryElements(trajectoryElements, statement.c_str());
	    if (success) {
	      BOOST_FOREACH(int id, trajectoryIds)
		trajectories.push_back(std::shared_ptr<Trajectory<cv::Point2f> >(new Trajectory<cv::Point2f>(id, trajectoryElements[id])));
	    }
	  }
	  return success;
	}

	/** Write the trajectory to the database */
	bool write(const Trajectory<T> &trajectory, const std::string& tableName = "trajectories")
	{
		std::string sid = TrajectoryDBAccess<T>::toString(trajectory.getId());

		for (unsigned int i = 0; i < trajectory.size(); ++i)
		{
		  //std::string stmt = "insert into trajectories (trajectory_id, frame_number, x_coordinate, y_coordinate, z_coordinate) values (";
			std::string stmt = "insert into "+tableName+" (trajectory_id, frame_number, x_coordinate, y_coordinate) values (";
			stmt += sid + ", ";
			std::stringstream ss;
			T p = trajectory[i];
			ss << trajectory.getFrameNumber(i) << ", " << p.x << ", " << p.y;
			stmt += ss.str();

			/* if (dim(trajectory.getPoint(i)) == 2) //for z_coordinate */
			/* { */
			/* 	stmt += ",0"; */
			/* } */

			stmt += ");";

			bool success = TrajectoryDBAccess<T>::db->executeStatement(stmt.c_str());
			if (!success)
			{
			  std::cout << "rollback" << std::endl;
			  success = TrajectoryDBAccess<T>::db->rollback();
			  if (!success)
			    {
			      return false;
			    }
			}
		}
		return true;
	}

private:
	/**
	 * Convert string to variable with a type of \a Tc.
	 *
	 * @param s input parameter
	 * @return output variable
	 */
	template<typename Tc> Tc convertString(std::string s)
	{
	  std::istringstream is(s);
	  Tc x;
	  is >> x;
	  return x;
	}

	/// Returns the name of the view
	static std::string getViewName(const std::string& firstOrLast) { return "trajectory_"+firstOrLast+"_instants";}
};

#endif /* TRAJECTORYDBACCESSLIST_H_ */
