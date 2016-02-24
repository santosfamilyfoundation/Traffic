#include "Motion.hpp"
#include "cvutils.hpp"

#include "src/TrajectoryDBAccessList.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <boost/graph/connected_components.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <memory>

using namespace std;
using namespace cv;
using namespace boost;

/******************** FeatureTrajectory ********************/

FeatureTrajectory::FeatureTrajectory(const unsigned int& frameNum, const cv::Point2f& p, const Mat& homography)
  : firstInstant(frameNum), lastInstant(frameNum) {
  positions = TrajectoryPoint2fPtr(new TrajectoryPoint2f());
  velocities = TrajectoryPoint2fPtr(new TrajectoryPoint2f());
  addPoint(frameNum, p, homography);
}

FeatureTrajectory::FeatureTrajectory(TrajectoryPoint2fPtr& _positions, TrajectoryPoint2fPtr& _velocities) {
  positions = _positions;
  velocities = _velocities;
  positions->computeInstants(firstInstant, lastInstant);
}

FeatureTrajectory::FeatureTrajectory(const int& id, TrajectoryDBAccess<Point2f>& trajectoryDB, const string& positionsTableName, const string& velocitiesTableName) {
  bool success = trajectoryDB.read(positions, id, positionsTableName);
  if (!success)
    cout << "problem loading positions" << endl;
  success = trajectoryDB.read(velocities, id, velocitiesTableName);
  if (!success)
    cout << "problem loading velocities" << endl;
  // take advantage to request first and last instant from database
  trajectoryDB.timeInterval(firstInstant, lastInstant, id);
}

bool FeatureTrajectory::isDisplacementSmall(const unsigned int& nDisplacements, const float& minTotalFeatureDisplacement) const {
  bool result = false;
  unsigned int nPositions = positions->size();
  if (nPositions > nDisplacements) {
    float disp = 0;
    for (unsigned int i=0; i<nDisplacements; i++)
      disp += displacementDistances[nPositions-2-i];
    result = disp <= minTotalFeatureDisplacement;
  }
  return result;
}

bool FeatureTrajectory::isMotionSmooth(const int& accelerationBound, const int& deviationBound) const {
  bool result = true;
  unsigned int nPositions = positions->size();
  if (nPositions >= 3) {
    float ratio;
    if (displacementDistances[nPositions-2] > displacementDistances[nPositions-3])
      ratio = displacementDistances[nPositions-2] / displacementDistances[nPositions-3];
    else
      ratio = displacementDistances[nPositions-3] / displacementDistances[nPositions-2];

    float cosine = (*velocities)[nPositions-3].dot((*velocities)[nPositions-2]) / (displacementDistances[nPositions-3] * displacementDistances[nPositions-2]);
    
    result = (ratio < accelerationBound) & (cosine > deviationBound);
  }
  return result;
}

bool FeatureTrajectory::minMaxSimilarity(const FeatureTrajectory& ft, const int& firstInstant, const int& lastInstant, const float& connectionDistance, const float& segmentationDistance) {
  float minDistance = norm(positions->getPointAtInstant(firstInstant)-ft.positions->getPointAtInstant(firstInstant));
  float maxDistance = minDistance;
  bool connected = (minDistance <= connectionDistance);
  int t=firstInstant+1;
  while (t <= lastInstant && connected) {
    float distance = norm(positions->getPointAtInstant(t)-ft.positions->getPointAtInstant(t));
    if (distance < minDistance)
      minDistance = distance;
    else if (distance > maxDistance)
      maxDistance = distance;
    connected = connected && (maxDistance-minDistance <= segmentationDistance);
    t++;
  }

  return connected;
}

void FeatureTrajectory::addPoint(const unsigned int& frameNum, const Point2f& p, const Mat& homography) {
  Point2f pp = p;
  if (!homography.empty())
    pp = project(p, homography);
  positions->add(frameNum, pp);
  if (frameNum < firstInstant)
    firstInstant = frameNum;
  if (frameNum > lastInstant)
    lastInstant = frameNum;
  computeMotionData(frameNum);
  assert(positions->size() == displacementDistances.size()+1);
  assert(positions->size() == velocities->size()+1);
}

void FeatureTrajectory::shorten(void) { 
  positions->pop_back(); 
  velocities->pop_back(); 
  displacementDistances.pop_back();
}

void FeatureTrajectory::movingAverage(const unsigned int& nFramesSmoothing) {
  positions->movingAverage(nFramesSmoothing);
  velocities->movingAverage(nFramesSmoothing);
}

void FeatureTrajectory::write(TrajectoryDBAccess<Point2f>& trajectoryDB, const string& positionsTableName, const string& velocitiesTableName) const {
  trajectoryDB.write(*positions, positionsTableName);
  trajectoryDB.write(*velocities, velocitiesTableName);
}

#ifdef USE_OPENCV
/// \todo add option for anti-aliased drawing, thickness
void FeatureTrajectory::draw(Mat& img, const Mat& homography, const Scalar& color) const {
  Point2f p1, p2;
  if (!homography.empty())
    p1 = project((*positions)[0], homography);
  else
    p1 = (*positions)[0];
  for (unsigned int i=1; i<positions->size(); i++) {
    if (!homography.empty())
      p2 = project((*positions)[i], homography);
    else
      p2 = (*positions)[i];
    line(img, p1, p2, color, 1);
    p1 = p2;
  }
}
#endif

// protected

void FeatureTrajectory::computeMotionData(const int& frameNum) {
  unsigned int nPositions = positions->size();
  if (nPositions >= 2) {
    Point2f displacement = (*positions)[nPositions-1] - (*positions)[nPositions-2];
    //if (nPositions == 2) // duplicate first displacement so that positions and velocities have the same length
    //velocities.add(frameNum-1, displacement);
    velocities->add(frameNum, displacement);
    float dist = norm(displacement);
    displacementDistances.push_back(dist);
  }
}

/******************** FeatureGraph ********************/

void FeatureGraph::addFeature(const FeatureTrajectoryPtr& ft) {
  vertex_descriptor newVertex = add_vertex(graph);
  graph[newVertex].feature = ft;
  for (graph_traits<UndirectedGraph>::vertex_iterator vi = vertices(graph).first; 
       vi!=vertices(graph).second; ++vi) { // vi pointer to vertex_descriptor
    FeatureTrajectoryPtr ft2 = graph[*vi].feature;
    if (newVertex != *vi) {
      int lastInstant = static_cast<int>(MIN(ft->getLastInstant(), ft2->getLastInstant()));
      int firstInstant = static_cast<int>(MAX(ft->getFirstInstant(), ft2->getFirstInstant()));
      if (lastInstant-firstInstant > static_cast<int>(minFeatureTime)) { // equivalent to lastInstant-firstInstant+1 >= minFeatureTime
	if (ft->minMaxSimilarity(*ft2, firstInstant, lastInstant, connectionDistance, segmentationDistance)) {
	  UndirectedGraph::edge_descriptor e;
	  bool unused;
	  boost::tuples::tie(e, unused) = add_edge(newVertex, *vi, graph);
	  // no need to add measures to graph[e] (edge properties)
	}
      }
    }
  }
}

void FeatureGraph::connectedComponents(const unsigned int& lastInstant) {
  computeVertexIndex();
  property_map<UndirectedGraph, int VertexInformation::*>::type components = get(&VertexInformation::index, graph);

  int num = connected_components(graph, components, vertex_index_map(get(&VertexInformation::index, graph)));
#ifdef DEBUG
  cout << "last instant threshold " << lastInstant << " Total number of components: " << num << endl;
#endif

  vector<unsigned int> lastInstants(num, 0); // last instant of component with id
  vector<vector<vertex_descriptor> > tmpobjects(num); // vector of components (component = vector of vertex descriptors)

  graph_traits<UndirectedGraph>::vertex_iterator vi, vend;
  for(boost::tuples::tie(vi,vend) = vertices(graph); vi != vend; ++vi) {
    unsigned int id = components[*vi];
    lastInstants[id] = max(lastInstants[id], graph[*vi].feature->getLastInstant());
    tmpobjects[id].push_back(*vi);
  }

  objectHypotheses.clear();
  for (int i = 0; i < num; ++i) {
#ifdef DEBUG
    cout << i << " " << lastInstants[i] << endl;
#endif
    if (lastInstants[i] < lastInstant)
      objectHypotheses.push_back(tmpobjects[i]);
  }
}

void FeatureGraph::getFeatureGroups(vector<vector<FeatureTrajectoryPtr> >& featureGroups) {
  featureGroups.clear();

  for (unsigned int i=0; i<objectHypotheses.size(); ++i) {
    // check that there is on average at least minNFeaturesPerGroup features at each frame in the group
    unsigned int totalFeatureTime= graph[objectHypotheses[i][0]].feature->length();
    unsigned int firstInstant = graph[objectHypotheses[i][0]].feature->getFirstInstant();
    unsigned int lastInstant = graph[objectHypotheses[i][0]].feature->getLastInstant();
    for (unsigned int j=1; j<objectHypotheses[i].size(); ++j) {
      totalFeatureTime += graph[objectHypotheses[i][j]].feature->length();
      firstInstant = MIN(firstInstant, graph[objectHypotheses[i][j]].feature->getFirstInstant());
      lastInstant = MAX(lastInstant, graph[objectHypotheses[i][j]].feature->getLastInstant());	
    }
    bool saveFeatureGroup = (static_cast<float>(totalFeatureTime)/static_cast<float>(lastInstant-firstInstant+1) > minNFeaturesPerGroup);
#if DEBUG
    cout << "save group " << i << " of " << objectHypotheses[i].size() << " features " << endl;
#endif
    if (saveFeatureGroup)
      featureGroups.push_back(vector<FeatureTrajectoryPtr>());
    for (unsigned int j=0; j<objectHypotheses[i].size(); ++j) {
      if (saveFeatureGroup)
	featureGroups.back().push_back(graph[objectHypotheses[i][j]].feature);
#if DEBUG
      cout << featureGroups.size() << " " << objectHypotheses[i][j] << endl;
#endif
      clear_vertex(objectHypotheses[i][j], graph);
      remove_vertex(objectHypotheses[i][j], graph);
    }
  }
}

string FeatureGraph::informationString(void) const {
  stringstream ss;
  ss << num_vertices(graph) << " vertices, " << num_edges(graph) << " edges";
  return ss.str();
}

int FeatureGraph::getNVertices(void) const { return num_vertices(graph);}

int FeatureGraph::getNEdges(void) const  { return num_edges(graph);}

void FeatureGraph::computeVertexIndex(void) {
  graph_traits<FeatureGraph::UndirectedGraph>::vertex_iterator vi, vend;
  graph_traits<FeatureGraph::UndirectedGraph>::vertices_size_type cnt = 0;
  for(boost::tuples::tie(vi,vend) = vertices(graph); vi != vend; ++vi)
    graph[*vi].index = cnt++;
}
