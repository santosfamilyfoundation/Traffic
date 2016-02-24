#include "Parameters.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>

namespace po = boost::program_options;
using namespace std;

KLTFeatureTrackingParameters::KLTFeatureTrackingParameters(const int argc, char* argv[]) {
  string configurationFilename;
  po::options_description onlyCmdLine("Command line only");
  po::options_description cmdLineAndFile("Command line and configuration file");

  // configuration filename
  onlyCmdLine.add_options()
    ("help,h", "displays this help message")
    ("tf", "tracks features")
    ("gf", "groups features")
    ("loading-time", "report feature and object loading times")
    ("config-file", po::value<string>(&configurationFilename), "configuration file")
    ;

  po::positional_options_description p;
  p.add("config-file", 1);
  
  // common to cnnfiguration and command line
  cmdLineAndFile.add_options()
    ("video-filename", po::value<string>(&videoFilename), "filename of the video to process")
    ("database-filename", po::value<string>(&databaseFilename), "filename of the database where results are saved")
    ("homography-filename", po::value<string>(&homographyFilename), "filename of the homography matrix")
    ("intrinsic-camera-filename", po::value<string>(&intrinsicCameraFilename), "filename of the homography matrix")
    ("distortion-coefficients", po::value<std::vector<float> >(&distortionCoefficients)->multitoken(), "")
    ("undistorted-size-multiplication", po::value<float>(&undistortedImageMultiplication), "undistorted image multiplication")
    ("interpolation-method", po::value<int>(&interpolationMethod), "Interpolation method for remapping image when correcting for distortion: 0 for INTER_NEAREST - a nearest-neighbor interpolation; 1 for INTER_LINEAR - a bilinear interpolation (used by default); 2 for INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood; 3 for INTER_LANCZOS4")
    ("mask-filename", po::value<string>(&maskFilename), "filename of the mask image (where features are detected)")
    ("undistort", po::value<bool>(&undistort), "undistort the video for feature tracking")
    ("load-features", po::value<bool>(&loadFeatures), "load features from database")
    ("display", po::value<bool>(&display), "display trajectories on the video")
    ("video-fps", po::value<float>(&videoFPS), "original video frame rate")
    ("frame1", po::value<unsigned int>(&frame1), "first frame to process")
    ("nframes", po::value<int>(&nFrames), "number of frame to process")
    // feature tracking
    ("max-nfeatures", po::value<int>(&maxNFeatures), "maximum number of features added at each frame (1000s)")
    ("feature-quality", po::value<float>(&featureQuality), "quality level of the good features to track (]0. 1?])")
    ("min-feature-distanceklt", po::value<float>(&minFeatureDistanceKLT), "minimum distance between features (]0. 10?])")
    ("block-size", po::value<int>(&blockSize), "size of the block for feature characteristics ([1 ?])")
    ("use-harris-detector", po::value<bool>(&useHarrisDetector), "use of Harris corner detector")
    ("k", po::value<float>(&k), "k parameter to detect good features to track (OpenCV)")
    ("window-size", po::value<int>(&windowSize), "size of the search window at each pyramid level ([1 ?])")
    ("pyramid-level", po::value<int>(&pyramidLevel), "maximal pyramid level in the feature tracking algorithm ([0 maxLevel=5?])")
    ("ndisplacements", po::value<unsigned int>(&nDisplacements), "number of displacements to test minimum feature motion ([2 4])")
    ("min-feature-displacement", po::value<float>(&minFeatureDisplacement), "minimum displacement per frame (in world space) to keep features (]0. 0.1?])")
    ("acceleration-bound", po::value<float>(&accelerationBound), "maximum feature acceleration (]1 3+])")
    ("deviation-bound", po::value<float>(&deviationBound), "maximum feature deviation (on cosine) (]0 1])")
    ("smoothing-halfwidth", po::value<int>(&nFramesSmoothing), "number of frames to smooth positions (half window) ([0 inf[")
    ("max-number-iterations", po::value<int>(&maxNumberTrackingIterations), "maximum number of iterations to stop optical flow (20-30?)")
    ("min-tracking-error", po::value<float>(&minTrackingError), "minimum error to reach to stop optical flow (0.3-0.01)")
    ("min-feature-eig-threshold", po::value<float>(&minFeatureEigThreshold)->default_value(1e-4), "minimum eigen value of a 2x2 normal matrix of optical flow equations (10^-4)")
    ("min-feature-time", po::value<unsigned int>(&minFeatureTime), "minimum length of a feature (number of frames) to consider a feature for grouping [5 20+]")
    ("mm-connection-distance", po::value<float>(&mmConnectionDistance), "connection distance in feature grouping (in world space) (ped: [0.5m 2m+], cars: [1.7m 4m+])")
    ("mm-segmentation-distance", po::value<float>(&mmSegmentationDistance), "segmentation distance in feature grouping (in world space) (< mm-connection-distance, empirically ~ mm-connection-distance / 2.5)")
    ("max-distance", po::value<float>(&maxDistance), "maximum distance between features for grouping (in world space) (unused)")
    ("min-velocity-cosine", po::value<float>(&minVelocityCosine), "minimum cosine of the angle between the velocity vectors for grouping (unused)")
    ("min-nfeatures-group", po::value<float>(&minNFeaturesPerGroup), "minimum average number of features per frame to create a vehicle hypothesis (]1 3+])")
    // ("max-uturn-cosine", po::value<float>(&maxUTurnCosine), "maximum cosine value to detect U-turn")
    // ("nframes-avoid-uturn", po::value<int>(&nFramesAvoidUTurn), "number of frames over which a feature should not make a U-turn")
    // Safety Analysis
    ("max-predicted-speed", po::value<float>(&maxPredictedSpeed)->default_value(50.), "maximum speed when predicting future motion (km/h)")
    ("prediction-time-horizon", po::value<float>(&predictionTimeHorizon)->default_value(5.), "time horizon for collision prediction (s)")
    ("collision-distance", po::value<float>(&collisionDistance)->default_value(1.8), "collision distance threshold (m)")
    ("crossing-zones", po::value<bool>(&crossingZones)->default_value(false), "option to compute crossing zones and predicted PET")
    ("prediction-method", po::value<string>(&predictionMethod)->default_value("na"), "prediction method")
    ("npredicted-trajectories", po::value<int>(&nPredictedTrajectories)->default_value(1), "number of predicted trajectories (use depends on prediction method)")
    ("min-acceleration", po::value<float>(&minAcceleration)->default_value(-9.1), "minimum acceleration for input distribution (m/s2) (used only for evasive action distributions)")
    ("max-acceleration", po::value<float>(&maxAcceleration)->default_value(2.), "maximum acceleration for input distribution (m/s2)")
    ("max-steering", po::value<float>(&maxSteering)->default_value(0.5), "maximum steering for input distribution (rad/s)")
    ("use-features-prediction", po::value<bool>(&useFeaturesForPrediction)->default_value(false), "use feature positions and velocities for prediction")
    ;
  
  po::options_description cmdLine;
  cmdLine.add(onlyCmdLine).add(cmdLineAndFile);
  try {
    po::variables_map vm;
    store(po::command_line_parser(argc, argv).
	  options(cmdLine).positional(p).allow_unregistered().run(), vm);
    notify(vm);

    if (vm.count("config-file") == 0) {
      cout << "Missing configuration file" << endl;
      cout << cmdLine << endl;
      exit(0);      
    }
      
    cout << "Using configuration file " << configurationFilename << endl;

    ifstream configurationFile(configurationFilename.c_str());
    store(po::parse_config_file(configurationFile, cmdLineAndFile, true), vm);
    notify(vm);

    parameterDescription = getParameterDescription(cmdLineAndFile, vm);

    trackFeatures = vm.count("tf")>0;
    groupFeatures = vm.count("gf")>0;
    loadingTime = vm.count("loading-time")>0;

    if (vm.count("help")) {
      cout << cmdLine << endl;
      // cout << "Positional options:";
      // for (unsigned int i=0; i<p.max_total_count(); i++)
      // 	cout << " " << p.name_for_position(i);
      // cout << endl;
      exit(0);
    }
  } catch(exception& e) {
    cout << e.what() << endl;
  }
}

string KLTFeatureTrackingParameters::getParameterDescription(po::options_description& options, const po::variables_map& vm, const string& separator /* = " " */) const {
  stringstream stream;
  vector<boost::shared_ptr<po::option_description> > optionsVec = options.options();
  for (unsigned int i=0; i<optionsVec.size(); ++i) {
    boost::any value = vm[optionsVec[i]->long_name()].value();
    if (value.type() == typeid(bool))
      stream << boost::any_cast<bool>(value) << separator;
    else if (value.type() == typeid(int))
      stream << boost::any_cast<int>(value) << separator;
    else if (value.type() == typeid(unsigned int))
      stream << boost::any_cast<unsigned int>(value) << separator;
    else if (value.type() == typeid(float))
      stream << boost::any_cast<float>(value) << separator;
    else if (value.type() == typeid(string))
      stream << boost::any_cast<string>(value) << separator;
    else if (value.type() == typeid(vector<float>)) {
      for (unsigned int j=0; j<boost::any_cast<vector<float> >(value).size(); j++)
	     stream << boost::any_cast<vector<float> >(value)[j] << separator;
    } else
      cerr << "the type of the option " << optionsVec[i]->long_name() << " (" << i << ") is not int, float or string." << endl;
  }

  return stream.str();
}
