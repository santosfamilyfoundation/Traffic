#ifndef HAUSDORFFMETRICTEST_H_
#define HAUSDORFFMETRICTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "../src/HausdorffMetric.h"
using namespace std;

class HausdorffMetricTest: public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE( HausdorffMetricTest);
		CPPUNIT_TEST( testMetric1);
		CPPUNIT_TEST( testMetric2);
		CPPUNIT_TEST( testMetric3);
		CPPUNIT_TEST( testMetric4);
		CPPUNIT_TEST( testMetric5);
		CPPUNIT_TEST( testMetric6);
		CPPUNIT_TEST( testMetric7);
		CPPUNIT_TEST( testMetric8);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
protected:
	void testMetric1(void);
	void testMetric2(void);
	void testMetric3(void);
	void testMetric4(void);
	void testMetric5(void);
	void testMetric6(void);
	void testMetric7(void);
	void testMetric8(void);
private:
	Trajectory<CvPoint> *trajectoryA;
	Trajectory<CvPoint> *trajectoryB;
	Metric<CvPoint, double> *metric;
};

#endif /* HAUSDORFFMETRICTEST_H_ */
