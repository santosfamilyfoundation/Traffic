#include "PointOperationsTest.h"

void PointOperationsTest::testInitPoint(void)
{
	testInitPoint2D(cv::Point_<ushort>(10, 20), 10, 20, 30);
	testInitPoint2D(cv::Point_<short int>(10, 20), 10, 20, 30);
	testInitPoint2D(cv::Point_<unsigned int>(10, 20), 10, 20, 30);
	testInitPoint2D(cv::Point_<int>(10, 20), 10, 20, 30);
	testInitPoint2D(cv::Point_<float>(10.1, 20.1), 10.1, 20.1, 30.1);
	testInitPoint2D(cv::Point_<double>(10.1, 20.1), 10.1, 20.1, 30.1);

	testInitPoint3D(cv::Point3_<ushort>(10, 20, 30), 10, 20, 30);
	testInitPoint3D(cv::Point3_<short int>(10, 20, 30), 10, 20, 30);
	testInitPoint3D(cv::Point3_<unsigned int>(10, 20, 30), 10, 20, 30);
	testInitPoint3D(cv::Point3_<int>(10, 20, 30), 10, 20, 30);
	testInitPoint3D(cv::Point3_<float>(10.1, 20.1, 30.1), 10.1, 20.1, 30.1);
	testInitPoint3D(cv::Point3_<double>(10.1, 20.1, 30.1), 10.1, 20.1, 30.1);

	testInitPoint2D(cvPoint(10, 20), 10, 20, 30);
	testInitPoint2D(cvPoint2D32f(10.1, 20.2), 10.1, 20.2, 30.3);
	testInitPoint3D(cvPoint3D32f(10.1, 20.2, 30.3), 10.1, 20.2, 30.3);
	testInitPoint2D(cvPoint2D64f(10.1, 20.2), 10.1, 20.2, 30.3);
	testInitPoint3D(cvPoint3D64f(10.1, 20.2, 30.3), 10.1, 20.2, 30.3);
}

void PointOperationsTest::testNorm(void)
{
	CPPUNIT_ASSERT_EQUAL(norm(cvPoint(10, 20)), norm(cv::Point_<int>(10, 20)));
	CPPUNIT_ASSERT_EQUAL(norm(cvPoint2D32f(10.1, 20.2)), norm(cv::Point_<float>(10.1, 20.2)));
	CPPUNIT_ASSERT_EQUAL(norm(cvPoint3D32f(10.1, 20.2, 30.3)), norm(cv::Point3_<float>(10.1, 20.2, 30.3)));
	CPPUNIT_ASSERT_EQUAL(norm(cvPoint2D64f(10.1, 20.2)), norm(cv::Point_<double>(10.1, 20.2)));
	CPPUNIT_ASSERT_EQUAL(norm(cvPoint3D64f(10.1, 20.2, 30.3)), norm(cv::Point3_<double>(10.1, 20.2, 30.3)));
}

void PointOperationsTest::testDim(void)
{
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<ushort>()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<short int>()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<unsigned int>()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<int>()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<float>()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point_<double>()), (unsigned int) 2);

	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<ushort>()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<short int>()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<unsigned int>()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<int>()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<float>()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(cv::Point3_<double>()), (unsigned int) 3);

	CPPUNIT_ASSERT_EQUAL(dim(CvPoint()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(CvPoint2D32f()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(CvPoint3D32f()), (unsigned int) 3);
	CPPUNIT_ASSERT_EQUAL(dim(CvPoint2D64f()), (unsigned int) 2);
	CPPUNIT_ASSERT_EQUAL(dim(CvPoint3D64f()), (unsigned int) 3);
}

void PointOperationsTest::testMinMax(void)
{
	testMinMax<cv::Point_<ushort>, ushort> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point_<short int>, short int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point_<unsigned int>, unsigned int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point_<int>, int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point_<float>, float> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
	testMinMax<cv::Point_<double>, double> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);

	testMinMax<cv::Point3_<ushort>, ushort> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point3_<short int>, short int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point3_<unsigned int>, unsigned int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point3_<int>, int> (10, 20, 30, 20, 10, 20);
	testMinMax<cv::Point3_<float>, float> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
	testMinMax<cv::Point3_<double>, double> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);

	testMinMax<CvPoint, int> (10, 20, 30, 20, 10, 20);
	testMinMax<CvPoint2D32f, float> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
	testMinMax<CvPoint3D32f, float> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
	testMinMax<CvPoint2D64f, double> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
	testMinMax<CvPoint3D64f, double> (10.1, 20.2, 30.3, 20.4, 10.5, 20.6);
}

void PointOperationsTest::testOperatorEqNotEq(void)
{
	testOperatorEqNotEq<cv::Point_<ushort>, ushort> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point_<short int>, short int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point_<unsigned int>, unsigned int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point_<int>, int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point_<float>, float> (10.1, 20.2, 30.3, 0.1);
	testOperatorEqNotEq<cv::Point_<double>, double> (10.1, 20.2, 30.3, 0.1);

	testOperatorEqNotEq<cv::Point3_<ushort>, ushort> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point3_<short int>, short int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point3_<unsigned int>, unsigned int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point3_<int>, int> (10, 20, 30, 1);
	testOperatorEqNotEq<cv::Point3_<float>, float> (10.1, 20.2, 30.3, 0.1);
	testOperatorEqNotEq<cv::Point3_<double>, double> (10.1, 20.2, 30.3, 0.1);

	testOperatorEqNotEq<CvPoint, int> (10, 20, 30, 1);
	testOperatorEqNotEq<CvPoint2D32f, float> (10.1, 20.2, 30.3, 0.1);
	testOperatorEqNotEq<CvPoint3D32f, float> (10.1, 20.2, 30.3, 0.1);
	testOperatorEqNotEq<CvPoint2D64f, double> (10.1, 20.2, 30.3, 0.1);
	testOperatorEqNotEq<CvPoint3D64f, double> (10.1, 20.2, 30.3, 0.1);
}

void PointOperationsTest::testOperatorMi1(void)
{
	CPPUNIT_ASSERT_EQUAL(-cvPoint(10, -20), cvPoint(-10.1, 20.2));
	CPPUNIT_ASSERT_EQUAL(-cvPoint2D32f(10.1, -20.2), cvPoint2D32f(-10.1, 20.2));
	CPPUNIT_ASSERT_EQUAL(-cvPoint3D32f(10.1, -20.2, 30.3), cvPoint3D32f(-10.1, 20.2, -30.3));
	CPPUNIT_ASSERT_EQUAL(-cvPoint2D64f(10.1, -20.2), cvPoint2D64f(-10.1, 20.2));
	CPPUNIT_ASSERT_EQUAL(-cvPoint3D64f(10.1, -20.2, 30.3), cvPoint3D64f(-10.1, 20.2, -30.3));
}

void PointOperationsTest::testOperatorPl(void)
{
	testOperatorPl(cvPoint(10, 20), cvPoint(1, 2), cvPoint(11, 22));
	testOperatorPl(cvPoint2D32f(10.1, 20.2), cvPoint2D32f(1.1, 2.2), cvPoint2D32f(11.2, 22.4));
	testOperatorPl(cvPoint3D32f(10.1, 20.2, 30.3), cvPoint3D32f(1.1, 2.2, 3.3), cvPoint3D32f(11.2, 22.4, 33.6));
	testOperatorPl(cvPoint2D64f(10.1, 20.2), cvPoint2D64f(1.1, 2.2), cvPoint2D64f(11.2, 22.4));
	testOperatorPl(cvPoint3D64f(10.1, 20.2, 30.3), cvPoint3D64f(1.1, 2.2, 3.3), cvPoint3D64f(11.2, 22.4, 33.6));
}

void PointOperationsTest::testOperatorMi2(void)
{
	testOperatorMi(cvPoint(10, 20), cvPoint(1, 2), cvPoint(11, 22));
	testOperatorMi(cvPoint2D32f(10.1, 20.2), cvPoint2D32f(1.1, 2.2), cvPoint2D32f(11.2, 22.4));
	testOperatorMi(cvPoint3D32f(10.1, 20.2, 30.3), cvPoint3D32f(1.1, 2.2, 3.3), cvPoint3D32f(11.2, 22.4, 33.6));
	testOperatorMi(cvPoint2D64f(10.1, 20.2), cvPoint2D64f(1.1, 2.2), cvPoint2D64f(11.2, 22.4));
	testOperatorMi(cvPoint3D64f(10.1, 20.2, 30.3), cvPoint3D64f(1.1, 2.2, 3.3), cvPoint3D64f(11.2, 22.4, 33.6));
}

void PointOperationsTest::testOperatorInOut(void)
{
	testOperatorInOut(cv::Point_<ushort>(10, 20), "10 20");
	testOperatorInOut(cv::Point_<short int>(10, 20), "10 20");
	testOperatorInOut(cv::Point_<unsigned int>(10, 20), "10 20");
	testOperatorInOut(cv::Point_<int>(10, 20), "10 20");
	testOperatorInOut(cv::Point_<float>(10.1, 20.2), "10.1 20.2");
	testOperatorInOut(cv::Point_<double>(10.1, 20.2), "10.1 20.2");

	testOperatorInOut(cv::Point3_<ushort>(10, 20, 30), "10 20 30");
	testOperatorInOut(cv::Point3_<short int>(10, 20, 30), "10 20 30");
	testOperatorInOut(cv::Point3_<unsigned int>(10, 20, 30), "10 20 30");
	testOperatorInOut(cv::Point3_<int>(10, 20, 30), "10 20 30");
	testOperatorInOut(cv::Point3_<float>(10.1, 20.2, 30.3), "10.1 20.2 30.3");
	testOperatorInOut(cv::Point3_<double>(10.1, 20.2, 30.3), "10.1 20.2 30.3");

	testOperatorInOut(cvPoint(10, 20), "10 20");
	testOperatorInOut(cvPoint2D32f(10.1, 20.2), "10.1 20.2");
	testOperatorInOut(cvPoint3D32f(10.1, 20.2, 30.3), "10.1 20.2 30.3");
	testOperatorInOut(cvPoint2D64f(10.1, 20.2), "10.1 20.2");
	testOperatorInOut(cvPoint3D64f(10.1, 20.2, 30.3), "10.1 20.2 30.3");
}
