#ifndef MOTION_HPP
#define MOTION_HPP

#include "src/Trajectory.h"
#include <boost/graph/adjacency_list.hpp>


template<typename T> class TrajectoryDBAccess;

typedef std::shared_ptr<TrajectoryPoint2f> TrajectoryPoint2fPtr;

/** Class for feature data
    positions, velocities and other statistics to evaluate their quality
    before saving. */
class FeatureTrajectory 
{
public:
  FeatureTrajectory(const unsigned int& frameNum, const cv::Point2f& p, const cv::Mat& homography);

  FeatureTrajectory(TrajectoryPoint2fPtr& _positions, TrajectoryPoint2fPtr& _velocities);

  /** loads from database
   can be made generic for different list and blob */
  FeatureTrajectory(const int& id, TrajectoryDBAccess<cv::Point2f>& trajectoryDB, const std::string& positionsTableName, const std::string& velocitiesTableName);

  unsigned int length(void) const { return positions->size();} // cautious if not continuous: max-min+1

  unsigned int getId(void) const { return positions->getId();}
  void setId(const unsigned int& id) { positions->setId(id);velocities->setId(id);}

  unsigned int getFirstInstant(void) {return firstInstant;}
  unsigned int getLastInstant(void) {return lastInstant;}

  //TrajectoryPoint2fPtr& getPositions(void) { return positions;}
  //TrajectoryPoint2fPtr& getVelocities(void) { return velocities;}

  /// indicates whether the sum of the last nDisplacements displacements has been inferior to minFeatureDisplacement
  bool isDisplacementSmall(const unsigned int& nDisplacements, const float& minTotalFeatureDisplacement) const;

  /// indicates whether the last two displacements are smooth (limited acceleration and angle)
  bool isMotionSmooth(const int& accelerationBound, const int& deviationBound) const;

  /// computes the distance according to the Beymer et al. algorithm
  bool minMaxSimilarity(const FeatureTrajectory& ft, const int& firstInstant, const int& lastInstant, const float& connectionDistance, const float& segmentationDistance);

  void addPoint(const unsigned int& frameNum, const cv::Point2f& p, const cv::Mat& homography);

  void shorten(void);

  void movingAverage(const unsigned int& nFramesSmoothing);

  void write(TrajectoryDBAccess<cv::Point2f>& trajectoryDB, const std::string& positionsTableName, const std::string& velocitiesTableName) const;

#ifdef USE_OPENCV
  void draw(cv::Mat& img, const cv::Mat& homography, const cv::Scalar& color) const;
#endif

  friend std::ostream& operator<<(std::ostream& out, const FeatureTrajectory& ft);

protected:
  /// first frame number
  unsigned int firstInstant;
  /// last frame number
  unsigned int lastInstant;

  TrajectoryPoint2fPtr positions;
  /** one fewer velocity than position
      v_n = p_n - p_n-1*/
  TrajectoryPoint2fPtr velocities;
  
  /// norms of velocities for feature constraints, one fewer positions than positions
  std::vector<float> displacementDistances;

  void computeMotionData(const int& frameNum);
};

typedef std::shared_ptr<FeatureTrajectory> FeatureTrajectoryPtr;

// inlined
inline std::ostream& operator<<(std::ostream& out, const FeatureTrajectory& ft) 
{
  out << *(ft.positions);
  out << "\n";
  out << *(ft.velocities);

  return out;
}

// class MovingObject {}
// roadUserType, group of features

/** Class to group features: Beymer et al. 99/Saunier and Sayed 06
    \todo create various graph types with different parameters, that accept different feature distances or ways to connect and segment features */
class FeatureGraph {
protected:
  struct FeatureConnection {
    float minDistance;
    float maxDistance;
  };
  
  struct VertexInformation {
    FeatureTrajectoryPtr feature;
    int index;
  };

  typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS, VertexInformation, FeatureConnection> UndirectedGraph;

public:
  typedef UndirectedGraph::vertex_descriptor vertex_descriptor;

  FeatureGraph(float _connectionDistance, float _segmentationDistance, unsigned int _minFeatureTime, float _minNFeaturesPerGroup) : connectionDistance (_connectionDistance), segmentationDistance(_segmentationDistance), minFeatureTime(_minFeatureTime), minNFeaturesPerGroup(_minNFeaturesPerGroup) {}

  void addFeature(const FeatureTrajectoryPtr& ft);

  // add vertex, includes adding links to current vertices
  // find connected components, check if old enough, if so, remove

  /// Computes the connected components: features have to be older than lastInstant
  void connectedComponents(const unsigned int& lastInstant);

  /** Performs some checks on groups of features and return their lists of ids if correct
      Removes the vertices from the graph */
  void getFeatureGroups(std::vector<std::vector<FeatureTrajectoryPtr> >& featureGroups);

  std::string informationString(void) const;

  int getNVertices(void) const;
  int getNEdges(void) const;

protected:
  float connectionDistance;
  float segmentationDistance;
  unsigned int minFeatureTime;
  float minNFeaturesPerGroup;
  // float minDistance;
  // float maxDistance;

  UndirectedGraph graph;
  
  std::vector<std::vector<vertex_descriptor> > objectHypotheses;

  void computeVertexIndex(void);

  //std::vector<UndirectedGraph::vertex_descriptor> currentVertices, lostVertices;
};

// inlined implementations
// inline FeatureGraph::FeatureGraph(float _minDistance, float _maxDistance)

#endif
