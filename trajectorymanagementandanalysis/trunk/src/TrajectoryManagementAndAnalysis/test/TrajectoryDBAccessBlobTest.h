#ifndef TRAJECTORYDBACCESSBLOBTEST_H_
#define TRAJECTORYDBACCESSBLOBTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "TrajectoryDBAccessTest.h"
#include "../src/TrajectoryDBAccessBlob.h"
using namespace std;

template<typename T>
class TrajectoryDBAccessBlobTest: public TrajectoryDBAccessTest<T>
{
CPPUNIT_TEST_SUITE(TrajectoryDBAccessBlobTest);
		CPPUNIT_TEST(testSize);
		CPPUNIT_TEST(testMinTrajectoryId);
		CPPUNIT_TEST(testMaxTrajectoryId);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void)
	{
		TrajectoryDBAccessTest<T>::db = new TrajectoryDBAccessBlob<T> ();
		TrajectoryDBAccessTest<T>::setUp();
	}
};

#endif /* TRAJECTORYDBACCESSBLOBTEST_H_ */
