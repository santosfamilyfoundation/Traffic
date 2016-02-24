#ifndef TRAJECTORYDBACCESSTEST_H_
#define TRAJECTORYDBACCESSTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "../src/TrajectoryDBAccess.h"
using namespace std;

template<typename T>
class TrajectoryDBAccessTest: public CPPUNIT_NS::TestCase
{
public:
	virtual void setUp(void)
	{
		dbName = "XXXXXX.sqlite";
		int res = mkstemps((char*) dbName.c_str(), 7);
		CPPUNIT_ASSERT(res != -1);
	}

	void tearDown(void)
	{
		delete db;
		unlink(dbName.c_str());
	}

protected:
	void testSize(void)
	{
		T point;
		initPoint(point, 1, 2, 3);

		CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
		CPPUNIT_ASSERT_EQUAL(true, db->createTable());

		unsigned int size;

		for (unsigned int i = 0; i < 100; ++i)
		{
			CPPUNIT_ASSERT_EQUAL(true, db->size(size));
			CPPUNIT_ASSERT_EQUAL(i, size);

			Trajectory<T> trajectory;
			trajectory.setId(i);
			trajectory.add(i, point);
			CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory));
			trajectory.pop_back();
		}

		CPPUNIT_ASSERT_EQUAL(true, db->size(size));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 100, size);
	}

	void testMinTrajectoryId(void)
	{
		T point;
		initPoint(point, 1, 2, 3);

		Trajectory<T> trajectory[4];
		for (unsigned int i = 0; i < 4; ++i)
		{
			trajectory[i].setId(i + 1);
 			trajectory[i].add(i, point);
		}

		CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
		CPPUNIT_ASSERT_EQUAL(true, db->createTable());

		unsigned int id;

		CPPUNIT_ASSERT_EQUAL(false, db->minTrajectoryId(id));

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[1]));
		CPPUNIT_ASSERT_EQUAL(true, db->minTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 2, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[2]));
		CPPUNIT_ASSERT_EQUAL(true, db->minTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 2, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[0]));
		CPPUNIT_ASSERT_EQUAL(true, db->minTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 1, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[3]));
		CPPUNIT_ASSERT_EQUAL(true, db->minTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 1, id);
	}

	void testMaxTrajectoryId(void)
	{
		T point;
		initPoint(point, 1, 2, 3);

		Trajectory<T> trajectory[4];
		for (unsigned int i = 0; i < 4; ++i)
		{
			trajectory[i].setId(i + 1);
			trajectory[i].add(i, point);
		}

		CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
		CPPUNIT_ASSERT_EQUAL(true, db->createTable());

		unsigned int id;

		CPPUNIT_ASSERT_EQUAL(false, db->maxTrajectoryId(id));

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[1]));
		CPPUNIT_ASSERT_EQUAL(true, db->maxTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 2, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[2]));
		CPPUNIT_ASSERT_EQUAL(true, db->maxTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 3, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[0]));
		CPPUNIT_ASSERT_EQUAL(true, db->maxTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 3, id);

		CPPUNIT_ASSERT_EQUAL(true, db->write(trajectory[3]));
		CPPUNIT_ASSERT_EQUAL(true, db->maxTrajectoryId(id));
		CPPUNIT_ASSERT_EQUAL((unsigned int) 4, id);
	}

	void testPrototypeMatchStructure(void){

	  CPPUNIT_ASSERT_EQUAL(true, db->connect(dbName.c_str()));
	  CPPUNIT_ASSERT_EQUAL(true, db->createPrototypeTable());
	  multimap<int,int> matches;
	  for (int i = 0 ; i < 5 ; i++)
	    matches.insert(pair<int,int>(i,i));

	  CPPUNIT_ASSERT_EQUAL(true, db->write(matches));
	  
	  multimap<int,int> rematches;
	  CPPUNIT_ASSERT_EQUAL(true, db->read(rematches));
	  multimap<int,int>::iterator it_rematches;
	  multimap<int,int>::iterator it_matches (matches.begin());
	  for (it_rematches = rematches.begin() ; it_rematches != rematches.end() ; it_rematches++){
	    CPPUNIT_ASSERT_EQUAL(it_rematches->first, it_matches->first);
	    CPPUNIT_ASSERT_EQUAL(it_rematches->second,it_matches->second);
	    advance(it_matches, 1);
	  } // for iterator
	  				      	  
	} // testPrototypeMatchStructure

	TrajectoryDBAccess<T> *db;

	string dbName;
};

#endif /* TRAJECTORYDBACCESSTEST_H_ */
