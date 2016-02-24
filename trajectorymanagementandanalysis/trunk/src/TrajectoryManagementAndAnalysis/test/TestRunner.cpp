#include "TrajectoryTest.h"
#include "TrajectoryElementTest.h"
#include "PointOperationsTest.h"
#include "DBSQLiteAccessTest.h"
#include "TrajectoryDBAccessBlobTest.h"
#include "TrajectoryDBAccessListTest.h"
#include "EuclideanMetricTest.h"
#include "HausdorffMetricTest.h"
#include "HuMetricTest.h"
#include "ChebyshevMetricTest.h"
#include "MinimumMetricTest.h"
#include "SquaredEuclideanMetricTest.h"
#include "ManhattanMetricTest.h"
#include "CanberraMetricTest.h"
#include "LCSMetricTest.h"
#include "DTWMetricTest.h"

#include <cppunit/ui/text/TestRunner.h>

#include "opencv2/core/core.hpp"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<ushort> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<short int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<unsigned int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<float> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point_<double> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<ushort> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<short int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<unsigned int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<float> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<cv::Point3_<double> >);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<CvPoint>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<CvPoint2D32f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<CvPoint3D32f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<CvPoint2D64f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryElementTest<CvPoint3D64f>);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<ushort> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<short int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<unsigned int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<float> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point_<double> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<ushort> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<short int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<unsigned int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<int> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<float> >);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<cv::Point3_<double> >);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<CvPoint>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<CvPoint2D32f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<CvPoint3D32f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<CvPoint2D64f>);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryTest<CvPoint3D64f>);
CPPUNIT_TEST_SUITE_REGISTRATION(PointOperationsTest);
CPPUNIT_TEST_SUITE_REGISTRATION(DBSQLiteAccessTest);
// CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryDBAccessBlobTest<CvPoint>);
CPPUNIT_TEST_SUITE_REGISTRATION(TrajectoryDBAccessListTest<cv::Point3_<float> > );
CPPUNIT_TEST_SUITE_REGISTRATION(EuclideanMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(HausdorffMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(HuMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(ChebyshevMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(MinimumMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SquaredEuclideanMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(ManhattanMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CanberraMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(LCSMetricTest);
CPPUNIT_TEST_SUITE_REGISTRATION(DTWMetricTest);

int main(int ac, char **av)
{
	CPPUNIT_NS::TextUi::TestRunner runner;
	CPPUNIT_NS::TestFactoryRegistry &registry = CPPUNIT_NS::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	bool wasSuccessful = runner.run("", false);
	return wasSuccessful;
}
