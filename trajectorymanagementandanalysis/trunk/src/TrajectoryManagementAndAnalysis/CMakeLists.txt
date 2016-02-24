CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )


#PROJECT( TrajectoryManagementAndAnalysis )


FIND_PACKAGE(
	OpenCV REQUIRED
)

FIND_LIBRARY(
	SQLite3_LIBS sqlite3
)

FIND_LIBRARY(CppUnit_LIBS cppunit
	/usr/local/lib
	/usr/lib
)


SET(
	CMAKE_CXX_FLAGS "-g -Wall -DNDEBUG -fpermissive -std=c++11"
)


ADD_EXECUTABLE(
	TrajectoryManagementAndAnalysisDemo
	demo/TrajectoryManagementAndAnalysisDemo.cpp
	src/DBSQLiteAccess.cpp
)

TARGET_LINK_LIBRARIES(
	TrajectoryManagementAndAnalysisDemo
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	MetricsEvaluationDemo
	demo/MetricsEvaluationDemo.cpp
	src/DBSQLiteAccess.cpp
)

TARGET_LINK_LIBRARIES(
	MetricsEvaluationDemo
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	ReadAndSaveInDBSample
	samples/ReadAndSaveInDBSample.cpp
	src/DBSQLiteAccess.cpp
)

TARGET_LINK_LIBRARIES(
	ReadAndSaveInDBSample
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	SaveTrajectoriesSample
	samples/SaveTrajectoriesSample
)

TARGET_LINK_LIBRARIES(
	SaveTrajectoriesSample
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	TestRunner
	test/TestRunner.cpp
	#test/TrajectoryTest.cpp
	test/PointOperationsTest.cpp
	src/DBSQLiteAccess.cpp
	test/DBSQLiteAccessTest.cpp
	test/EuclideanMetricTest.cpp
	test/HausdorffMetricTest.cpp
	test/HuMetricTest.cpp
	test/ChebyshevMetricTest.cpp
	test/MinimumMetricTest.cpp
	test/SquaredEuclideanMetricTest.cpp
	test/ManhattanMetricTest.cpp
	test/CanberraMetricTest.cpp
	test/LCSMetricTest.cpp
	test/DTWMetricTest.cpp
)

TARGET_LINK_LIBRARIES(
	TestRunner
	${CppUnit_LIBS}
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	EfficiencyOfDataStorageDemo
	demo/EfficiencyOfDataStorageDemo.cpp
	src/DBSQLiteAccess.cpp
)

TARGET_LINK_LIBRARIES(
	EfficiencyOfDataStorageDemo
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_EXECUTABLE(
	ClusteringAndClassificationDemo
	demo/ClusteringAndClassificationDemo.cpp
	src/DBSQLiteAccess.cpp
)

TARGET_LINK_LIBRARIES(
	ClusteringAndClassificationDemo
	${OpenCV_LIBS}
	${SQLite3_LIBS}
)


ADD_LIBRARY(
	TrajectoryManagementAndAnalysis
	src/DBSQLiteAccess.cpp
)

install(TARGETS
	TrajectoryManagementAndAnalysisDemo
	MetricsEvaluationDemo
	ReadAndSaveInDBSample
	SaveTrajectoriesSample
	TestRunner
	EfficiencyOfDataStorageDemo
	ClusteringAndClassificationDemo
	DESTINATION bin)

file(GLOB headers src/*.h)
install(FILES ${headers} DESTINATION include)
install(TARGETS TrajectoryManagementAndAnalysis DESTINATION lib)
