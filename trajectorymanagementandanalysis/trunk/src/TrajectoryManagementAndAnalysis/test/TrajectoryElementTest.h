#ifndef TRAJECTORYELEMENTTEST_H_
#define TRAJECTORYELEMENTTEST_H_

#include "../src/TrajectoryElement.h"
#include "../src/PointOperations.h"

#include <cppunit/extensions/HelperMacros.h>

using namespace std;

template<typename T>
class TrajectoryElementTest: public CPPUNIT_NS::TestCase
{
CPPUNIT_TEST_SUITE(TrajectoryElementTest);
		CPPUNIT_TEST(testTrajectoryElement1);
		CPPUNIT_TEST(testTrajectoryElement2);
		CPPUNIT_TEST(testSetFrameNumber);
		CPPUNIT_TEST(testSetPoint);
		CPPUNIT_TEST(testShift1);
		CPPUNIT_TEST(testShift2);
		CPPUNIT_TEST(testOperatorEq1);
		CPPUNIT_TEST(testOperatorEq2);
		CPPUNIT_TEST(testOperatorEq3);
		CPPUNIT_TEST(testOperatorEq4);
		CPPUNIT_TEST(testOperatorEq5);
//		CPPUNIT_TEST(testOperatorIn1);
//		CPPUNIT_TEST(testOperatorIn2);
//		CPPUNIT_TEST(testOperatorOut1);
//		CPPUNIT_TEST(testOperatorOut2);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void)
	{
		trajectoryElement.setFrameNumber(2);

		T point;
		initPoint(point, int(4), int(8), int(16));
		trajectoryElement.setPoint(point);
	}

	void tearDown(void)
	{
	}

protected:
	void testTrajectoryElement1(void)
	{
		T point;
		initPoint(point, int(4), int(8), int(16));
		CPPUNIT_ASSERT_EQUAL(trajectoryElement.getFrameNumber(), (unsigned int) 2);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement.getPoint(), point);
	}

	void testTrajectoryElement2(void)
	{
		TrajectoryElement<T> trajectoryElement2(trajectoryElement);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement, trajectoryElement2);
	}

	void testSetFrameNumber(void)
	{
		for (unsigned int i = 0; i < 20; ++i)
		{
			trajectoryElement.setFrameNumber(i);
			CPPUNIT_ASSERT_EQUAL(trajectoryElement.getFrameNumber(), i);
		}
	}

	void testSetPoint(void)
	{
		T point1, point2;
		for (unsigned int i = 0; i < 20; ++i)
		{
			initPoint(point1, int(4), int(8), int(16));
			initPoint(point2, int(4), int(8), int(16));
			trajectoryElement.setPoint(point1);
			CPPUNIT_ASSERT_EQUAL(trajectoryElement.getPoint(), point2);
		}
	}

	void testShift1(void)
	{
		T shiftPoint, endPoint;
		initPoint(shiftPoint, int(32), int(16), int(4));
		initPoint(endPoint, int(36), int(24), int(20));
		trajectoryElement.shift(shiftPoint);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement.getPoint(), endPoint);
	}

	void testShift2(void)
	{
		T shiftPoint1, shiftPoint2, endPoint;
		initPoint(shiftPoint1, int(32), int(16), int(4));
		initPoint(shiftPoint2, int(2), int(4), int(28));
		initPoint(endPoint, int(38), int(28), int(48));
		trajectoryElement.shift(shiftPoint1);
		trajectoryElement.shift(shiftPoint2);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement.getPoint(), endPoint);
	}

	void testOperatorEq1(void)
	{
		CPPUNIT_ASSERT(trajectoryElement == trajectoryElement);

		trajectoryElement.setFrameNumber(2);
		CPPUNIT_ASSERT(trajectoryElement == trajectoryElement);

		T point;
		initPoint(point, int(10), int(20), int(30));
		trajectoryElement.setPoint(point);
		CPPUNIT_ASSERT(trajectoryElement == trajectoryElement);
	}

	void testOperatorEq2(void)
	{
		TrajectoryElement<T> trajectoryElement2(trajectoryElement);
		CPPUNIT_ASSERT(trajectoryElement2 == trajectoryElement);
	}

	void testOperatorEq3(void)
	{
		TrajectoryElement<T> trajectoryElement2;
		trajectoryElement2 = trajectoryElement;
		CPPUNIT_ASSERT(trajectoryElement2 == trajectoryElement);
	}

	void testOperatorEq4(void)
	{
		const TrajectoryElement<T> trajectoryElement2(trajectoryElement);
		trajectoryElement.setFrameNumber(16);
		CPPUNIT_ASSERT(trajectoryElement != trajectoryElement2);
	}

	void testOperatorEq5(void)
	{
		const TrajectoryElement<T> trajectoryElement2(trajectoryElement);

		T point;

		initPoint(point, int(4), int(8), int(16));
		trajectoryElement.setPoint(point);
		CPPUNIT_ASSERT(trajectoryElement == trajectoryElement2);

		initPoint(point, int(4 + 10), int(8), int(16));
		trajectoryElement.setPoint(point);
		CPPUNIT_ASSERT(trajectoryElement != trajectoryElement2);

		initPoint(point, int(4), int(8 + 10), int(16));
		trajectoryElement.setPoint(point);
		CPPUNIT_ASSERT(trajectoryElement != trajectoryElement2);

		initPoint(point, int(4), int(8), int(16 + 10));
		trajectoryElement.setPoint(point);
		if (dim(point) == 2)
		{
			CPPUNIT_ASSERT(trajectoryElement == trajectoryElement2);
		}
		else if (dim(point) == 3)
		{
			CPPUNIT_ASSERT(trajectoryElement != trajectoryElement2);
		}
	}

	void testOperatorIn1(void)
	{
		stringstream ss;
		ss << trajectoryElement;

		string s("2 4 8");
		if (dim(trajectoryElement.getPoint()) == 3)
		{
			s += string(" 16");
		}

		CPPUNIT_ASSERT_EQUAL(ss.str(), s);
	}

	void testOperatorIn2(void)
	{
		trajectoryElement.setFrameNumber(4);

		T point;
		initPoint(point, int(10), int(20), int(30));
		trajectoryElement.setPoint(point);

		stringstream ss;
		ss << trajectoryElement;

		string s("4 10 20");
		if (dim(trajectoryElement.getPoint()) == 3)
		{
			s += string(" 30");
		}

		CPPUNIT_ASSERT_EQUAL(ss.str(), s);
	}

	void testOperatorOut1(void)
	{
		stringstream ss;
		ss << trajectoryElement;

		TrajectoryElement<T> trajectoryElement2;
		istringstream is(ss.str());
		is >> trajectoryElement2;

		CPPUNIT_ASSERT_EQUAL(trajectoryElement, trajectoryElement2);
	}

	void testOperatorOut2(void)
	{
		trajectoryElement.setFrameNumber(4);

		T point;
		initPoint(point, int(10), int(20), int(30));
		trajectoryElement.setPoint(point);

		stringstream ss;
		ss << trajectoryElement;

		TrajectoryElement<T> trajectoryElement2;
		istringstream is(ss.str());
		is >> trajectoryElement2;

		CPPUNIT_ASSERT_EQUAL(trajectoryElement, trajectoryElement2);
	}

private:
	TrajectoryElement<T> trajectoryElement;
};

#endif /* TRAJECTORYELEMENTTEST_H_ */
