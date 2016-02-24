#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

/// \todo Class for parameters, with utilities to save and load from configuration files

#include <string>
#include <vector>

namespace boost{
  namespace program_options {
    class options_description;
    class variables_map;
  }
}

struct KLTFeatureTrackingParameters {
  bool trackFeatures;
  bool groupFeatures;
  bool loadingTime;

  std::string videoFilename;
  std::string databaseFilename;
  std::string homographyFilename;
  std::string intrinsicCameraFilename;
  std::vector<float> distortionCoefficients;
  float undistortedImageMultiplication;
  int interpolationMethod;
  std::string maskFilename;
  bool undistort;
  bool loadFeatures;
  bool display;
  float videoFPS;
  // int measurementPrecision;
  unsigned int frame1;
  int nFrames;
  // feature tracking
  /// "Maximum number of corners to return" (OpenCV goodFeaturesToTrack) (should be large enough not to limit the potential number of features)
  int maxNFeatures;
  /// "Parameter characterizing the minimal accepted quality of image corners" (OpenCV goodFeaturesToTrack )
  float featureQuality;
  /// "Minimum possible Euclidean distance between the returned corners" (OpenCV goodFeaturesToTrack)
  float minFeatureDistanceKLT;
  /// "Size of an average block for computing a derivative covariation matrix over each pixel neighborhood" (OpenCV goodFeaturesToTrack)
  int blockSize;
  /// "Parameter indicating whether to use a Harris detector" (OpenCV goodFeaturesToTrack)
  bool useHarrisDetector;
  /// "Free parameter of the Harris detector" (OpenCV goodFeaturesToTrack)
  float k;
  /// "size of the search window at each pyramid level" (OpenCV calcOpticalFlowPyrLK)
  int windowSize;
  /// "0-based maximal pyramid level number" (OpenCV calcOpticalFlowPyrLK) higher is higher quality
  int pyramidLevel;
  /// Number of displacements (number of frames-1) over which minimum motion is computed 
  unsigned int nDisplacements;
  /// Minimum displacement per frame (in world space) to keep features
  float minFeatureDisplacement;
  /// Maximum feature acceleration
  float accelerationBound;
  /// Maximum feature deviation
  float deviationBound;
  /// Number of frames to smooth positions (half window)
  int nFramesSmoothing;
  //int nFramesVelocity;
  /// Maximum number of iterations to stop optical flow (OpenCV calcOpticalFlowPyrLK)
  int maxNumberTrackingIterations;
  /// Minimum error to reach to stop optical flow (OpenCV calcOpticalFlowPyrLK)
  float minTrackingError;
  /// Minimum eigen value of a 2x2 normal matrix of optical flow equations (OpenCV calcOpticalFlowPyrLK)
  float minFeatureEigThreshold;
  /// Minimum length of a feature (number of frames) to consider a feature for grouping
  unsigned int minFeatureTime;
  /// Connection distance in feature grouping (in world space)
  float mmConnectionDistance;
  /// Segmentation distance in feature grouping (in world space)
  float mmSegmentationDistance;
  /// Maximum distance between features for grouping (in world space) (unused)
  float maxDistance;
  /// Minimum cosine of the angle between the velocity vectors for grouping (unused)
  float minVelocityCosine;
  /// Minimum average number of features per frame to create a vehicle hypothesis
  float minNFeaturesPerGroup;
  // safety analysis
  float maxPredictedSpeed;
  float predictionTimeHorizon;
  float collisionDistance;
  bool crossingZones;
  std::string predictionMethod;
  int nPredictedTrajectories;
  float minAcceleration;
  float maxAcceleration;
  float maxSteering;
  bool useFeaturesForPrediction;

  std::string parameterDescription;

  KLTFeatureTrackingParameters(const int argc, char* argv[]);

  //KLTFeatureTrackingParameters(bool loadFeatures, std::string videoFilename, int videoFPS, int measurementPrecision, int frame1, int nFrames, int maxNFeatures, float featureQuality, float minFeatureDistanceKLT, int windowSize, int pyramidLevel, int nDisplacements, float minFeatureDisplacement, float accelerationBound, float deviationBound, int nFramesSmoothing, int nFramesVelocity, int maxNumberTrackingIterations, float minTrackingError, int minFeatureTime, float mmConnectionDistance, float mmSegmentationDistance, float maxDistance, float minVelocityCosine, int minNFeaturesPerGroup);

  std::string getParameterDescription(boost::program_options::options_description& options, const boost::program_options::variables_map& vm, const std::string& separator = " ") const;
};

#endif
