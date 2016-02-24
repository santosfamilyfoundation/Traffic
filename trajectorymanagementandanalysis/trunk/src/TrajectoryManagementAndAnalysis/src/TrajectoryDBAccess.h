#ifndef TRAJECTORYDBACCESS_H_
#define TRAJECTORYDBACCESS_H_

#include "DBSQLiteAccess.h"
#include "Trajectory.h"

#include <iostream>
#include <sstream>
#include <cassert>

/**
 * TrajectoryDBAccess class.
 *
 * The TrajectoryDBAccess is an abstract class with basic operations on trajectories and databases.
 */

template<typename T>
class TrajectoryDBAccess
{
public:
	/**
	 * Constructor.
	 */
	TrajectoryDBAccess()
	{
		db = new DBSQLiteAccess();
	}

	/**
	 * Destructor.
	 */
	virtual ~TrajectoryDBAccess()
	{
		delete db;
	}

	/**
	 * Connect to the database.
	 *
	 * @param[in] database name of the database
	 * @return information whether the operation was successful
	 */
	bool connect(const char *database)
	{
		return db->connect(database);
	}

	/**
	 * Disconnect from the database.
	 *
	 * @return information whether the operation was successful
	 */
	bool disconnect()
	{
		return db->disconnect();
	}

	/**
	 * Create a table.
	 *
	 * @return information whether the operation was successful
	 */
	virtual bool createTable(const std::string& tableName = "trajectories") = 0;

	/**
	 * This function creates a prototype table which contains a map between prototype and trajectory_id 
	 * which matches the prototype
	 */
	bool createPrototypeTable(const std::string& tableName = "prototypes"){
	  if (!db->isConnected()){
	    return false;
	  }
	  
	  std::string statementString = "create table "+tableName+" ( prototype_id INTEGER, trajectory_id_matched INTEGER);";
	  bool success = db->executeStatement(statementString.c_str());
	  return success;
	}

	/** Create a table of objects, ie list of trajectory ids with attributes */
	bool createObjectTable(const std::string& objectTableName = "objects", const std::string& linkTableName = "objects_features") {
	  //id_obj, road user type, size / db lien features-obj id_feature, id_obj
	  
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	    {
	      return false;
	    }
	  
	  std::string statementString = "create table "+objectTableName+" ( object_id INTEGER, road_user_type INTEGER DEFAULT 0, n_objects INTEGER DEFAULT 1, PRIMARY KEY(object_id) );";
	  bool success = TrajectoryDBAccess<T>::db->executeStatement(statementString.c_str());
	  statementString = "create table "+linkTableName+" (object_id INTEGER, trajectory_id INTEGER, PRIMARY KEY(object_id, trajectory_id) );";
	  success = success && TrajectoryDBAccess<T>::db->executeStatement(statementString.c_str());
	  return success;
	}

	/** Create a table of bounding boxes of the objects in image space */
	bool createBoundingBoxTable(const std::string& bbTableName = "bounding_box") {
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	    {
	      return false;
	    }
	  
	  std::string statementString = "CREATE TABLE bounding_boxes (object_id INTEGER, frame_number INTEGER, x_top_left REAL, y_top_left REAL, x_bottom_right REAL, y_bottom_right REAL,  PRIMARY KEY(object_id, frame_number))";
	  bool success = TrajectoryDBAccess<T>::db->executeStatement(statementString.c_str());
	  return success;
	}

	/**
	 * Delete a table.
	 *
	 * @return information whether the operation was successful
	 */
	bool deleteTable(const std::string& tableName = "trajectories")
	{
		if (!db->isConnected())
		{
			return false;
		}

		std::string statementString = "drop table "+tableName+";";
		const char *statement = statementString.c_str();

		bool success = db->executeStatement(statement);
		return success;
	}

	/**
	 * Read trajectories from a database.
	 *
	 * @param[out] trajectories trajectories
	 * @return information whether the operation was successful
	 */
	virtual bool read(std::vector<std::shared_ptr<Trajectory<T> > > &trajectories, const std::string& tableName = "trajectories") = 0;

	/**
	 * Read prototypes from a database and matching trajectory Ids.
	 *
	 * @param[out] multimap of prototype ids and trajectory ids
	 * @retur boolean : if  the operation was successful or not
	 * 
	 */
	bool read(std::multimap<int,int>& matches, const std::string& tableName = "prototypes") { 
	  if (!TrajectoryDBAccess<T>::db->isConnected())
	      return false;

	  std::string statement = "select * from "+tableName+" order by prototype_id, trajectory_id_matched;";
	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectPrototypeMatches(matches, statement.c_str());
	  return success;
	}

	/// Reads trajectory with specific number
	virtual bool read(std::shared_ptr<Trajectory<T> >& trajectory, const int& trajectoryId, const std::string& tableName = "trajectories") = 0;

	bool beginTransaction(void) { return db->begin();}

	bool endTransaction(void) { return db->end();}

	/**
	 * Write the trajectory to the database within a transaction, calling write.
	 *
	 * @param[in] trajectory trajectory
	 * @return information whether the operation was successful
	 */
	bool writeTransaction(const Trajectory<T> &trajectory, const std::string& tableName = "trajectories") {
	  if (!db->isConnected())
	    {
	      return false;
	    }

		bool success = db->begin();
		if (!success)
		{
			return false;
		}

		success = write(trajectory, tableName);
		if (!success)
		{
			return false;
		}

		success = db->end();
		if (!success)
		{
			return false;
		}

		return true;
	}

	/**
	 * Write the trajectory to the database.
	 *
	 * @param[in] trajectory trajectory
	 * @return information whether the operation was successful
	 */
	virtual bool write(const Trajectory<T> &trajectory, const std::string& tableName = "trajectories") = 0;

	/**
	 * Write the prototype and trajectory id matches to the database.
	 *
	 * @param[in] the multimap between prototype_id and trajectory_id 
	 * @return boolean on whether the operation was successful
	 */
	bool write(const std::multimap<int,int> &matches, const std::string& tableName = "prototypes"){
	  std::string stmt;
	  bool success = true;
	  bool failure = false;
	  int p_id;
	  int t_id;

	  std::multimap<int,int>::const_iterator it;
	  for (it = matches.begin(); it != matches.end(); ++it){
	    p_id = it->first;
	    t_id = it->second;
	    stmt = "insert into "+tableName+" (prototype_id, trajectory_id_matched) values ("+TrajectoryDBAccess<T>::toString(p_id)+", "+TrajectoryDBAccess<T>::toString(t_id)+")";
	    success = db->executeStatement(stmt.c_str());
	    if (success == false)
	      failure = true;
	  } // (go) for it
	  return !failure;
	}

	/** Write object */
	bool writeObject(const unsigned int id, const std::vector<unsigned int>& object, const int roadUserType = 0, const int nObjects = 1, const std::string& objectTableName = "objects", const std::string& linkTableName = "objects_features") {
	  std::string sid = TrajectoryDBAccess<T>::toString(id);
	  std::string stmt = "insert into "+objectTableName+" (object_id, road_user_type, n_objects) values ("+sid+", "+TrajectoryDBAccess<T>::toString(roadUserType)+", "+TrajectoryDBAccess<T>::toString(nObjects)+")";
	  bool success = TrajectoryDBAccess<T>::db->executeStatement(stmt.c_str());

	  if (!success)	{
	    std::cout << "rollback" << std::endl;
	    success = TrajectoryDBAccess<T>::db->rollback();
	    if (!success)
	      return false;
	  }

	  for (unsigned int i=0; i<object.size(); ++i) {
	    std::string stmt = "insert into "+linkTableName+" (object_id, trajectory_id) values ("+sid+","+TrajectoryDBAccess<T>::toString(object[i])+")";
	    success = TrajectoryDBAccess<T>::db->executeStatement(stmt.c_str());
	    // check rollback?
	  }
	  return success;
	}

	/**
	 * Get the label of the trajectory.
	 *
	 * @param[in] trajectoryId id of the trajectory
	 * @param[out] resultOut label
	 * @return information whether the operation was successful
	 */
	bool getTrajectoryLabel(unsigned trajectoryId, std::string& resultOut)
	{
		std::string trajectoryIdString = toString(trajectoryId);
		std::string stmtStr = "select label from trajectorytypes where trajectory_id=" + trajectoryIdString + ";";
		resultOut.clear();
		bool success = db->executeStatementGetSingleValue(stmtStr.c_str(), resultOut);
		return success;
	}

	/**
	 * Inform whether the table with trajectories exists or not.
	 *
	 * @param[out] result information whether the table exists or not
	 * @return information whether the operation was successful
	 */
	bool tableExists(bool &result, const std::string& tableName = "trajectories")
	{
		std::string stmtStr = "select count() from sqlite_master where name=\'"+tableName+"\' and type=\'table\'";
		bool success = db->executeStatementGetSingleValue(stmtStr.c_str(), result);
		return success;
	}

	/**
	 * Inform whether the table with labels of trajectories exists or not.
	 *
	 * @param[out] result information whether the table exists or not
	 * @return information whether the operation was successful
	 */
	bool labelTableExists(bool &result)
	{
		std::string stmtStr = "select count() from sqlite_master where name=\'trajectorytypes\' and type=\'table\'";
		bool success = db->executeStatementGetSingleValue(stmtStr.c_str(), result);
		return success;
	}

	/**
	 * Get the number of trajectories in the database.
	 *
	 * @param[out] result number of trajectories in the database
	 * @return information whether the operation was successful
	 */
	bool size(unsigned int &result, const std::string& tableName = "trajectories")
	{
		std::string stmtStr = "select count(distinct trajectory_id) from "+tableName+";";
		const char *stmt = stmtStr.c_str();
		bool success = db->executeStatementGetSingleValue(stmt, result);
		return success;
	}

	/**
	 * Get the minimum id of all trajectories in a database.
	 *
	 * @param[out] result minimum id
	 * @return information whether the operation was successful
	 */
	bool minTrajectoryId(unsigned int &result, const std::string& tableName = "trajectories")
	{
		std::string stmtStr = "select min(trajectory_id) from "+tableName+";";
		const char *stmt = stmtStr.c_str();
		bool success = db->executeStatementGetSingleValue(stmt, result);
		return success;
	}

	/**
	 * Get the maximum id of all trajectories in a database.
	 *
	 * @param[out] result maximum id
	 * @return information whether the operation was successful
	 */
	bool maxTrajectoryId(unsigned int &result, const std::string& tableName = "trajectories")
	{
		std::string stmtStr = "select max(trajectory_id) from "+tableName+";";
		const char *stmt = stmtStr.c_str();
		bool success = db->executeStatementGetSingleValue(stmt, result);
		return success;
	}

	/** Returns the first and last instants of all trajectory elements */
	bool firstLastInstants(unsigned int& firstInstant, unsigned int& lastInstant) {
	  std::string stmtStr = "select min(first_instant), max(last_instant) from trajectory_instants";
	  std::vector<std::vector<int> > result;
	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectMultipleIntegers(result, stmtStr.c_str());
	  if (!result.empty()) {
	    firstInstant = result[0][0];
	    lastInstant = result[0][1];
	  }
	  return success;
	}

	/** Returns the first and last frame of the trajectory with given id */
	bool timeInterval(unsigned int& firstInstant, unsigned int& lastInstant, const int& trajectoryId) {
	  std::stringstream stmtSS;
	  stmtSS << "select first_instant,last_instant from trajectory_instants where trajectory_id=" << trajectoryId;
	  std::vector<std::vector<int> > result;
	  bool success = TrajectoryDBAccess<T>::db->executeStatementSelectMultipleIntegers(result, stmtSS.str().c_str());
	  assert(result.size() == 1);
	  assert(result[0].size() == 2);
	  firstInstant = result[0][0];
	  lastInstant = result[0][1];
	  return success;
	}

protected:
	/**
	 * Pointer to the DBSQLiteAccess class.
	 */
	DBSQLiteAccess *db;

	/**
	 * Convert variable \a x to string.
	 *
	 * @param x input parameter
	 * @return output string
	 */
	template<typename Ts> std::string toString(Ts x)
	{
	  std::stringstream ss;
	  ss << x;
	  return ss.str();
	}
};

#endif /* TRAJECTORYDBACCESS_H_ */
