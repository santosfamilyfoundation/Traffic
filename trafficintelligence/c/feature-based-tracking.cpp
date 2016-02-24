#include "Motion.hpp"
#include "Parameters.hpp"
#include "cvutils.hpp"
#include "utils.hpp"

#include "src/Trajectory.h"
#include "src/TrajectoryDBAccessList.h"
#include "src/TrajectoryDBAccessBlob.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <boost/foreach.hpp>

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <memory>
#include <limits>

using namespace std;
using namespace cv;

void drawMatchesRelative(const vector<KeyPoint>& train, const vector<KeyPoint>& query, std::vector<cv::DMatch>& matches, Mat& img) {
  for (int i = 0; i < (int)matches.size(); i++)
    {
      Point2f pt_new = query[matches[i].queryIdx].pt;
      Point2f pt_old = train[matches[i].trainIdx].pt;
      Point2f dist = pt_new - pt_old;
      if (norm(dist) < 20) {
	line(img, pt_new, pt_old, Scalar(125, 255, 125), 1);
	circle(img, pt_old, 2, Scalar(255, 0, 125), 1);
      }
    }
}

void drawOpticalFlow(const vector<Point2f>& prevPts, const vector<Point2f>& currPts, const vector<uchar> status, Mat& img) {
  for (unsigned int i=0; i<status.size(); i++) {
    if (status[i]) {
 	line(img, prevPts[i], currPts[i], Scalar(125, 255, 125), 1);
	circle(img, prevPts[i], 2, Scalar(255, 0, 125), 1);     
    }
  }
}

struct FeaturePointMatch {
  FeatureTrajectoryPtr feature;
  int pointNum;

  FeaturePointMatch(FeatureTrajectoryPtr _feature, const int& _pointNum):
    feature(_feature), pointNum(_pointNum) {}
};

inline void saveFeatures(vector<FeatureTrajectoryPtr>& features, TrajectoryDBAccess<Point2f>& db, const string& positionsTableName, const string& velocitiesTableName) {
  BOOST_FOREACH(FeatureTrajectoryPtr f, features) f->write(db, positionsTableName, velocitiesTableName);
  features.clear();
}

void trackFeatures(const KLTFeatureTrackingParameters& params) {
  // BriefDescriptorExtractor brief(32);
  // const int DESIRED_FTRS = 500;
  // GridAdaptedFeatureDetector detector(new FastFeatureDetector(10, true), DESIRED_FTRS, 4, 4);

  Mat homography = ::loadMat(params.homographyFilename, " ");
  Mat invHomography;
  if (params.display && !homography.empty())
    invHomography = homography.inv();

  float minTotalFeatureDisplacement = params.nDisplacements*params.minFeatureDisplacement;
  Size window = Size(params.windowSize, params.windowSize);

  int interpolationMethod = -1;
  if (params.interpolationMethod == 0)
    interpolationMethod = INTER_NEAREST;
  else if (params.interpolationMethod == 1)
    interpolationMethod = INTER_LINEAR;
  else if (params.interpolationMethod == 2)
    interpolationMethod = INTER_CUBIC;
  else if (params.interpolationMethod == 3)
    interpolationMethod = INTER_LANCZOS4;
  else
    cout << "Unsupported option " << interpolationMethod << " for interpolation method" << endl;

  // BruteForceMatcher<Hamming> descMatcher;
  // vector<DMatch> matches;

  if(params.videoFilename.empty()) {
    cout << "Empty video filename. Exiting." << endl;
    exit(0);
  }
    
  VideoCapture capture(params.videoFilename);
  if(!capture.isOpened()) {
    cout << "Video filename " << params.videoFilename << " could not be opened. Exiting." << endl;
    exit(0);
  }
  
  Size videoSize = Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
  unsigned int nFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
  if (nFrames <= 0) {
    cout << "Guessing that the number of frames could not be read: " << nFrames << endl;
    nFrames = numeric_limits<int>::max();
    cout << "Setting arbitrarily high number for testing: " << nFrames << endl;
  }

  cout << "Video " << params.videoFilename <<
	  ": width=" << videoSize.width <<
	  ", height=" << videoSize.height <<
	  ", nframes=" << nFrames << endl;

  Mat map1, map2;
  if (params.undistort) {
    Mat intrinsicCameraMatrix = ::loadMat(params.intrinsicCameraFilename, " ");
    Mat newIntrinsicCameraMatrix = intrinsicCameraMatrix.clone(); 
    videoSize = Size(static_cast<int>(round(videoSize.width*params.undistortedImageMultiplication)), static_cast<int>(round(videoSize.height*params.undistortedImageMultiplication)));
    newIntrinsicCameraMatrix.at<float>(0,2) = videoSize.width/2.;
    newIntrinsicCameraMatrix.at<float>(1,2) = videoSize.height/2.;
    initUndistortRectifyMap(intrinsicCameraMatrix, params.distortionCoefficients, Mat::eye(3,3, CV_32FC1), newIntrinsicCameraMatrix, videoSize, CV_32FC1, map1, map2);
    
    cout << "Undistorted width=" << videoSize.width <<
      ", height=" << videoSize.height << endl;
  }
  
  Mat mask = imread(params.maskFilename, 0);
  if (mask.empty()) {
    cout << "Mask filename " << params.maskFilename << " could not be opened." << endl;
    mask = Mat::ones(videoSize, CV_8UC1);
  }

  std::shared_ptr<TrajectoryDBAccess<Point2f> > trajectoryDB = std::shared_ptr<TrajectoryDBAccess<Point2f> >(new TrajectoryDBAccessList<Point2f>());
  //TrajectoryDBAccess<Point2f>* trajectoryDB = new TrajectoryDBAccessBlob<Point2f>();
  trajectoryDB->connect(params.databaseFilename.c_str());
  trajectoryDB->createTable("positions");
  trajectoryDB->createTable("velocities");
  trajectoryDB->beginTransaction();

  std::vector<KeyPoint> prevKpts, currKpts;
  std::vector<Point2f> prevPts, currPts, newPts;
  std::vector<uchar> status;
  std::vector<float> errors;
  Mat prevDesc, currDesc;

  std::vector<FeatureTrajectoryPtr> lostFeatures;
  std::vector<FeaturePointMatch> featurePointMatches;

  int key = '?';
  unsigned int savedFeatureId=0;
  Mat frame = Mat::zeros(1, 1, CV_8UC1), currentFrameBW, previousFrameBW, undistortedFrame;

  unsigned int lastFrameNum = nFrames;
  if (params.nFrames > 0)
    lastFrameNum = MIN(params.frame1+static_cast<unsigned int>(params.nFrames), nFrames);

  capture.set(CV_CAP_PROP_POS_FRAMES, params.frame1);
  for (unsigned int frameNum = params.frame1; (frameNum < lastFrameNum) && !::interruptionKey(key); frameNum++) {
    capture >> frame;
    if (frame.empty()) {
      cout << "Empty frame " << frameNum << ", breaking (" << frame.empty() << " [" << frame.size().width << "x" << frame.size().height << "])" << endl;
      break;
    } else if (frameNum%50 ==0)
      cout << "frame " << frameNum << endl;

    if (params.undistort) {
      remap(frame, undistortedFrame, map1, map2, interpolationMethod, BORDER_CONSTANT, 0.);
      frame = undistortedFrame;

      if (frame.size() != videoSize) {
	cout << "Different frame size " << frameNum << ", breaking ([" << frame.size().width << "x" << frame.size().height << "])" << endl;
	break;
      }
    }

      
    cvtColor(frame, currentFrameBW, CV_RGB2GRAY);
    
    if (!prevPts.empty()) {
      currPts.clear();
      calcOpticalFlowPyrLK(previousFrameBW, currentFrameBW, prevPts, currPts, status, errors, window, params.pyramidLevel, TermCriteria(static_cast<int>(TermCriteria::COUNT)+static_cast<int>(TermCriteria::EPS) /* = 3 */, params.maxNumberTrackingIterations, params.minTrackingError), /* int flags = */ 0, params.minFeatureEigThreshold);
      /// \todo try calcOpticalFlowFarneback

      std::vector<Point2f> trackedPts;
      std::vector<FeaturePointMatch>::iterator iter = featurePointMatches.begin();
      while (iter != featurePointMatches.end()) {
	bool deleteFeature = false;
	  
	if (status[iter->pointNum]) {
	  iter->feature->addPoint(frameNum, currPts[iter->pointNum], homography);

	  deleteFeature = iter->feature->isDisplacementSmall(params.nDisplacements, minTotalFeatureDisplacement)
	    || !iter->feature->isMotionSmooth(params.accelerationBound, params.deviationBound);
	  if (deleteFeature)
	    iter->feature->shorten();
	} else
	  deleteFeature = true;

	if (deleteFeature) {
	  if (iter->feature->length() >= params.minFeatureTime) {
	    iter->feature->setId(savedFeatureId);
	    savedFeatureId++;
	    iter->feature->movingAverage(params.nFramesSmoothing);
	    lostFeatures.push_back(iter->feature);
	  }
	  iter = featurePointMatches.erase(iter);
	} else {
	  trackedPts.push_back(currPts[iter->pointNum]);
	  iter->pointNum = trackedPts.size()-1;
	  iter++;
	}
      }
      currPts = trackedPts;
      assert(currPts.size() == featurePointMatches.size());
      saveFeatures(lostFeatures, *trajectoryDB, "positions", "velocities");
	
      if (params.display) {
	BOOST_FOREACH(FeaturePointMatch fp, featurePointMatches)
	  fp.feature->draw(frame, invHomography, Colors::red());
	// object detection
	// vector<Rect> locations;
	// hog.detectMultiScale(frame, locations, 0, Size(8,8), Size(32,32), 1.05, 2);
	// BOOST_FOREACH(Rect r, locations)
	//   rectangle(frame, r.tl(), r.br(), cv::Scalar(0,255,0), 3);
      }
    }
      
    // adding new features, using mask around existing feature positions
    Mat featureMask = mask.clone();
    for (unsigned int n=0;n<currPts.size(); n++)
      for (int j=MAX(0, currPts[n].x-params.minFeatureDistanceKLT); j<MIN(videoSize.width, currPts[n].x+params.minFeatureDistanceKLT+1); j++)
	for (int i=MAX(0, currPts[n].y-params.minFeatureDistanceKLT); i<MIN(videoSize.height, currPts[n].y+params.minFeatureDistanceKLT+1); i++)
	  featureMask.at<uchar>(i,j)=0;
    goodFeaturesToTrack(currentFrameBW, newPts, params.maxNFeatures, params.featureQuality, params.minFeatureDistanceKLT, featureMask, params.blockSize, params.useHarrisDetector, params.k);
    BOOST_FOREACH(Point2f p, newPts) { //for (unsigned int i=0; i<newPts.size(); i++) {
      FeatureTrajectoryPtr f = FeatureTrajectoryPtr(new FeatureTrajectory(frameNum, p, homography));
      featurePointMatches.push_back(FeaturePointMatch(f, currPts.size()));
      currPts.push_back(p);
    }
      
    if (params.display) {
      imshow("mask", featureMask*256);
      imshow("frame", frame);
      key = waitKey(2);
    }
    previousFrameBW = currentFrameBW.clone();
    prevPts = currPts;
  }

  // save the remaining currently tracked features
  std::vector<FeaturePointMatch>::iterator iter = featurePointMatches.begin();
  while (iter != featurePointMatches.end()) {
    if (iter->feature->length() >= params.minFeatureTime) {
      iter->feature->setId(savedFeatureId);
      savedFeatureId++;
      iter->feature->movingAverage(params.nFramesSmoothing);
      iter->feature->write(*trajectoryDB, "positions", "velocities");
    }
    iter++;
  }
  
  trajectoryDB->endTransaction();
  trajectoryDB->disconnect();
}

void groupFeatures(const KLTFeatureTrackingParameters& params) {
  std::shared_ptr<TrajectoryDBAccessList<Point2f> > trajectoryDB = std::shared_ptr<TrajectoryDBAccessList<Point2f> >(new TrajectoryDBAccessList<Point2f>());
  bool success = trajectoryDB->connect(params.databaseFilename.c_str());
  trajectoryDB->createObjectTable("objects", "objects_features");
  unsigned int savedObjectId=0;

  trajectoryDB->createInstants("table");
  //trajectoryDB->createIndex("positions","trajectory_id"); // does not seem to make loading features faster
  //trajectoryDB->createIndex("velocities","trajectory_id");

  unsigned int maxTrajectoryLength = 0;
  success = trajectoryDB->maxTrajectoryLength(maxTrajectoryLength);
  if (!success || maxTrajectoryLength == 0) {
    cout << "problem with trajectory length " << success << endl;
    exit(0);
  }
  cout << "Longest trajectory: " << maxTrajectoryLength << endl;

  FeatureGraph featureGraph(params.mmConnectionDistance, params.mmSegmentationDistance, params.minFeatureTime, params.minNFeaturesPerGroup);

  // main loop
  unsigned int frameNum;
  unsigned int firstFrameNum = -1, lastFrameNum = -1;
  trajectoryDB->firstLastInstants(firstFrameNum, lastFrameNum);
  firstFrameNum = MAX(firstFrameNum, params.frame1);
  if (params.nFrames>0)
    lastFrameNum = MIN(lastFrameNum,params.frame1+params.nFrames);
  for (frameNum = firstFrameNum; frameNum<lastFrameNum; frameNum ++) {
    vector<int> trajectoryIds;
    success  = trajectoryDB->trajectoryIdEndingAt(trajectoryIds, frameNum);
    if (frameNum%100 ==0)
      cout << "frame " << frameNum << endl;
#if DEBUG
    cout << trajectoryIds.size() << " trajectories " << endl;
#endif
    BOOST_FOREACH(int trajectoryId, trajectoryIds) {
      FeatureTrajectoryPtr ft = FeatureTrajectoryPtr(new FeatureTrajectory(trajectoryId, *trajectoryDB, "positions", "velocities"));
      featureGraph.addFeature(ft);
    }

    // check for connected components
    int lastInstant = frameNum+params.minFeatureTime-maxTrajectoryLength;
    if (lastInstant > 0 && frameNum%10==0) {
      featureGraph.connectedComponents(lastInstant);
      vector<vector<FeatureTrajectoryPtr> > featureGroups;
      featureGraph.getFeatureGroups(featureGroups);
      for (unsigned int i=0; i<featureGroups.size(); ++i) {
	vector<unsigned int> featureNumbers;
	for (unsigned int j=0; j<featureGroups[i].size(); ++j)
	  featureNumbers.push_back(featureGroups[i][j]->getId());
	trajectoryDB->writeObject(savedObjectId, featureNumbers, 0 /* unknown type */, 1, string("objects"), string("objects_features"));
	savedObjectId++;
      }
    }
    
    if (frameNum%100 ==0)
      cout << featureGraph.informationString() << endl;
  }

  // save remaining objects
  featureGraph.connectedComponents(frameNum+maxTrajectoryLength+1);
  vector<vector<FeatureTrajectoryPtr> > featureGroups;
  featureGraph.getFeatureGroups(featureGroups);
  for (unsigned int i=0; i<featureGroups.size(); ++i) {
    vector<unsigned int> featureNumbers;
    for (unsigned int j=0; j<featureGroups[i].size(); ++j)
      featureNumbers.push_back(featureGroups[i][j]->getId());
    trajectoryDB->writeObject(savedObjectId, featureNumbers, 0 /* unknown */, 1, string("objects"), string("objects_features"));
    savedObjectId++;
  }

  trajectoryDB->endTransaction();
  trajectoryDB->disconnect();
}

void loadingTimes(const KLTFeatureTrackingParameters& params) {
  std::shared_ptr<TrajectoryDBAccessList<Point2f> > trajectoryDB = std::shared_ptr<TrajectoryDBAccessList<Point2f> >(new TrajectoryDBAccessList<Point2f>());
  bool success = trajectoryDB->connect(params.databaseFilename.c_str());
  
  vector<std::shared_ptr<Trajectory<Point2f> > > trajectories;
  //cout << trajectories.size() << endl;
  std::clock_t c_start = std::clock();
  success = trajectoryDB->read(trajectories, "positions");
  std::clock_t c_end = std::clock();
  if (!success)
    cout << "Issue with db reading" << endl;
  cout << "Loaded " << trajectories.size() << " trajectories in " << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " CPU seconds" << endl;

  std::shared_ptr<Trajectory<cv::Point2f> > trajectory;
  c_start = std::clock();
  for (unsigned int i = 0; i<trajectories.size(); ++i) {
    success = trajectoryDB->read(trajectory, i, "positions");
  }
  c_end = std::clock();
  cout << "Loaded " << trajectories.size() << " trajectories one by one in " << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " CPU seconds" << endl;

  trajectoryDB->endTransaction();
  trajectoryDB->disconnect();
}

int main(int argc, char *argv[]) {
  KLTFeatureTrackingParameters params(argc, argv);
  cout << params.parameterDescription << endl;
  
  if (params.trackFeatures) {
    cout << "The program tracks features" << endl;
    trackFeatures(params);
  } else if (params.groupFeatures) {
    cout << "The program groups features" << endl;
    groupFeatures(params);
  } else if (params.loadingTime) {
    cout << "The program reports loading times" << endl;
    loadingTimes(params);
  } else {
    cout << "Main option missing or misspelt" << endl;
  }

  return 0;
}

/* ------------------ DOCUMENTATION ------------------ */


/*! \mainpage 

This project is a collection of software tools for transportation called Traffic Intelligence. Other documents are:

- \ref feature_based_tracking

The code is partially self-described using the doxygen tool and comment formatting. The documentation can be extracted using doxygen, typing \c doxygen in the main directory (or <tt>make doc</tt> on a system with the Makefile tool installed). 

*/

/*! \page feature_based_tracking Feature-based Tracking: User Manual

This document describes a software tool for object tracking in video data, developed for road traffic monitoring and safety diagnosis. It is part of a larger collection of software tools for transportation called Traffic Intelligence. 

The tool relies on feature-based tracking, a robust object tracking methods, particularly suited for the extraction of traffic data such as trajectories and speeds. The best description of this method is given in <a href="http://nicolas.saunier.confins.net/data/saunier06crv.html">this paper</a>. The program has a command line interface and this document will shortly explain how to use the tool. Keep in mind this is a work in progress and major changes are continuously being made. 

\section License

The code is licensed under the MIT open source license (http://www.opensource.org/licenses/mit-license).

If you make use of this piece of software, please cite one of my paper, e.g. N. Saunier, T. Sayed and K. Ismail. Large Scale Automated Analysis of Vehicle Interactions and Collisions. Transportation Research Record: Journal of the Transportation Research Board, 2147:42-50, 2010. I would be very happy in any case to know about any use of the code, and to discuss any opportunity for collaboration. 

Contact me at nicolas.saunier@polymtl.ca and learn more about my work at http://nicolas.saunier.confins.net.

*/
