#include "DBSQLiteAccessTest.h"

void DBSQLiteAccessTest::setUp(void)
{
	db = new DBSQLiteAccess();

	dbName = "XXXXXX.sqlite";
	int res = mkstemps((char*) dbName.c_str(), 7);
	CPPUNIT_ASSERT(res != -1);
}

void DBSQLiteAccessTest::tearDown(void)
{
	delete db;
	remove(dbName.c_str());
}

void DBSQLiteAccessTest::testConnect1(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
}

void DBSQLiteAccessTest::testConnect2(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);

	string dbName2 = "XXXXXX.sqlite";
	int res = mkstemps((char*) dbName2.c_str(), 7);
	CPPUNIT_ASSERT(res != -1);

	CPPUNIT_ASSERT_EQUAL(db->connect(dbName2.c_str()), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);

	remove(dbName2.c_str());
}

void DBSQLiteAccessTest::testConnect3(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect("TestRunner/"), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), false);
	CPPUNIT_ASSERT_EQUAL(db->connect("TestRunner/"), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), false);
}

void DBSQLiteAccessTest::testConnect4(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
	CPPUNIT_ASSERT_EQUAL(db->connect("TestRunner/"), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
}

void DBSQLiteAccessTest::testConnect5(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect("TestRunner/"), false);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), false);
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
}

void DBSQLiteAccessTest::testDisconnect1(void)
{
	CPPUNIT_ASSERT_EQUAL(false, db->isConnected());
	CPPUNIT_ASSERT_EQUAL(true, db->disconnect());
	CPPUNIT_ASSERT_EQUAL(false, db->isConnected());
}

void DBSQLiteAccessTest::testDisconnect2(void)
{
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
	CPPUNIT_ASSERT_EQUAL(db->disconnect(), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), false);
	CPPUNIT_ASSERT_EQUAL(db->connect(dbName.c_str()), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), true);
	CPPUNIT_ASSERT_EQUAL(db->disconnect(), true);
	CPPUNIT_ASSERT_EQUAL(db->isConnected(), false);
}

void DBSQLiteAccessTest::testDisconnect3(void)
{
	string dbName2 = "XXXXXX.sqlite";
	int res = mkstemps((char*) dbName2.c_str(), 7);
	CPPUNIT_ASSERT(res != -1);

	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->isConnected());

	CPPUNIT_ASSERT_EQUAL(false, db->connect(dbName2.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->isConnected());

	CPPUNIT_ASSERT_EQUAL(true, db->disconnect());
	CPPUNIT_ASSERT_EQUAL(false, db->isConnected());

	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName2.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->isConnected());
	CPPUNIT_ASSERT_EQUAL(true, db->disconnect());
	CPPUNIT_ASSERT_EQUAL(false, db->isConnected());

	remove(dbName2.c_str());
}

void DBSQLiteAccessTest::testSqliteErrCodeMsg1(void)
{
	CPPUNIT_ASSERT_EQUAL(false, db->connect("TestRunner/"));
	CPPUNIT_ASSERT_EQUAL(SQLITE_MISUSE, db->sqliteErrCode());
	CPPUNIT_ASSERT_EQUAL(*"library routine called out of sequence", *db->sqliteErrMsg());
}

void DBSQLiteAccessTest::testSqliteErrCodeMsg2(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(false, db->executeStatement("create table A ();"));
	CPPUNIT_ASSERT_EQUAL(SQLITE_ERROR, db->sqliteErrCode());
	CPPUNIT_ASSERT_EQUAL(*"near \")\": syntax error", *db->sqliteErrMsg());
}

void DBSQLiteAccessTest::testSqliteErrCodeMsg3(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( A INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(false, db->executeStatement("create table A ( A INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(SQLITE_ERROR, db->sqliteErrCode());
	CPPUNIT_ASSERT_EQUAL(*"table A already exists", *db->sqliteErrMsg());
}

void DBSQLiteAccessTest::testSqliteErrCodeMsg4(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(false, db->executeStatement("select * from A;"));
	CPPUNIT_ASSERT_EQUAL(SQLITE_ERROR, db->sqliteErrCode());
	CPPUNIT_ASSERT_EQUAL(*"no such table: A", *db->sqliteErrMsg());
}

void DBSQLiteAccessTest::testExecuteStatement1(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));

	int size;

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( a INTEGER, b INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(0, size);

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(1, size);

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(2, size);
}

void DBSQLiteAccessTest::testExecuteStatement2(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));

	string size;

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( a INTEGER, b INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(*"0", *size.c_str());

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(*"1", *size.c_str());

	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(*"2", *size.c_str());
}

void DBSQLiteAccessTest::testExecuteStatementGetMatrix(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( a INTEGER, b INTEGER, c INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2, 3);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(4, 5, 6);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(7, 8, 9);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(10, 11, 12);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(13, 14, 15);"));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(16, 17, 18);"));

	const string correctResult[6][3] =
	{
	{ "1", "2", "3" },
	{ "4", "5", "6" },
	{ "7", "8", "9" },
	{ "10", "11", "12" },
	{ "13", "14", "15" },
	{ "16", "17", "18" } };

	vector<vector<string> > returnedData;
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetMatrix("select * from A;", returnedData));

	for (unsigned int row = 0; row < 6; ++row)
	{
		for (unsigned int col = 0; col < 3; ++col)
		{
			CPPUNIT_ASSERT_EQUAL(correctResult[row][col], returnedData[row][col]);
		}
	}
}

void DBSQLiteAccessTest::testTransaction1(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(false, db->begin());
	CPPUNIT_ASSERT_EQUAL(false, db->begin());
}

void DBSQLiteAccessTest::testTransaction2(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(false, db->rollback());
	CPPUNIT_ASSERT_EQUAL(false, db->rollback());
}

void DBSQLiteAccessTest::testTransaction3(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->rollback());
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->rollback());
}

void DBSQLiteAccessTest::testTransaction4(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->commit());
	CPPUNIT_ASSERT_EQUAL(false, db->rollback());
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->commit());
	CPPUNIT_ASSERT_EQUAL(false, db->rollback());
}

void DBSQLiteAccessTest::testTransaction5(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(false, db->commit());
	CPPUNIT_ASSERT_EQUAL(false, db->commit());
}

void DBSQLiteAccessTest::testTransaction6(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->commit());
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->commit());
}

void DBSQLiteAccessTest::testTransaction7(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->rollback());
	CPPUNIT_ASSERT_EQUAL(false, db->commit());
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->rollback());
	CPPUNIT_ASSERT_EQUAL(false, db->commit());
}

void DBSQLiteAccessTest::testTransaction8(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( a INTEGER, b INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	int size;
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(1, size);
	CPPUNIT_ASSERT_EQUAL(true, db->rollback());
	CPPUNIT_ASSERT_EQUAL(false, db->commit());
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(0, size);
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
}

void DBSQLiteAccessTest::testTransaction9(void)
{
	CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("create table A ( a INTEGER, b INTEGER );"));
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatement("insert into A values(1, 2);"));
	int size;
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(1, size);
	CPPUNIT_ASSERT_EQUAL(true, db->commit());
	CPPUNIT_ASSERT_EQUAL(false, db->rollback());
	CPPUNIT_ASSERT_EQUAL(true, db->executeStatementGetSingleValue("select count(*) from A;", size));
	CPPUNIT_ASSERT_EQUAL(1, size);
	CPPUNIT_ASSERT_EQUAL(true, db->begin());
}
