#include "MinimumMetricTest.h"

void MinimumMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new MinimumMetric<CvPoint, int> ();
}

void MinimumMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void MinimumMetricTest::testMetric1(void)
{
	int result = 0;
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result), TrajectoryLengthErrorException);
}

void MinimumMetricTest::testMetric2(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 0);
}

void MinimumMetricTest::testMetric3(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 1);
}

void MinimumMetricTest::testMetric4(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 3);
}

void MinimumMetricTest::testMetric5(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(6, 8));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 3);
}

void MinimumMetricTest::testMetric6(void)
{
	int result = 0;
	unsigned n = 100;
	for (unsigned i = 1; i <= n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
		trajectoryB ->add(cvPoint(i * 2 + 10, i * 3 + 10));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 11);
}
