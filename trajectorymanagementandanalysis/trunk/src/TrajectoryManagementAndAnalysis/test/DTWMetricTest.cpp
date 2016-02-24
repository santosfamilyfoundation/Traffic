#include "DTWMetricTest.h"

void DTWMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new DTWMetric<CvPoint, double> ();
}

void DTWMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void DTWMetricTest::testMetric1(void)
{
	double result = double(0);
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void DTWMetricTest::testMetric2(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void DTWMetricTest::testMetric3(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, sqrt(double(2)));
}

void DTWMetricTest::testMetric4(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(5));
}

void DTWMetricTest::testMetric5(void)
{
	double result = double(0);
	unsigned n = 100;
	for (unsigned i = 1; i <= n; ++i)
	{
		trajectoryA->add(cvPoint(i * 100, i * 100));
		trajectoryB ->add(cvPoint(i * 100 + 3, i * 100 + 4));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(n * 5));
}

void DTWMetricTest::testMetric6(void)
{
	double result = double(0);
	unsigned n = 100;
	for (unsigned i = 1; i <= n; ++i)
	{
		trajectoryA->add(cvPoint(i * 100, i * 100));
		trajectoryB ->add(cvPoint(i * 100 + 6, i * 100 + 8));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(n * sqrt(double(100))));
}
