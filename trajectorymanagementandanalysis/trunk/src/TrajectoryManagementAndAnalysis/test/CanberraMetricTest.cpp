#include "CanberraMetricTest.h"

void CanberraMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new CanberraMetric<CvPoint> ();
}

void CanberraMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void CanberraMetricTest::testMetric1(void)
{
	double result = double(0);
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result),
			TrajectoryLengthErrorException);
}

void CanberraMetricTest::testMetric2(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void CanberraMetricTest::testMetric3(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(2));
}

void CanberraMetricTest::testMetric4(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(2));
}

void CanberraMetricTest::testMetric5(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(3, 4));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(4));
}

void CanberraMetricTest::testMetric6(void)
{
	double result = double(0);
	unsigned n = 100;
	unsigned maxRand = 10000;
	srand(time(NULL));
	double correctResult = double(0);
	for (unsigned i = 0; i < n; ++i)
	{
		unsigned rand1 = rand() % maxRand;
		unsigned rand2 = rand() % maxRand;
		trajectoryA->add(cvPoint(rand1, rand2));
		trajectoryB->add(cvPoint(rand1 + 3, rand2 + 4));
		correctResult += double(3) / double(rand1 * 2 + 3) + double(4) / double(rand2 * 2 + 4);
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result, correctResult, 1e-10);
}
