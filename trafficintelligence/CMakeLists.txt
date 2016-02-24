CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

FIND_PACKAGE(
	OpenCV REQUIRED
)

FIND_LIBRARY(
	SQLite3_LIBS sqlite3
)

#FIND_PACKAGE(TrajectoryManagement)

SET(
	CMAKE_CXX_FLAGS "-g -Wall"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(feature-based-tracking
	c/cvutils.cpp
	c/feature-based-tracking.cpp
	c/Motion.cpp
	c/Parameters.cpp
	c/utils.cpp
	c/InputFrameListModule.cpp
	c/InputVideoFileModule.cpp
	)

find_package(Boost REQUIRED program_options filesystem system)
find_library(TrajectoryManagement_LIBRARY TrajectoryManagementAndAnalysis)
find_path(TrajectoryManagement_INCLUDE_DIR src/Trajectory.h)

add_definitions(
	-DUSE_OPENCV
	)

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${TrajectoryManagement_INCLUDE_DIR}
	)

target_link_libraries(feature-based-tracking
	${TrajectoryManagement_LIBRARY}
	${SQLite3_LIBS}
	${OpenCV_LIBS}
	${Boost_LIBRARIES}
	)

install(TARGETS feature-based-tracking DESTINATION bin)
