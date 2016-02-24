#include "HausdorffMetricTest.h"

void HausdorffMetricTest::setUp(void)
{
	trajectoryA = new Trajectory<CvPoint> ();
	trajectoryB = new Trajectory<CvPoint> ();
	metric = new HausdorffMetric<CvPoint, double> ();
}

void HausdorffMetricTest::tearDown(void)
{
	delete trajectoryA;
	delete trajectoryB;
	delete metric;

	trajectoryA = NULL;
	trajectoryB = NULL;
	metric = NULL;
}

void HausdorffMetricTest::testMetric1(void)
{
	double result = double(0);
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result), TrajectoryLengthErrorException);
}

void HausdorffMetricTest::testMetric2(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	CPPUNIT_ASSERT_THROW(metric->distance(trajectoryA, trajectoryB, result), TrajectoryLengthErrorException);
}

void HausdorffMetricTest::testMetric3(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(0));
}

void HausdorffMetricTest::testMetric4(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(1, 1));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, sqrt(double(2)));
}

void HausdorffMetricTest::testMetric5(void)
{
	double result = double(0);
	trajectoryA->add(cvPoint(0, 0));
	trajectoryB->add(cvPoint(3, 4));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(5));
}

void HausdorffMetricTest::testMetric6(void)
{
	double result = double(0);
	unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		trajectoryA->add(cvPoint(0, i));
		trajectoryB ->add(cvPoint(1, i));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, double(1));
}

void HausdorffMetricTest::testMetric7(void)
{
	double result = double(0);
	unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
		trajectoryB ->add(cvPoint(n + i, n - 2 + i));
	}
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, norm(cv::Point_<int>(n, n - 2)));
}

void HausdorffMetricTest::testMetric8(void)
{
	double result = double(0);
	unsigned n = 100;
	for (unsigned i = 0; i < n; ++i)
	{
		trajectoryA->add(cvPoint(i, i));
	}
	trajectoryB ->add(cvPoint(0, 0));
	metric->distance(trajectoryA, trajectoryB, result);
	CPPUNIT_ASSERT_EQUAL(result, norm(cv::Point_<int>(n - 1, n - 1)));
}
