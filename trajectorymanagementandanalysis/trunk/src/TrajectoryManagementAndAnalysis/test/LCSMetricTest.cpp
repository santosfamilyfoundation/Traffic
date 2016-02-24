#include "LCSMetricTest.h"

void LCSMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new LCSMetric<CvPoint, unsigned int> ();
}

void LCSMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void LCSMetricTest::testMetric1(void)
{
	unsigned int result = 0;
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, (unsigned int)(0));
}

void LCSMetricTest::testMetric2(void)
{
	unsigned int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, (unsigned int)(1));
}

void LCSMetricTest::testMetric3(void)
{
	unsigned int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, (unsigned int)(0));
}

void LCSMetricTest::testMetric4(void)
{
	unsigned int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, (unsigned int)(0));
}

void LCSMetricTest::testMetric5(void)
{
	unsigned int result = 0;
	unsigned n = 50;
	for (unsigned i = 1; i <= n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
		trajectoryB ->add(cvPoint(n - i + 1, n - 1 + 1));
	}
	for (unsigned i = 1; i <= n; ++i)
	{
		trajectoryA ->add(cvPoint(n - i + 1, n - 1 + 1));
		trajectoryB->add(cvPoint(i, i));
	}
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, (unsigned int)(50));
}

void LCSMetricTest::testMetric6(void)
{
	unsigned int result = 0;
	unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
		trajectoryB ->add(cvPoint(i + 3, i + 3));
	}
	metric->similarity(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, n - 3);
}
