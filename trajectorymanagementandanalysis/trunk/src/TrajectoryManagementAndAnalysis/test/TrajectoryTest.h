#ifndef TRAJECTORYTEST_H_
#define TRAJECTORYTEST_H_

#include "../src/Trajectory.h"

#include <cppunit/extensions/HelperMacros.h>

using namespace std;

template<class T>
class TrajectoryTest: public CPPUNIT_NS::TestCase
{
CPPUNIT_TEST_SUITE(TrajectoryTest);
		CPPUNIT_TEST(testTrajectory1);
		CPPUNIT_TEST(testTrajectory2);
		CPPUNIT_TEST(testTrajectory3);
		CPPUNIT_TEST(testSetId);
		CPPUNIT_TEST(testGetTrajectoryElement);
		CPPUNIT_TEST(testAdd1);
		CPPUNIT_TEST(testAdd2);
		CPPUNIT_TEST(testAdd3);
		CPPUNIT_TEST(testAdd4);
		CPPUNIT_TEST(testAdd5);
		CPPUNIT_TEST(testAdd6);
		CPPUNIT_TEST(testAdd7);
		CPPUNIT_TEST(testAdd8);
		CPPUNIT_TEST(testAdd9);
		CPPUNIT_TEST(testAdd10);
		CPPUNIT_TEST(testAdd11);
		CPPUNIT_TEST(testAdd12);
		CPPUNIT_TEST(testAdd13);
		CPPUNIT_TEST(testAdd14);
		CPPUNIT_TEST(testAdd15);
		CPPUNIT_TEST(testAdd16);
		CPPUNIT_TEST(testAdd17);
		CPPUNIT_TEST(testAdd18);
		CPPUNIT_TEST(testAdd19);
		CPPUNIT_TEST(testAdd20);
		CPPUNIT_TEST(testAdd21);
		CPPUNIT_TEST(testAdd22);
		CPPUNIT_TEST(testAdd23);
		CPPUNIT_TEST(testAdd24);
		CPPUNIT_TEST(testAdd25);
		CPPUNIT_TEST(testAdd26);
		CPPUNIT_TEST(testAdd27);
		CPPUNIT_TEST(testAdd28);
		CPPUNIT_TEST(testAdd29);
		CPPUNIT_TEST(testAdd30);
		CPPUNIT_TEST(testAdd31);
		CPPUNIT_TEST(testAdd32);
		CPPUNIT_TEST(testAdd33);
		CPPUNIT_TEST(testAdd34);
		CPPUNIT_TEST(testAdd35);
		CPPUNIT_TEST(testAdd36);
		CPPUNIT_TEST(testAdd37);
		CPPUNIT_TEST(testAdd38);
		CPPUNIT_TEST(testAdd39);
		CPPUNIT_TEST(testGetFrameNumber);
		CPPUNIT_TEST(testGetPoint);
		CPPUNIT_TEST(testGetCheckAscFrameNumber1);
		CPPUNIT_TEST(testGetCheckAscFrameNumber2);
		CPPUNIT_TEST(testSetCheckAscFrameNumber1);
		CPPUNIT_TEST(testSetCheckAscFrameNumber2);
		CPPUNIT_TEST(testSetCheckAscFrameNumber3);
		CPPUNIT_TEST(testSetCheckAscFrameNumber4);
		CPPUNIT_TEST(testSetCheckAscFrameNumber5);
		CPPUNIT_TEST(testSetCheckAscFrameNumber6);
		CPPUNIT_TEST(testSetCheckAscFrameNumber7);
		CPPUNIT_TEST(testSetCheckAscFrameNumber8);
		CPPUNIT_TEST(testSetCheckAscFrameNumber9);
		CPPUNIT_TEST(testSetCheckAscFrameNumber10);
		CPPUNIT_TEST(testSize1);
		CPPUNIT_TEST(testSize2);
		CPPUNIT_TEST(testSizeInc);
		CPPUNIT_TEST(testSizeDec);
		CPPUNIT_TEST(testSizeClear);
		CPPUNIT_TEST(testEmptySizeInc);
		CPPUNIT_TEST(testEmptySizeDec);
		CPPUNIT_TEST(testInsert1);
		CPPUNIT_TEST(testInsert2);
		CPPUNIT_TEST(testInsert3);
		CPPUNIT_TEST(testInsert4);
		CPPUNIT_TEST(testInsert5);
		CPPUNIT_TEST(testInsert6);
		CPPUNIT_TEST(testInsert7);
		CPPUNIT_TEST(testInsert8);
		CPPUNIT_TEST(testInsert9);
		CPPUNIT_TEST(testErase1);
		CPPUNIT_TEST(testErase2);
		CPPUNIT_TEST(testErase3);
		CPPUNIT_TEST(testErase4);
		CPPUNIT_TEST(testErase5);
		CPPUNIT_TEST(testErase6);
		CPPUNIT_TEST(testErase7);
		CPPUNIT_TEST(testErase8);
		CPPUNIT_TEST(testErase9);
		CPPUNIT_TEST(testErase10);
		CPPUNIT_TEST(testErase11);
		CPPUNIT_TEST(testErase12);
		CPPUNIT_TEST(testErase13);
		CPPUNIT_TEST(testErase14);
		CPPUNIT_TEST(testPop_back1);
		CPPUNIT_TEST(testPop_back2);
		CPPUNIT_TEST(testPop_back3);
		CPPUNIT_TEST(testPop_back4);
		CPPUNIT_TEST(testOperatorGet);
		CPPUNIT_TEST(testAt1);
		CPPUNIT_TEST(testAt2);
		CPPUNIT_TEST(testAt3);
		CPPUNIT_TEST(testAt4);
		CPPUNIT_TEST(testAt5);
		CPPUNIT_TEST(testAt6);
		CPPUNIT_TEST(testAt7);
		CPPUNIT_TEST(testShift1);
		CPPUNIT_TEST(testShift2);
		CPPUNIT_TEST(testOperatorEq1);
		CPPUNIT_TEST(testOperatorEq2);
		CPPUNIT_TEST(testOperatorEq3);
		CPPUNIT_TEST(testOperatorEq4);
		CPPUNIT_TEST(testOperatorEq5);
		CPPUNIT_TEST(testOperatorEq6);
		CPPUNIT_TEST(testOperatorEq7);
		CPPUNIT_TEST(testOperatorEq8);
		CPPUNIT_TEST(testOperatorAdd1);
		CPPUNIT_TEST(testOperatorAdd2);
//		CPPUNIT_TEST(testOperatorIn1);
//		CPPUNIT_TEST(testOperatorIn2);
//		CPPUNIT_TEST(testOperatorIn3);
//		CPPUNIT_TEST(testOperatorOut1);
//		CPPUNIT_TEST(testOperatorOut2);
//		CPPUNIT_TEST(testOperatorOut3);
		CPPUNIT_TEST(testMin1);
		CPPUNIT_TEST(testMin2);
		CPPUNIT_TEST(testMin3);
		CPPUNIT_TEST(testMin4);
		CPPUNIT_TEST(testMax1);
		CPPUNIT_TEST(testMax2);
		CPPUNIT_TEST(testMax3);
		CPPUNIT_TEST(testMax4);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void)
	{
		sampleTrajectory.setId(1);

		const unsigned int n = 100;
		for (unsigned int i = 0; i < n; ++i)
		{
			T point;
			initPoint(point, int(i), int(i + 10), int(i + 20));
			sampleTrajectory.add(i + 1, point);
		}
	}

	void tearDown(void)
	{
		sampleTrajectory.clear();
	}

protected:
	void testTrajectory1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.getId(), (unsigned int) 0);
	}

	void testTrajectory2(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.getCheckAscFrameNumber(), false);
	}

	void testTrajectory3(void)
	{
		Trajectory<T> trajectory(sampleTrajectory);
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testSetId(void)
	{
		const unsigned int id = 123;
		sampleTrajectory.setId(id);
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.getId(), id);
	}

	void testGetTrajectoryElement(void)
	{
		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			TrajectoryElement<T> trajectoryElement1 = sampleTrajectory.getTrajectoryElement(i);
			T point;
			initPoint(point, int(i), int(i + 10), int(i + 20));
			TrajectoryElement<T> trajectoryElement2(i + 1, point);
			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}
	}

	void testAdd1(void)
	{
		Trajectory<T> trajectory;
		trajectory.setId(1);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			const T point = sampleTrajectory.getPoint(i);

			trajectory.add(frameNumber, point);

			TrajectoryElement<T> trajectoryElement1 = trajectory.getTrajectoryElement(i);
			TrajectoryElement<T> trajectoryElement2(frameNumber, point);

			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testAdd2(void)
	{
		Trajectory<T> trajectory;
		trajectory.setId(1);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			const T point = sampleTrajectory.getPoint(i);

			//trajectory.add(point);
			trajectory.add(frameNumber, point);

			const TrajectoryElement<T> trajectoryElement1(frameNumber, point);
			const TrajectoryElement<T> trajectoryElement2 = trajectory.getTrajectoryElement(i);

			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testAdd3(void)
	{
		Trajectory<T> trajectory;
		trajectory.setId(1);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			const T point = sampleTrajectory.getPoint(i);

			const TrajectoryElement<T> trajectoryElement1(frameNumber, point);

			trajectory.add(trajectoryElement1);

			const TrajectoryElement<T> trajectoryElement2 = trajectory.getTrajectoryElement(i);

			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testAdd4(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
	}

	void testAdd5(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
	}

	void testAdd6(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
		CPPUNIT_ASSERT_THROW(trajectory.add(0, point), TrajectoryFrameNumberErrorException);
	}

	void testAdd7(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
	}

	void testAdd8(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(1, point));
	}

	void testAdd9(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(0, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(1, point));
	}

	void testAdd10(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
	}

	void testAdd11(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
	}

	void testAdd12(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_THROW(trajectory.add(2, point), TrajectoryFrameNumberErrorException);
	}

	void testAdd13(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
	}

	void testAdd14(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(3, point));
	}

	void testAdd15(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(3, point));
	}

	void testAdd16(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(4, point));
	}

	void testAdd17(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(2, point));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(4, point));
	}

	void testAdd18(void)
	{
		const T point = sampleTrajectory.getPoint(0);

		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		trajectory.add(10, point);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			CPPUNIT_ASSERT_THROW(trajectory.add(10 + i - 1, point), TrajectoryFrameNumberErrorException);
			CPPUNIT_ASSERT_THROW(trajectory.add(10 + i, point), TrajectoryFrameNumberErrorException);
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(10 + i + 1, point));
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) (sampleTrajectory.size() + 1));
	}

	void testAdd19(void)
	{
		const T point = sampleTrajectory.getPoint(0);

		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		trajectory.add(10, point);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(10 + i - 1, point));
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(10 + i, point));
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(10 + i + 1, point));
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) (3 * sampleTrajectory.size() + 1));
	}

	void testAdd20(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(true);
		const unsigned int frameNumber = sampleTrajectory.getFrameNumber(sampleTrajectory.size() - 1);
		const T point = sampleTrajectory.getPoint(sampleTrajectory.size() - 1);

		for (unsigned int i = 0; i <= frameNumber; ++i)
		{
			CPPUNIT_ASSERT_THROW(sampleTrajectory.add(i, point), TrajectoryFrameNumberErrorException);
		}

		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(frameNumber + 1, point));

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 101);
	}

	void testAdd21(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(false);
		const unsigned int frameNumber = sampleTrajectory.getFrameNumber(sampleTrajectory.size() - 1);
		const T point = sampleTrajectory.getPoint(sampleTrajectory.size() - 1);

		for (unsigned int i = 0; i <= frameNumber; ++i)
		{
			CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(i, point));
		}

		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(frameNumber + 1, point));

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 202);
	}

	void testAdd22(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement(0, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd23(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement(0, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd24(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement(0, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
		CPPUNIT_ASSERT_THROW(trajectory.add(trajectoryElement), TrajectoryFrameNumberErrorException);
	}

	void testAdd25(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement(0, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd26(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement1(0, point);
		const TrajectoryElement<T> trajectoryElement2(1, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd27(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement1(0, point);
		const TrajectoryElement<T> trajectoryElement2(1, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd28(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement(2, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd29(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement(2, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd30(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement(2, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
		CPPUNIT_ASSERT_THROW(trajectory.add(trajectoryElement), TrajectoryFrameNumberErrorException);
	}

	void testAdd31(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement(2, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement));
	}

	void testAdd32(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement1(2, point);
		const TrajectoryElement<T> trajectoryElement2(3, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1) );
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd33(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement1(2, point);
		const TrajectoryElement<T> trajectoryElement2(3, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd34(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement1(2, point);
		const TrajectoryElement<T> trajectoryElement2(4, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd35(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement1(2, point);
		const TrajectoryElement<T> trajectoryElement2(4, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
	}

	void testAdd36(void)
	{
		const T point = sampleTrajectory.getPoint(0);

		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(true);
		const TrajectoryElement<T> trajectoryElement1(10, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const TrajectoryElement<T> trajectoryElement2(10 + i - 1, point);
			const TrajectoryElement<T> trajectoryElement3(10 + i, point);
			const TrajectoryElement<T> trajectoryElement4(10 + i + 1, point);
			CPPUNIT_ASSERT_THROW(trajectory.add(trajectoryElement2), TrajectoryFrameNumberErrorException);
			CPPUNIT_ASSERT_THROW(trajectory.add(trajectoryElement3), TrajectoryFrameNumberErrorException);
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement4));
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) (sampleTrajectory.size() + 1));
	}

	void testAdd37(void)
	{
		const T point = sampleTrajectory.getPoint(0);

		Trajectory<T> trajectory;
		trajectory.setCheckAscFrameNumber(false);
		const TrajectoryElement<T> trajectoryElement1(10, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement1));

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const TrajectoryElement<T> trajectoryElement2(10 + i - 1, point);
			const TrajectoryElement<T> trajectoryElement3(10 + i, point);
			const TrajectoryElement<T> trajectoryElement4(10 + i + 1, point);
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement2));
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement3));
			CPPUNIT_ASSERT_NO_THROW(trajectory.add(trajectoryElement4));
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) (3 * sampleTrajectory.size() + 1));
	}

	void testAdd38(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(true);
		const unsigned int frameNumber = sampleTrajectory.getFrameNumber(sampleTrajectory.size() - 1);
		const T point = sampleTrajectory.getPoint(sampleTrajectory.size() - 1);

		for (unsigned int i = 0; i <= frameNumber; ++i)
		{
			const TrajectoryElement<T> trajectoryElement(i, point);
			CPPUNIT_ASSERT_THROW(sampleTrajectory.add(trajectoryElement), TrajectoryFrameNumberErrorException);
		}

		const TrajectoryElement<T> trajectoryElement(frameNumber + 1, point);
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(trajectoryElement));

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 101);
	}

	void testAdd39(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(false);
		const unsigned int frameNumber = sampleTrajectory.getFrameNumber(sampleTrajectory.size() - 1);
		const T point = sampleTrajectory.getPoint(sampleTrajectory.size() - 1);

		for (unsigned int i = 0; i <= frameNumber; ++i)
		{
			const TrajectoryElement<T> trajectoryElement(i, point);
			CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(trajectoryElement));
		}

		const TrajectoryElement<T> trajectoryElement(frameNumber + 1, point);
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.add(trajectoryElement));

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 202);
	}

	void testGetFrameNumber(void)
	{
		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			CPPUNIT_ASSERT_EQUAL(frameNumber, i + 1);
		}
	}

	void testGetPoint(void)
	{
		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			T point1 = sampleTrajectory.getPoint(i);
			T point2;
			initPoint(point2, int(i), int(i + 10), int(i + 20));
			CPPUNIT_ASSERT_EQUAL(point1, point2);
		}
	}

	void testGetCheckAscFrameNumber1(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(false);
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.getCheckAscFrameNumber(), false);
	}

	void testGetCheckAscFrameNumber2(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.getCheckAscFrameNumber(), true);
	}

	void testSetCheckAscFrameNumber1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(true));
	}

	void testSetCheckAscFrameNumber2(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(0, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(true));
	}

	void testSetCheckAscFrameNumber3(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(1, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(true));
	}

	void testSetCheckAscFrameNumber4(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(1, point);
		trajectory.add(2, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(true));
	}

	void testSetCheckAscFrameNumber5(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(1, point);
		trajectory.add(1, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_THROW(trajectory.setCheckAscFrameNumber(true), TrajectoryFrameNumberErrorException);
	}

	void testSetCheckAscFrameNumber6(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(2, point);
		trajectory.add(1, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_THROW(trajectory.setCheckAscFrameNumber(true), TrajectoryFrameNumberErrorException);
	}

	void testSetCheckAscFrameNumber7(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		Trajectory<T> trajectory;
		trajectory.add(1, point);
		trajectory.add(2, point);
		trajectory.add(2, point);
		trajectory.add(3, point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_THROW(trajectory.setCheckAscFrameNumber(true), TrajectoryFrameNumberErrorException);
	}

	void testSetCheckAscFrameNumber8(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.setCheckAscFrameNumber(true));
	}

	void testSetCheckAscFrameNumber9(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		sampleTrajectory.add(100, point);
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_THROW(sampleTrajectory.setCheckAscFrameNumber(true), TrajectoryFrameNumberErrorException);
	}

	void testSetCheckAscFrameNumber10(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));
		sampleTrajectory.add(101, point);
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.setCheckAscFrameNumber(false));
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.setCheckAscFrameNumber(true));
	}

	void testSize1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) 0);
	}

	void testSize2(void)
	{
		Trajectory<T> trajectory;
		trajectory = sampleTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) 100);
	}

	void testSizeInc(void)
	{
		Trajectory<T> trajectory;

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) i);

			T point = (sampleTrajectory)[i];
			trajectory.add(point);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), sampleTrajectory.size());
	}

	void testSizeDec(void)
	{
		unsigned int size = sampleTrajectory.size();

		for (unsigned int i = 0; i < size; ++i)
		{
			CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int)(size - i));

			sampleTrajectory.pop_back();
		}

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 0);
	}

	void testSizeClear(void)
	{
		sampleTrajectory.clear();
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 0);
	}

	void testEmptySizeInc(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.empty(), true);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const TrajectoryElement<T> trajectoryElement = sampleTrajectory.getTrajectoryElement(i);
			trajectory.add(trajectoryElement);
			CPPUNIT_ASSERT_EQUAL(trajectory.empty(), false);
		}
	}

	void testEmptySizeDec(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory.empty(), true);

		trajectory = sampleTrajectory;

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			CPPUNIT_ASSERT_EQUAL(trajectory.empty(), false);
			trajectory.pop_back();
		}

		CPPUNIT_ASSERT_EQUAL(trajectory.empty(), true);
	}

	void testInsert1(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(trajectory.insert(1, 4, point), TrajectoryOutOfRangeErrorException);
	}

	void testInsert2(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_NO_THROW(trajectory.insert(0, 4, point));

		CPPUNIT_ASSERT_EQUAL(trajectory.size(), (unsigned int) 1);

		const TrajectoryElement<T> trajectoryElement1 = trajectory.getTrajectoryElement(0);
		const TrajectoryElement<T> trajectoryElement2(4, point);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
	}

	void testInsert3(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.insert(sampleTrajectory.size(), 4, point));
	}

	void testInsert4(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(sampleTrajectory.insert(sampleTrajectory.size() + 1, 4, point), TrajectoryOutOfRangeErrorException);
	}

	void testInsert5(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.insert(sampleTrajectory.size() - 1, 20, point));

		const TrajectoryElement<T> trajectoryElement1 = sampleTrajectory.getTrajectoryElement(sampleTrajectory.size() - 1);
		const TrajectoryElement<T> trajectoryElement2(20, point);
		CPPUNIT_ASSERT (trajectoryElement1 != trajectoryElement2);
	}

	void testInsert6(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.insert(sampleTrajectory.size(), 20, point));

		const TrajectoryElement<T> trajectoryElement1 = sampleTrajectory.getTrajectoryElement(sampleTrajectory.size());
		const TrajectoryElement<T> trajectoryElement2(20, point);
		CPPUNIT_ASSERT(trajectoryElement1 != trajectoryElement2);
	}

	void testInsert7(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.insert(10, 20, point));

		const TrajectoryElement<T> trajectoryElement1 = sampleTrajectory.getTrajectoryElement(10);
		const TrajectoryElement<T> trajectoryElement2(20, point);
		CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
	}

	void testInsert8(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(true);

		T point;
		initPoint(point, int(10), int(40), int(80));

		CPPUNIT_ASSERT_THROW(sampleTrajectory.insert(sampleTrajectory.size() + 1, 100, point), TrajectoryOutOfRangeErrorException);
		CPPUNIT_ASSERT_THROW(sampleTrajectory.insert(sampleTrajectory.size(), 100, point), TrajectoryFrameNumberErrorException);
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.insert(sampleTrajectory.size(), 101, point));
	}

	void testInsert9(void)
	{
		sampleTrajectory.setCheckAscFrameNumber(true);

		T point;
		initPoint(point, int(10), int(40), int(80));

		for (unsigned int i = 0; i < sampleTrajectory.size() - 1; ++i)
		{
			CPPUNIT_ASSERT_THROW(sampleTrajectory.insert(i, 20, point), TrajectoryFrameNumberErrorException);
			CPPUNIT_ASSERT_THROW(sampleTrajectory.insert(i, 200, point), TrajectoryFrameNumberErrorException);
		}
	}

	void testErase1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(trajectory.erase(0), TrajectoryOutOfRangeErrorException);
	}

	void testErase2(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(trajectory.erase(1), TrajectoryOutOfRangeErrorException);
	}

	void testErase3(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.add(point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.erase(0));

		Trajectory<T> emptyTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory, emptyTrajectory);
	}

	void testErase4(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.add(point);
		CPPUNIT_ASSERT_THROW(trajectory.erase(1), TrajectoryOutOfRangeErrorException);
	}

	void testErase5(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(0));
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 99);
	}

	void testErase6(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(1));
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 99);
	}

	void testErase7(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(0));
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(0));
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 98);
	}

	void testErase8(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(sampleTrajectory.size() - 1));
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(sampleTrajectory.size() - 1));
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 98);
	}

	void testErase9(void)
	{
		CPPUNIT_ASSERT_THROW(sampleTrajectory.erase(sampleTrajectory.size()), TrajectoryOutOfRangeErrorException);
	}

	void testErase10(void)
	{
		CPPUNIT_ASSERT_THROW(sampleTrajectory.erase(sampleTrajectory.size() + 1), TrajectoryOutOfRangeErrorException);
	}

	void testErase11(void)
	{
		for (unsigned int i = 0; i < 100; ++i)
		{
			CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(0));
			CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), 100 - i - 1);
		}

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 0);

		CPPUNIT_ASSERT_THROW(sampleTrajectory.erase(0), TrajectoryOutOfRangeErrorException);
	}

	void testErase12(void)
	{
		for (unsigned int i = 0; i < 100; ++i)
		{
			CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.erase(sampleTrajectory.size() - 1));
		}

		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 0);

		CPPUNIT_ASSERT_THROW(sampleTrajectory.erase(0), TrajectoryOutOfRangeErrorException);
	}

	void testErase13(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory = sampleTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);

		trajectory.insert(20, 20, point);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);

		trajectory.erase(20);
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testErase14(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory = sampleTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);

		trajectory.insert(20, 20, point);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);

		trajectory.erase(10);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);
	}

	void testPop_back1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(trajectory.pop_back(), TrajectoryLengthErrorException);
	}

	void testPop_back2(void)
	{
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int) 100);

		for (int i = 100; i > 0; --i)
		{
			CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.pop_back());
			CPPUNIT_ASSERT_EQUAL(sampleTrajectory.size(), (unsigned int)(i - 1));
		}

		CPPUNIT_ASSERT_THROW(sampleTrajectory.pop_back(), TrajectoryLengthErrorException);
	}

	void testPop_back3(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory = sampleTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);

		trajectory.add(point);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);

		trajectory.pop_back();
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testPop_back4(void)
	{
		T point;
		initPoint(point, int(10), int(40), int(80));

		Trajectory<T> trajectory = sampleTrajectory;
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);

		trajectory.insert(trajectory.size(), 20, point);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);

		trajectory.pop_back();
		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testOperatorGet(void)
	{
		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			T point1 = sampleTrajectory[i];
			T point2;
			initPoint(point2, int(i), int(i + 10), int(i + 20));
			CPPUNIT_ASSERT_EQUAL(point1, point2);
		}
	}

	void testAt1(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(trajectory.at(0), TrajectoryOutOfRangeErrorException);
		CPPUNIT_ASSERT_THROW(trajectory.at(1), TrajectoryOutOfRangeErrorException);
		CPPUNIT_ASSERT_THROW(trajectory.at(2), TrajectoryOutOfRangeErrorException);
	}

	void testAt2(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.add(point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.at(0));
		CPPUNIT_ASSERT_THROW(trajectory.at(1), TrajectoryOutOfRangeErrorException);
		CPPUNIT_ASSERT_THROW(trajectory.at(2), TrajectoryOutOfRangeErrorException);
	}

	void testAt3(void)
	{
		const T point = sampleTrajectory.getPoint(0);
		Trajectory<T> trajectory;
		trajectory.add(point);
		trajectory.add(point);
		CPPUNIT_ASSERT_NO_THROW(trajectory.at(0));
		CPPUNIT_ASSERT_NO_THROW(trajectory.at(1));
		CPPUNIT_ASSERT_THROW(trajectory.at(2), TrajectoryOutOfRangeErrorException);
	}

	void testAt4(void)
	{
		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			T point1;
			CPPUNIT_ASSERT_NO_THROW(point1 = sampleTrajectory.at(i));
			T point2;
			initPoint(point2, int(i), int(i + 10), int(i + 20));
			CPPUNIT_ASSERT_EQUAL(point1, point2);
		}
	}

	void testAt5(void)
	{
		CPPUNIT_ASSERT_NO_THROW(sampleTrajectory.at(sampleTrajectory.size() - 1));
	}

	void testAt6(void)
	{
		CPPUNIT_ASSERT_THROW(sampleTrajectory.at(sampleTrajectory.size()), TrajectoryOutOfRangeErrorException);
	}

	void testAt7(void)
	{
		CPPUNIT_ASSERT_THROW(sampleTrajectory.at(sampleTrajectory.size() + 1), TrajectoryOutOfRangeErrorException);
	}

	void testShift1(void)
	{
		const int shiftX = 8;
		const int shiftY = 4;
		const int shiftZ = 2;

		T shiftPoint;
		initPoint(shiftPoint, shiftX, shiftY, shiftZ);
		sampleTrajectory.shift(shiftPoint);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const T p1 = sampleTrajectory.getPoint(i);
			T p2;
			initPoint(p2, int(i) + shiftX, int(i + 10) + shiftY, int(i + 20) + shiftZ);
			CPPUNIT_ASSERT_EQUAL(p1, p2);
		}
	}

	void testShift2(void)
	{
		const int shiftX1 = 1;
		const int shiftX2 = 10;
		const int shiftY1 = 2;
		const int shiftY2 = 20;
		const int shiftZ1 = 3;
		const int shiftZ2 = 30;

		T shiftPoint1;
		T shiftPoint2;
		T shiftPoint3;
		initPoint(shiftPoint1, shiftX1, shiftY1, shiftZ1);
		initPoint(shiftPoint2, shiftX2, shiftY2, shiftZ2);
		initPoint(shiftPoint3, shiftX1 + shiftX2, shiftY1 + shiftY2, shiftZ1 + shiftZ2);

		Trajectory<T> trajectory = sampleTrajectory;

		trajectory.shift(shiftPoint1);
		trajectory.shift(shiftPoint2);

		sampleTrajectory.shift(shiftPoint3);

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testOperatorEq1(void)
	{
		CPPUNIT_ASSERT(sampleTrajectory == sampleTrajectory);
	}

	void testOperatorEq2(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		CPPUNIT_ASSERT(trajectory == sampleTrajectory);
	}

	void testOperatorEq3(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		trajectory.pop_back();
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);
	}

	void testOperatorEq4(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		trajectory.setId(2);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);
	}

	void testOperatorEq5(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		trajectory.setCheckAscFrameNumber(true);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);
	}

	void testOperatorEq6(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		trajectory.pop_back();
		const TrajectoryElement<T> trajectoryElement = sampleTrajectory.getTrajectoryElement(sampleTrajectory.size() - 1);
		trajectory.add(trajectoryElement);
		CPPUNIT_ASSERT(trajectory == sampleTrajectory);
	}

	void testOperatorEq7(void)
	{
		Trajectory<T> trajectory = sampleTrajectory;
		trajectory.pop_back();
		TrajectoryElement<T> trajectoryElement = sampleTrajectory.getTrajectoryElement(sampleTrajectory.size() - 1);
		trajectoryElement.setFrameNumber(2);
		trajectory.add(trajectoryElement);
		CPPUNIT_ASSERT(trajectory != sampleTrajectory);
	}

	void testOperatorEq8(void)
	{
		const Trajectory<T> trajectory = sampleTrajectory;

		T point;

		initPoint(point, int(99), int(99 + 10), int(99 + 20));
		sampleTrajectory.pop_back();
		sampleTrajectory.add(100, point);
		CPPUNIT_ASSERT(sampleTrajectory == trajectory);

		initPoint(point, int(99 + 1), int(99 + 10), int(99 + 20));
		sampleTrajectory.pop_back();
		sampleTrajectory.add(100, point);
		CPPUNIT_ASSERT(sampleTrajectory != trajectory);

		initPoint(point, int(99), int(99 + 10 + 1), int(99 + 20));
		sampleTrajectory.pop_back();
		sampleTrajectory.add(100, point);
		CPPUNIT_ASSERT(sampleTrajectory != trajectory);

		initPoint(point, int(99), int(99 + 10), int(99 + 20 + 1));
		sampleTrajectory.pop_back();
		sampleTrajectory.add(100, point);
		if (dim(point) == 2)
		{
			CPPUNIT_ASSERT(sampleTrajectory == trajectory);
		}
		else if (dim(point) == 3)
		{
			CPPUNIT_ASSERT(sampleTrajectory != trajectory);
		}
	}

	void testOperatorAdd1(void)
	{
		Trajectory<T> trajectory;
		trajectory.setId(1);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			const T point = sampleTrajectory.getPoint(i);

			trajectory += point;

			const TrajectoryElement<T> trajectoryElement1(frameNumber, point);
			const TrajectoryElement<T> trajectoryElement2 = trajectory.getTrajectoryElement(i);

			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testOperatorAdd2(void)
	{
		Trajectory<T> trajectory;
		trajectory.setId(1);

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			const unsigned int frameNumber = sampleTrajectory.getFrameNumber(i);
			const T point = sampleTrajectory.getPoint(i);

			const TrajectoryElement<T> trajectoryElement1(frameNumber, point);

			trajectory += trajectoryElement1;

			const TrajectoryElement<T> trajectoryElement2 = trajectory.getTrajectoryElement(i);

			CPPUNIT_ASSERT_EQUAL(trajectoryElement1, trajectoryElement2);
		}

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testOperatorIn1(void)
	{
		Trajectory<T> trajectory;
		stringstream ss;
		ss << trajectory;
		CPPUNIT_ASSERT_EQUAL(ss.str(), string("0"));
	}

	void testOperatorIn2(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));

		Trajectory<T> trajectory;
		trajectory.setId(2);
		trajectory.add(4, point);

		string s("2 4 10 20");
		if (dim(point) == 3)
		{
			s += string(" 30");
		}

		stringstream ss;
		ss << trajectory;

		CPPUNIT_ASSERT_EQUAL(ss.str(), s);
	}

	void testOperatorIn3(void)
	{
		sampleTrajectory.setId(20);

		string s("20");

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			stringstream ss;
			ss << string(" ") << i + 1 << string(" ") << i << string(" ") << i + 10;
			if (dim(sampleTrajectory.getPoint(0)) == 3)
			{
				ss << string(" ") << i + 20;
			}
			s += ss.str();
		}

		stringstream ss;
		ss << sampleTrajectory;

		CPPUNIT_ASSERT_EQUAL(ss.str(), s);
	}

	void testOperatorOut1(void)
	{
		Trajectory<T> trajectory1;
		stringstream ss;
		ss << trajectory1;

		Trajectory<T> trajectory2;
		istringstream is(ss.str());
		is >> trajectory2;

		CPPUNIT_ASSERT_EQUAL(trajectory1, trajectory2);
	}

	void testOperatorOut2(void)
	{
		T point;
		initPoint(point, int(10), int(20), int(30));

		Trajectory<T> trajectory1;
		trajectory1.setId(2);
		trajectory1.add(4, point);

		string s("2 4 10 20");
		if (dim(point) == 3)
		{
			s += " 30";
		}

		stringstream ss;
		ss << trajectory1;

		Trajectory<T> trajectory2;
		istringstream is(ss.str());
		is >> trajectory2;

		CPPUNIT_ASSERT_EQUAL(trajectory1, trajectory2);
	}

	void testOperatorOut3(void)
	{
		sampleTrajectory.setId(20);

		string s("20");

		for (unsigned int i = 0; i < sampleTrajectory.size(); ++i)
		{
			stringstream ss;
			ss << " " << i + 1 << " " << i << " " << i + 10;
			if (dim(sampleTrajectory.getPoint(0)) == 3)
			{
				ss << " " << i + 20;
			}
			s += ss.str();
		}

		stringstream ss;
		ss << sampleTrajectory;

		Trajectory<T> trajectory;
		istringstream is(ss.str());
		is >> trajectory;

		CPPUNIT_ASSERT_EQUAL(trajectory, sampleTrajectory);
	}

	void testMin1(void)
	{
		T point1;
		initPoint(point1, 0, 10, 20);
		T point2;
		CPPUNIT_ASSERT_NO_THROW(point2 = min(sampleTrajectory));
		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

	void testMin2(void)
	{
		Trajectory<T> trajectory;
		T point1;
		for (unsigned int i = 0; i < 100; ++i)
		{
			initPoint(point1, i + 4, 104 - i, i % 24 + 1);
			trajectory += point1;
		}

		initPoint(point1, 4, 5, 1);

		T point2;
		CPPUNIT_ASSERT_NO_THROW(point2 = min(trajectory));

		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

	void testMin3(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(min(trajectory), TrajectoryLengthErrorException);
	}

	void testMin4(void)
	{
		Trajectory<T> trajectory;
		T point1, point2;
		initPoint(point1, 8, 4, 2);
		trajectory.add(point1);
		CPPUNIT_ASSERT_NO_THROW(point2 = min(trajectory));
		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

	void testMax1(void)
	{
		T point1;
		initPoint(point1, int(99), int(109), int(119));
		T point2;
		CPPUNIT_ASSERT_NO_THROW(point2 = max(sampleTrajectory));
		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

	void testMax2(void)
	{
		Trajectory<T> trajectory;
		T point1;
		for (unsigned int i = 0; i < 100; ++i)
		{
			initPoint(point1, int(i + 4), int(104 - i), int(i % 24 + 1));
			trajectory += point1;
		}

		initPoint(point1, 103, 104, 24);

		T point2;
		CPPUNIT_ASSERT_NO_THROW(point2 = max(trajectory));

		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

	void testMax3(void)
	{
		Trajectory<T> trajectory;
		CPPUNIT_ASSERT_THROW(max(trajectory), TrajectoryLengthErrorException);
	}

	void testMax4(void)
	{
		Trajectory<T> trajectory;
		T point1, point2;
		initPoint(point1, 8, 4, 2);
		trajectory.add(point1);
		CPPUNIT_ASSERT_NO_THROW(point2 = max(trajectory));
		CPPUNIT_ASSERT_EQUAL(point1, point2);
	}

private:
	Trajectory<T> sampleTrajectory;
};

#endif /* TRAJECTORYTEST_H_ */
