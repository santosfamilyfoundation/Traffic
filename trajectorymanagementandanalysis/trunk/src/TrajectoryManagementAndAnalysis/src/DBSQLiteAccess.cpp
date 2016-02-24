#include "DBSQLiteAccess.h"
#include "TrajectoryElement.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace cv;

DBSQLiteAccess::DBSQLiteAccess()
{
	db = 0;
	connected = false;
}

DBSQLiteAccess::~DBSQLiteAccess()
{
	if (connected)
	{
		disconnect();
	}
}

bool DBSQLiteAccess::connect(const char *database)
{
	if (connected)
	{
		return false;
	}

	int success = sqlite3_open(database, &db);
	connected = (success == SQLITE_OK);
	if (!connected)
	{
		sqlite3_close(db);
	} else {
	  connected = connected && executeStatement("PRAGMA synchronous = OFF") && executeStatement("PRAGMA journal_mode = MEMORY");
	}
	return connected;
}

bool DBSQLiteAccess::isConnected()
{
	return connected;
}

bool DBSQLiteAccess::disconnect()
{
	if (connected)
	{
		int success = sqlite3_close(db);
		if (success == SQLITE_OK)
		{
			db = 0;
			connected = false;
		}
	}
	return connected == false;
}

int DBSQLiteAccess::sqliteErrCode()
{
	return sqlite3_errcode(db);
}

const char *DBSQLiteAccess::sqliteErrMsg()
{
	return sqlite3_errmsg(db);
}

bool DBSQLiteAccess::executeStatement(const char *statement)
{
	if (!connected)
	{
		return false;
	}

	char *errorMsg = 0;
	int errorCode = sqlite3_exec(db, statement, 0, 0, &errorMsg);
	return errorCode == SQLITE_OK;
}

bool DBSQLiteAccess::executeStatementGetSingleValue(const char *statement, string& result)
{
	char **dbResult = 0;
	int nrows, ncols;
	bool success = executeStatement(statement, &dbResult, nrows, ncols);

	if (success && nrows == 1 && ncols == 1 && dbResult[1] != NULL)
	{
		result = *dbResult[1];
	}
	else
	{
		success = false;
	}

	sqlite3_free_table(dbResult);

	return success;
}

bool DBSQLiteAccess::executeStatementGetMatrix(const char *statement, vector<vector<string> >& result)
{
	char **dbResult = 0;
	int nrows, ncols;
	bool success = executeStatement(statement, &dbResult, nrows, ncols);

	result.resize(nrows);
	if (success)
	{
		for (int row = 0; row < nrows; ++row)
		{
		  result[row].resize(ncols);
			for (int col = 0; col < ncols; ++col)
			{
				if (dbResult[ncols + row * ncols + col] != NULL)
				{
					result[row][col] = dbResult[ncols + row * ncols + col];
				}
				else
				  {
				    result[row][col] = string();
				  }
			}
		}
	}

	sqlite3_free_table(dbResult);

	return success;
}

bool DBSQLiteAccess::executeStatementSelect(vector<vector<string> >& result, const char *statement) {
  result.clear();
  sqlite3_stmt *preparedStatement;
  if (sqlite3_prepare_v2(db, statement, -1, &preparedStatement, 0) == SQLITE_OK) {
    int nCols = sqlite3_column_count(preparedStatement);

    int dbResult = sqlite3_step(preparedStatement);
    while (dbResult == SQLITE_ROW) {
      vector<string> row(nCols);
      for(int col = 0; col < nCols; col++) {
	row[col] = (char*)(sqlite3_column_text(preparedStatement, col));
      }
      result.push_back(row);

      dbResult = sqlite3_step(preparedStatement);
    }
  }

  return (sqlite3_finalize(preparedStatement) == SQLITE_OK);
}

bool DBSQLiteAccess::executeStatementSelectIntegers(std::vector<int>& result, const char *statement) {
  result.clear();
  sqlite3_stmt *preparedStatement;
  if (sqlite3_prepare_v2(db, statement, -1, &preparedStatement, 0) == SQLITE_OK) {
    assert(sqlite3_column_count(preparedStatement) == 1);
    int dbResult = sqlite3_step(preparedStatement);
    while (dbResult == SQLITE_ROW) {
      result.push_back(sqlite3_column_int(preparedStatement, 0));
      dbResult = sqlite3_step(preparedStatement);
    }
  }
  return (sqlite3_finalize(preparedStatement) == SQLITE_OK);
}

bool DBSQLiteAccess::executeStatementSelectMultipleIntegers(std::vector<std::vector<int> >& result, const char *statement) {
  result.clear();
  sqlite3_stmt *preparedStatement;
  if (sqlite3_prepare_v2(db, statement, -1, &preparedStatement, 0) == SQLITE_OK) {
    int nColumns = sqlite3_column_count(preparedStatement);
    vector<int> tmp(nColumns);
    int dbResult = sqlite3_step(preparedStatement);
    while (dbResult == SQLITE_ROW) {
      for (int i=0; i<nColumns; ++i)
	tmp[i]=sqlite3_column_int(preparedStatement, i);
      result.push_back(tmp);
      dbResult = sqlite3_step(preparedStatement);
    }
  }
  return (sqlite3_finalize(preparedStatement) == SQLITE_OK);
}

bool DBSQLiteAccess::executeStatementSelectTrajectoryElements(map<int, vector<TrajectoryElement<Point2f> > >& trajectoryElements, const char *statement) {
  trajectoryElements.clear();
  sqlite3_stmt *preparedStatement;
  if (sqlite3_prepare_v2(db, statement, -1, &preparedStatement, 0) == SQLITE_OK) {
    assert(sqlite3_column_count(preparedStatement) == 4);

    int dbResult = sqlite3_step(preparedStatement);
    while (dbResult == SQLITE_ROW) {
      int id =  sqlite3_column_int(preparedStatement, 0);
      int frameNumber = sqlite3_column_int(preparedStatement, 1);
      float x =  static_cast<float>(sqlite3_column_double(preparedStatement, 2));
      float y =  static_cast<float>(sqlite3_column_double(preparedStatement, 3));
      trajectoryElements[id].push_back(TrajectoryElement<Point2f>(frameNumber, Point2f(x, y)));

      dbResult = sqlite3_step(preparedStatement);
    }
  }

  return (sqlite3_finalize(preparedStatement) == SQLITE_OK);
}

bool DBSQLiteAccess::executeStatementSelectPrototypeMatches(multimap<int,int>& matches, const char *statement) {
  sqlite3_stmt *preparedStatement;
  if (sqlite3_prepare_v2(db, statement, -1, &preparedStatement, 0) == SQLITE_OK) {
    assert(sqlite3_column_count(preparedStatement) == 2); // à vérifier

    int dbResult = sqlite3_step(preparedStatement);
    while (dbResult == SQLITE_ROW) {
      int prototype_id =  sqlite3_column_int(preparedStatement, 0);
      int trajectory_id = sqlite3_column_int(preparedStatement, 1);
      matches.insert(pair<int,int>(prototype_id,trajectory_id));

      dbResult = sqlite3_step(preparedStatement);
    }
  }

  return (sqlite3_finalize(preparedStatement) == SQLITE_OK);
}


bool DBSQLiteAccess::begin()
{
	const char *stmt = "begin transaction";
	return executeStatement(stmt);
}

bool DBSQLiteAccess::end()
{
	const char *stmt = "end transaction";
	return executeStatement(stmt);
}

bool DBSQLiteAccess::rollback()
{
	const char *stmt = "rollback;";
	return executeStatement(stmt);
}

bool DBSQLiteAccess::commit()
{
	const char *stmt = "commit;";
	return executeStatement(stmt);
}

bool DBSQLiteAccess::executeStatement(const char *statement, char ***result, int &nrows, int &ncols)
{
	if (!connected)
	{
		return false;
	}

	char *errorMsg = NULL;
	int errorCode = sqlite3_get_table(db, statement, result, &nrows, &ncols, &errorMsg);
	return errorCode == SQLITE_OK;
}
