#include "ChebyshevMetricTest.h"

void ChebyshevMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new ChebyshevMetric<CvPoint, int> ();
}

void ChebyshevMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void ChebyshevMetricTest::testMetric1(void)
{
	int result = 0;
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result), TrajectoryLengthErrorException);
}

void ChebyshevMetricTest::testMetric2(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 0);
}

void ChebyshevMetricTest::testMetric3(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 1);
}

void ChebyshevMetricTest::testMetric4(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 4);
}

void ChebyshevMetricTest::testMetric5(void)
{
	int result = 0;
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(3, 4));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, 4);
}

void ChebyshevMetricTest::testMetric6(void)
{
	int result = 0;
	unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
		trajectoryB ->add(cvPoint(i * 2, i * 3));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, int((n - 1) * 2));
}
