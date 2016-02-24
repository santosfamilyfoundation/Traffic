#ifndef DBSQLITEACCESS_H_
#define DBSQLITEACCESS_H_

#include "opencv2/core/core.hpp"

#include <sqlite3.h>

#include <sstream>
#include <string>
#include <vector>
#include <map>

template<typename T> class TrajectoryElement;

/**
 * DBSQLiteAccess class.
 *
 * The DBSQLiteAccess class allows to perform basic operations on the database like connecting, disconnecting, creating a table, deleting a table or executing other statements.
 */
class DBSQLiteAccess
{
public:
	/**
	 * Constructor.
	 */
	DBSQLiteAccess();

	/**
	 * Destructor.
	 */
	virtual ~DBSQLiteAccess();

	/**
	 * Connect to the database.
	 *
	 * @param[in] database name of the database
	 * @return information whether the operation was successful
	 */
	bool connect(const char *database);

	/**
	 * Inform whether the connection to the database has been established.
	 *
	 * @return information whether the connection to the database has been established
	 */
	bool isConnected();

	/**
	 * Disconnect from the database.
	 *
	 * @return information whether the operation was successful
	 */
	bool disconnect();

	int sqliteErrCode();

	/**
	 * Get error message.
	 *
	 * @return last error message
	 */
	const char *sqliteErrMsg();

	/**
	 * Execute statement.
	 *
	 * @param[in] statement statement
	 * @return information whether the operation was successful
	 */
	bool executeStatement(const char *statement);

	bool executeStatementGetSingleValue(const char *statement, std::string& result);

	template<typename T>
	bool executeStatementGetSingleValue(const char *statement, T& result)
	{
		char **dbResult = 0;
		int nrows, ncols;
		bool success = executeStatement(statement, &dbResult, nrows, ncols);

		if (success && nrows == 1 && ncols == 1 && dbResult[1] != NULL)
		{
		  std::istringstream is(dbResult[1]);
		  is >> result;
		}
		else
		{
			success = false;
		}

		sqlite3_free_table(dbResult);

		return success;
	}

	/**
	 * Execute statement and get results.
	 *
	 * @param[in] statement statement
	 * @return result as a matrix of strings
	 */
	bool executeStatementGetMatrix(const char *statement, std::vector<std::vector<std::string> >& result);

	/**
	 * Execute statement and get results of a select.
	 *
	 * @param[in] statement statement
	 * @return result as a matrix of strings
	 */
	bool executeStatementSelect(std::vector<std::vector<std::string> >& result, const char *statement);

	/** Execute statement and get result of select as a list of integers (eg trajectory ids) */
	bool executeStatementSelectIntegers(std::vector<int>& result, const char *statement);

	/** Execute statement and get result of select as a list of integers (more than one per row, eg trajectory first and last frames) */
	bool executeStatementSelectMultipleIntegers(std::vector<std::vector<int> >& result, const char *statement);

	/**
	 * Execute statement and get results of a select for two integers followed by two floats.
	 *
	 * @param[in] statement statement
	 * @return result as a matrix of strings
	 */
	bool executeStatementSelectTrajectoryElements(std::map<int, std::vector<TrajectoryElement<cv::Point2f> > >& trajectoryElements, const char *statement);

/**
	 * Execute statement and get results of a select for two integers (prototype id and matched trajectory id)
	 *
	 * @param[in] 
	 * @return result as a matrix of strings
	 */
	bool executeStatementSelectPrototypeMatches(std::multimap<int,int>& matches, const char *statement);

	/**
	 * Execute begin transaction command.
	 *
	 * @return information whether the operation was successful
	 */
	bool begin();

	/**
	 * Execute end transaction command.
	 *
	 * @return information whether the operation was successful
	 */
	bool end();

	/**
	 * Execute rollback transaction command.
	 *
	 * @return information whether the operation was successful
	 */
	bool rollback();

	/**
	 * Execute commit transaction command.
	 *
	 * @return information whether the operation was successful
	 */
	bool commit();

private:
	/**
	 * Pointer to SQLite3 class.
	 */
	sqlite3 *db;

	/**
	 * Information whether a connection has been established.
	 */
	bool connected;

	/**
	 * Execute statement and get result.
	 *
	 * @param[in] statement statement
	 * @param[out] result results
	 * @param[out] nrows of rows in the returned data \a result
	 * @param[out] ncols of columns in the returned data \a result
	 * @return information whether the operation was successful
	 */
	bool executeStatement(const char *statement, char ***result, int &nrows, int &ncols);
};

#endif /* DBSQLITEACCESS_H_ */
