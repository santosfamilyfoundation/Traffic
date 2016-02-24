EXE_DIR=../bin
SCRIPTS_DIR=../scripts
TRAJECTORYMANAGEMENT_DIR=$(HOME)/Traffic/trajectorymanagementandanalysis/trunk/src/TrajectoryManagementAndAnalysis

CXX = g++

INCLUDE = -I../include -I$(TRAJECTORYMANAGEMENT_DIR)

LIBS = -L../../klt -L$(TRAJECTORYMANAGEMENT_DIR)

#LDFLAGS = -Wl,-Bstatic -lm
LDFLAGS = -lm
LDFLAGS += -lTrajectoryManagementAndAnalysis -lsqlite3
LDFLAGS += -lboost_program_options
#LDFLAGS += -lfltk

CFLAGS = -Wall -W -Wextra -std=c++11
UNAME = $(shell uname)

ifneq ($(OPENCV), 0)
	CFLAGS += -DUSE_OPENCV
	LDFLAGS += -lopencv_highgui -lopencv_core -lopencv_video -lopencv_ml -lopencv_features2d -lopencv_imgproc -lopencv_objdetect
endif

#LDFLAGS += -Wl,--as-needed -Wl,-Bdynamic,-lgcc_s,-Bstatic

ifeq ($(UNAME), Linux)
	#OPENCV_HOME=$(HOME)/Research/Code/opencv-2.4.10/release
	OPENCV_HOME=/usr/local
	INCLUDE+= -I$(OPENCV_HOME)/include -I$(OPENCV_HOME)/include/opencv
	LIBS += -L$(OPENCV_HOME)/lib
	#LIBS += -L/usr/lib/gcc/x86_64-linux-gnu/4.8/
	LINUX_BOOST_PREFIX = /usr/local
	CFLAGS += -DLINUX
	EXE_EXTENSION=''
else # windows
	MINGW_HOME = 'C:\MinGW'
	LDFLAGS += -mconsole -mwindows -lole32 -lwsock32 -luuid
	EXE_EXTENSION='.exe'
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -g -gstabs+ -DDEBUG
else
	CFLAGS += -O3 --fast-math
	CFLAGS += -DNDEBUG
endif

ifeq ($(PROFILING), 1)
	CFLAGS += -pg
endif

CXXFLAGS = $(INCLUDE) $(CFLAGS)

#GUI_OBJS = 
CV_OBJS = cvutils.o
COMMON_OBJS = utils.o Motion.o Parameters.o utils.o
OBJS = $(COMMON_OBJS) $(CV_OBJS)
TESTS_OBJS = test_feature.o test_graph.o


default: builddir all

all: test-pixels optical-flow track-features

builddir:
	@$(SCRIPTS_DIR)/createdirectory.sh $(EXE_DIR)

test:
	echo "coucou $(HOME)"

tests: $(TESTS_OBJS) $(OBJS)
	@$(SCRIPTS_DIR)/createdirectory.sh $(EXE_DIR)
	@$(SCRIPTS_DIR)/createdirectory.sh test_data
	$(CXX) $(CFLAGS) $(LIBS) -o $(EXE_DIR)/$@ $^ $(LDFLAGS)
	$(EXE_DIR)/$@

feature-based-tracking: feature-based-tracking.o $(OBJS)
	@$(SCRIPTS_DIR)/createdirectory.sh $(EXE_DIR)
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $(EXE_DIR)/$@ $(LDFLAGS)

track-features.o: track-features.cpp
	$(CXX) -I../../klt $(INCLUDE) $^ -c

track-features: track-features.o $(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) -L../../klt $^ -o $(EXE_DIR)/$@ $(LDFLAGS) -lklt

optical-flow: optical-flow.o $(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $(EXE_DIR)/$@ $(LDFLAGS)

test-pixels: test-pixels.o $(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $(EXE_DIR)/$@ $(LDFLAGS)

clean:
	rm -f *.gch */*.o *.o *.a  $(EXE_DIR)/*
# $(DISTFILE_OUTPUT)
