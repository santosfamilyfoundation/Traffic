#include "TrajectoryTest.h"

void TrajectoryTest::setUp(void)
{
	sampleTrajectory = new Trajectory<CvPoint> ();

	const unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		CvPoint point = cvPoint(int(i), int(i + 100));
		sampleTrajectory->add(i + 1, point);
	}
}

void TrajectoryTest::tearDown(void)
{
	delete sampleTrajectory;
	sampleTrajectory = NULL;
}

void TrajectoryTest::testTrajectory(void)
{
	Trajectory<CvPoint> trajectory(*sampleTrajectory);
	CPPUNIT_ASSERT(equal(trajectory, *sampleTrajectory));
}

void TrajectoryTest::testAdd1(void)
{
	Trajectory<CvPoint> trajectory;

	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		CvPoint p1 = sampleTrajectory->getPosition(i);
		trajectory.add(p1);

		const CvPoint p2 = trajectory.getPosition(i);
		CPPUNIT_ASSERT (equal(p1,p2));

		const unsigned frameNumber = trajectory.getFrameNumber(i);
		CPPUNIT_ASSERT_EQUAL(frameNumber, i + 1);
	}

	CPPUNIT_ASSERT(equal(trajectory, *sampleTrajectory));
}

void TrajectoryTest::testAdd2(void)
{
	Trajectory<CvPoint> trajectory;

	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		CvPoint p1 = sampleTrajectory->getPosition(i);
		trajectory.add(i + 1, p1);

		const CvPoint p2 = trajectory.getPosition(i);
		CPPUNIT_ASSERT (equal(p1,p2));

		const unsigned frameNumber = trajectory.getFrameNumber(i);
		CPPUNIT_ASSERT_EQUAL(frameNumber, i + 1);
	}

	CPPUNIT_ASSERT(equal(trajectory, *sampleTrajectory));
}

void TrajectoryTest::testGet(void)
{
	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		CvPoint p1 = sampleTrajectory->getPosition(i);
		CvPoint p2 = cvPoint(int(i), int(i + 100));
		CPPUNIT_ASSERT (equal(p1,p2));
	}
}

void TrajectoryTest::testGetFrameNumber(void)
{
	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		unsigned frameNumber = sampleTrajectory->getFrameNumber(i);
		CPPUNIT_ASSERT_EQUAL(frameNumber, i + 1);
	}
}

void TrajectoryTest::testGetTrajectoryElement(void)
{
	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		TrajectoryElement<CvPoint> e1 = sampleTrajectory->getTrajectoryElement(i);

		CvPoint p1 = e1.getPosition();
		CvPoint p2 = cvPoint(int(i), int(i + 100));
		CPPUNIT_ASSERT (equal(p1,p2));

		unsigned frameNumber = e1.getFrameNumber();
		CPPUNIT_ASSERT_EQUAL(frameNumber, i + 1);
	}
}

void TrajectoryTest::testSetAndGetId(void)
{
	CPPUNIT_ASSERT_EQUAL(sampleTrajectory->getId(), unsigned(0));

	unsigned newTrajectoryId = 10;
	sampleTrajectory->setId(newTrajectoryId);
	CPPUNIT_ASSERT_EQUAL(sampleTrajectory->getId(), newTrajectoryId);
}

void TrajectoryTest::testSizeInc(void)
{
	Trajectory<CvPoint> *t = new Trajectory<CvPoint> ();

	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		CPPUNIT_ASSERT_EQUAL(t->size(), (unsigned int)i);

		CvPoint point = (*sampleTrajectory)[i];
		t->add(point);
	}

	CPPUNIT_ASSERT_EQUAL(t->size(), sampleTrajectory->size());
}

void TrajectoryTest::testSizeDec(void)
{
	unsigned size = sampleTrajectory->size();

	for (unsigned i = 0; i < size; ++i)
	{
		CPPUNIT_ASSERT_EQUAL(sampleTrajectory->size(), (unsigned int)(size - i));

		sampleTrajectory->pop_back();
	}

	CPPUNIT_ASSERT_EQUAL(sampleTrajectory->size(), (unsigned int)0);
}

void TrajectoryTest::testSizeConstSize(void)
{
	CPPUNIT_ASSERT_EQUAL(sampleTrajectory->size(), (unsigned int)100);
}

void TrajectoryTest::testSizeClear(void)
{
	sampleTrajectory->clear();
	CPPUNIT_ASSERT_EQUAL(sampleTrajectory->size(), (unsigned int)0);
}

void TrajectoryTest::testShift(void)
{
	const int shiftX = 1000;
	const int shiftY = 2000;

	const CvPoint shiftPoint = cvPoint(shiftX, shiftY);
	sampleTrajectory->shift(shiftPoint);

	for (unsigned i = 0; i < sampleTrajectory->size(); ++i)
	{
		CvPoint p1 = sampleTrajectory->getPosition(i);
		CvPoint p2 = cvPoint(int(i) + shiftX, int(i + 100) + shiftY);
		CPPUNIT_ASSERT (equal(p1,p2));
	}
}

bool TrajectoryTest::equal(const Trajectory<CvPoint>& t1, const Trajectory<CvPoint>& t2) const
{
	if (t1.getId() != t2.getId())
	{
		CPPUNIT_ASSERT (t1.getId() == t2.getId());
		return false;
	}

	if (t1.size() != t2.size())
	{
		return false;
	}

	for (unsigned i = 0; i < t1.size(); ++i)
	{
		const TrajectoryElement<CvPoint> e1 = t1.getTrajectoryElement(i);
		const TrajectoryElement<CvPoint> e2 = t2.getTrajectoryElement(i);
		if (e1 != e2)
		{
			return false;
		}
	}

	return true;
}

bool TrajectoryTest::equal(const CvPoint& p1, const CvPoint& p2) const
{
	if (p1.x != p2.x)
	{
		return false;
	}

	if (p1.y != p2.y)
	{
		return false;
	}

	return true;
}
