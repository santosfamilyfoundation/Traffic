#include "Motion.hpp"
#include "testutils.hpp"

#include "opencv2/core/core.hpp"

#include "catch.hpp"

#include <iostream>

using namespace std;
using namespace cv;

TEST_CASE("graph/connected_components", "test graph connected components") {
  float connectionDistance = 5.;
  float segmentationDistance = 1.;
  unsigned int minFeatureTime = 5;
  float minNFeaturesPerGroup = 0.99;
  FeatureGraph featureGraph(connectionDistance, segmentationDistance, minFeatureTime, minNFeaturesPerGroup);
  unsigned int firstInstant = 10, lastInstant = 20;
  FeatureTrajectoryPtr ft1 = createFeatureTrajectory(1, firstInstant, lastInstant, Point2f(1,1), Point2f(0.5, 0.));
  FeatureTrajectoryPtr ft2 = createFeatureTrajectory(2, firstInstant, lastInstant, Point2f(1.1,1), Point2f(0.5, 0.));

  featureGraph.addFeature(ft1);
  REQUIRE(featureGraph.getNVertices() == 1);
  REQUIRE(featureGraph.getNEdges() == 0);

  featureGraph.addFeature(ft2);
  REQUIRE(featureGraph.getNVertices() == 2);
  REQUIRE(featureGraph.getNEdges() == 1);

  featureGraph.connectedComponents(lastInstant);
  vector<vector<FeatureTrajectoryPtr> > components;
  featureGraph.getFeatureGroups(components);
  REQUIRE(components.size() == 0);
  REQUIRE(featureGraph.getNVertices() == 2);
  REQUIRE(featureGraph.getNEdges() == 1);

  featureGraph.connectedComponents(lastInstant+1);
  featureGraph.getFeatureGroups(components);
  REQUIRE(components.size() == 1);
  REQUIRE(components[0].size() == 2);
  REQUIRE(featureGraph.getNVertices() == 0);
  REQUIRE(featureGraph.getNEdges() == 0);

  // test connection distance
  featureGraph.addFeature(ft1);
  featureGraph.addFeature(ft2);
  FeatureTrajectoryPtr ft3 = createFeatureTrajectory(3, firstInstant, lastInstant, Point2f(6.05,1), Point2f(0.5, 0.)); // connected to ft2 only
  featureGraph.addFeature(ft3);
  FeatureTrajectoryPtr ft4 = createFeatureTrajectory(4, firstInstant, lastInstant, Point2f(11.1,1), Point2f(0.5, 0.)); // not connected
  featureGraph.addFeature(ft4);

  REQUIRE(ft1->minMaxSimilarity(*ft2, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  REQUIRE(ft2->minMaxSimilarity(*ft3, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  REQUIRE_FALSE(ft1->minMaxSimilarity(*ft3, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  REQUIRE_FALSE(ft1->minMaxSimilarity(*ft4, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  REQUIRE_FALSE(ft2->minMaxSimilarity(*ft4, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  REQUIRE_FALSE(ft3->minMaxSimilarity(*ft4, firstInstant, lastInstant, connectionDistance, segmentationDistance));
  
  REQUIRE(featureGraph.getNVertices() == 4);
  REQUIRE(featureGraph.getNEdges() == 2);

  featureGraph.connectedComponents(lastInstant+1);
  featureGraph.getFeatureGroups(components);
  REQUIRE(components.size() == 2);
  REQUIRE(components[0].size() == 3);
  REQUIRE(components[1].size() == 1);
  REQUIRE(featureGraph.getNVertices() == 0);
  REQUIRE(featureGraph.getNEdges() == 0);
}
