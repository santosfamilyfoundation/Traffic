#ifndef TRAJECTORYDBACCESSLISTTEST_H_
#define TRAJECTORYDBACCESSLISTTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "TrajectoryDBAccessTest.h"
#include "../src/TrajectoryDBAccessList.h"
using namespace std;

template<typename T>
class TrajectoryDBAccessListTest: public TrajectoryDBAccessTest<T>
{
  CPPUNIT_TEST_SUITE(TrajectoryDBAccessListTest);
  CPPUNIT_TEST(testSize);
  CPPUNIT_TEST(testMinTrajectoryId);
  CPPUNIT_TEST(testMaxTrajectoryId);
  CPPUNIT_TEST(testPrototypeMatchStructure);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp(void)
  {
    TrajectoryDBAccessTest<T>::db = new TrajectoryDBAccessList<T> ();
    TrajectoryDBAccessTest<T>::setUp();
  }
};

#endif /* TRAJECTORYDBACCESSLISTTEST_H_ */
