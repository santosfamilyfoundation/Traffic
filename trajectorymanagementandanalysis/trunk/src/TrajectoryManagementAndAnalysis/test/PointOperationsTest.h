#ifndef POINTOPERATIONSTEST_H_
#define POINTOPERATIONSTEST_H_

#include "../src/Trajectory.h"

#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class PointOperationsTest: public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(PointOperationsTest);
  CPPUNIT_TEST(testInitPoint);
  CPPUNIT_TEST(testNorm);
  CPPUNIT_TEST(testDim);
  CPPUNIT_TEST(testMinMax);
  CPPUNIT_TEST(testOperatorEqNotEq);
  CPPUNIT_TEST(testOperatorMi1);
  CPPUNIT_TEST(testOperatorPl);
  CPPUNIT_TEST(testOperatorMi2);
  //CPPUNIT_TEST(testOperatorInOut);
  CPPUNIT_TEST_SUITE_END();

protected:
	void testInitPoint(void);
	void testNorm(void);
	void testDim(void);
	void testMinMax(void);
	void testOperatorEqNotEq(void);
	void testOperatorMi1(void);
	void testOperatorPl(void);
	void testOperatorMi2(void);
	void testOperatorInOut(void);

private:
	template<typename Tclass, typename Tparam>
	void testInitPoint2D(const Tclass &correctPoint, const Tparam x, const Tparam y, const Tparam z)
	{
		Tclass point;
		initPoint(point, x, y);
		CPPUNIT_ASSERT_EQUAL(point, correctPoint);

		testInitPoint3D(correctPoint, x, y, z);
	}

	template<typename Tclass, typename Tparam>
	void testInitPoint3D(const Tclass &correctPoint, const Tparam x, const Tparam y, const Tparam z)
	{
		Tclass point;
		initPoint(point, x, y, z);
		CPPUNIT_ASSERT_EQUAL(point, correctPoint);
	}

	template<typename Tclass, typename Tparam>
	void testMinMax(Tparam x1, Tparam y1, Tparam z1, Tparam x2, Tparam y2, Tparam z2)
	{
		Tclass point1, point2, pointMin, pointMax;
		initPoint(point1, x1, y1, z1);
		initPoint(point2, x2, y2, z2);
		initPoint(pointMin, min(x1, x2), min(y1, y2), min(z1, z2));
		initPoint(pointMax, max(x1, x2), max(y1, y2), max(z1, z2));
		CPPUNIT_ASSERT_EQUAL(min(point1, point2), pointMin);
		CPPUNIT_ASSERT_EQUAL(min(point2, point1), pointMin);
		CPPUNIT_ASSERT_EQUAL(max(point1, point2), pointMax);
		CPPUNIT_ASSERT_EQUAL(max(point2, point1), pointMax);
	}

	template<typename Tclass, typename Tparam>
	void testOperatorEqNotEq(const Tparam x1, const Tparam y1, const Tparam z1, const Tparam delta)
	{
		Tclass point1;
		initPoint(point1, x1, y1, z1);

		const Tclass point2 = point1;

		initPoint(point1, x1, y1, z1);
		CPPUNIT_ASSERT(point2 == point1);
		CPPUNIT_ASSERT(!(point2 != point1));

		initPoint(point1, (Tparam) (x1 + delta), y1, z1);
		CPPUNIT_ASSERT(point2 != point1);
		CPPUNIT_ASSERT(!(point2 == point1));

		initPoint(point1, (Tparam) (x1 - delta), y1, z1);
		CPPUNIT_ASSERT(point2 != point1);
		CPPUNIT_ASSERT(!(point2 == point1));

		initPoint(point1, x1, (Tparam) (y1 + delta), z1);
		CPPUNIT_ASSERT(point2 != point1);
		CPPUNIT_ASSERT(!(point2 == point1));

		initPoint(point1, x1, (Tparam) (y1 - delta), z1);
		CPPUNIT_ASSERT(point2 != point1);
		CPPUNIT_ASSERT(!(point2 == point1));

		initPoint(point1, x1, y1, (Tparam) (z1 + delta));
		if (dim(point1) == 2)
		{
			CPPUNIT_ASSERT(point2 == point1);
			CPPUNIT_ASSERT(!(point2 != point1));
		}
		else if (dim(point1) == 3)
		{
			CPPUNIT_ASSERT(point2 != point1);
			CPPUNIT_ASSERT(!(point2 == point1));
		}

		initPoint(point1, x1, y1, (Tparam) (z1 - delta));
		if (dim(point1) == 2)
		{
			CPPUNIT_ASSERT(point2 == point1);
			CPPUNIT_ASSERT(!(point2 != point1));
		}
		else if (dim(point1) == 3)
		{
			CPPUNIT_ASSERT(point2 != point1);
			CPPUNIT_ASSERT(!(point2 == point1));
		}
	}

	template<typename T>
	void testOperatorPl(const T point1, const T point2, const T point3)
	{
		const T point4 = point1 + point2;
		CPPUNIT_ASSERT_EQUAL(point3, point4);

		T point5 = point1;
		point5 += point2;

		CPPUNIT_ASSERT_EQUAL(point3, point5);
	}

	template<typename T>
	void testOperatorMi(const T point3, const T point2, const T point1)
	{
		const T point4 = point1 - point2;
		CPPUNIT_ASSERT_EQUAL(point3, point4);

		T point5 = point1;
		point5 -= point2;

		CPPUNIT_ASSERT_EQUAL(point3, point5);
	}

	// problem: different format using the opencv Point to string with the proposed one for CvPoint
	template<typename T>
	void testOperatorInOut(const T point1, string s)
	{
		stringstream ss;
		ss << point1;
		CPPUNIT_ASSERT_EQUAL(ss.str(), s);

		T point2;
		istringstream is(s);
		is >> point2;
		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}
};

#endif /* POINTOPERATIONSTEST_H_ */
