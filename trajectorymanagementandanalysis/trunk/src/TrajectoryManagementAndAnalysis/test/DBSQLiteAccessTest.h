#ifndef DBSQLITEACCESSTEST_H_
#define DBSQLITEACCESSTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <stdlib.h>
#include "../src/DBSQLiteAccess.h"
using namespace std;

class DBSQLiteAccessTest: public CPPUNIT_NS::TestCase
{
CPPUNIT_TEST_SUITE(DBSQLiteAccessTest);
		CPPUNIT_TEST(testConnect1);
		CPPUNIT_TEST(testConnect2);
		CPPUNIT_TEST(testConnect3);
		CPPUNIT_TEST(testConnect4);
		CPPUNIT_TEST(testConnect5);
		CPPUNIT_TEST(testDisconnect1);
		CPPUNIT_TEST(testDisconnect2);
		CPPUNIT_TEST(testDisconnect3);
		CPPUNIT_TEST(testSqliteErrCodeMsg1);
		CPPUNIT_TEST(testSqliteErrCodeMsg2);
		CPPUNIT_TEST(testSqliteErrCodeMsg3);
		CPPUNIT_TEST(testSqliteErrCodeMsg4);
		CPPUNIT_TEST(testExecuteStatement1);
		CPPUNIT_TEST(testExecuteStatement2);
		CPPUNIT_TEST(testExecuteStatementGetMatrix);
		CPPUNIT_TEST(testTransaction1);
		CPPUNIT_TEST(testTransaction2);
		CPPUNIT_TEST(testTransaction3);
		CPPUNIT_TEST(testTransaction4);
		CPPUNIT_TEST(testTransaction5);
		CPPUNIT_TEST(testTransaction6);
		CPPUNIT_TEST(testTransaction7);
		CPPUNIT_TEST(testTransaction8);
		CPPUNIT_TEST(testTransaction9);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);

protected:
	void testConnect1(void);
	void testConnect2(void);
	void testConnect3(void);
	void testConnect4(void);
	void testConnect5(void);
	void testDisconnect1(void);
	void testDisconnect2(void);
	void testDisconnect3(void);
	void testSqliteErrCodeMsg1(void);
	void testSqliteErrCodeMsg2(void);
	void testSqliteErrCodeMsg3(void);
	void testSqliteErrCodeMsg4(void);
	void testExecuteStatement1(void);
	void testExecuteStatement2(void);
	void testExecuteStatementGetMatrix(void);
	void testTransaction1(void);
	void testTransaction2(void);
	void testTransaction3(void);
	void testTransaction4(void);
	void testTransaction5(void);
	void testTransaction6(void);
	void testTransaction7(void);
	void testTransaction8(void);
	void testTransaction9(void);

private:
	DBSQLiteAccess *db;
	string dbName;
};

#endif /* DBSQLITEACCESSTEST_H_ */
