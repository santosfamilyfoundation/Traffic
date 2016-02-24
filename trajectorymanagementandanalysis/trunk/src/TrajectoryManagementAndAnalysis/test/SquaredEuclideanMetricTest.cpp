#include "SquaredEuclideanMetricTest.h"

void SquaredEuclideanMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new SquaredEuclideanMetric<CvPoint, double> ();
}

void SquaredEuclideanMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void SquaredEuclideanMetricTest::testMetric1(void)
{
	double result = double(0);
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result),
			TrajectoryLengthErrorException);
}

void SquaredEuclideanMetricTest::testMetric2(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void SquaredEuclideanMetricTest::testMetric3(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(2));
}

void SquaredEuclideanMetricTest::testMetric4(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(25));
}

void SquaredEuclideanMetricTest::testMetric5(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryA->add(cvPoint(-4, -3));
	trajectoryB->add(cvPoint(3, 4));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(50));
}

void SquaredEuclideanMetricTest::testMetric6(void)
{
	double result = double(0);
	unsigned n = 100;
	unsigned maxRand = 10000;
	srand(time(NULL));
	for (unsigned int i = 0; i < n; ++i)
	{
		unsigned int rand1 = rand() % maxRand;
		unsigned int rand2 = rand() % maxRand;
		trajectoryA->add(cvPoint(rand1, rand2));
		trajectoryB ->add(cvPoint(rand1 + 3, rand2 + 4));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(n * 9 + n * 16));
}
