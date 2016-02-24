#include "ManhattanMetricTest.h"

void ManhattanMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new ManhattanMetric<CvPoint, int> ();
}

void ManhattanMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void ManhattanMetricTest::testMetric1(void)
{
	int result = 0;
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result),
			TrajectoryLengthErrorException);
}

void ManhattanMetricTest::testMetric2(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 0);
}

void ManhattanMetricTest::testMetric3(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 2);
}

void ManhattanMetricTest::testMetric4(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 7);
}

void ManhattanMetricTest::testMetric5(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(3, 4));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 14);
}

void ManhattanMetricTest::testMetric6(void)
{
	int result = 0;
	unsigned n = 100;
	unsigned maxRand = 10000;
	srand(time(NULL));
	for (unsigned i = 0; i < n; ++i)
	{
		unsigned rand1 = rand() % maxRand;
		unsigned rand2 = rand() % maxRand;
		trajectoryA->add(cvPoint(rand1, rand2));
		trajectoryB ->add(cvPoint(rand1 + 3, rand2 + 4));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, int(n * 3 + n * 4));
}
