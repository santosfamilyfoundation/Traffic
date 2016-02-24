#include "HuMetricTest.h"

void HuMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new HuMetric<CvPoint,double> ();
}

void HuMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void HuMetricTest::testMetric1(void)
{
	double result = double(0);
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result), TrajectoryLengthErrorException);
}

void HuMetricTest::testMetric2(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void HuMetricTest::testMetric3(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, sqrt(double(2)) / double(2));
}

void HuMetricTest::testMetric4(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(5)/double(2));
}

void HuMetricTest::testMetric5(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(3, 4));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, sqrt(double(50))/double(4));
}

void HuMetricTest::testMetric6(void)
{
	double result = double(0);
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
	CPPUNIT_ASSERT_EQUAL(result, sqrt(double(n * 9 + n * 16))/double(n * 2));
}
