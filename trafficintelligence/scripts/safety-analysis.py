#! /usr/bin/env python

import storage, prediction, events, moving

import sys, argparse, random

import matplotlib.pyplot as plt
import numpy as np

# todo: very slow if too many predicted trajectories
# add computation of probality of unsucessful evasive action

parser = argparse.ArgumentParser(description='The program processes indicators for all pairs of road users in the scene')
parser.add_argument('--cfg', dest = 'configFilename', help = 'name of the configuration file', required = True)
parser.add_argument('--prediction-method', dest = 'predictionMethod', help = 'prediction method (constant velocity (vector computation), constant velocity, normal adaptation, point set prediction)', choices = ['cvd', 'cv', 'na', 'ps'])
parser.add_argument('--display-cp', dest = 'displayCollisionPoints', help = 'display collision points', action = 'store_true')
parser.add_argument('-n', dest = 'nProcesses', help = 'number of processes to run in parallel', type = int, default = 1)
args = parser.parse_args()

params = storage.ProcessParameters(args.configFilename)

# parameters for prediction methods
if args.predictionMethod:
    predictionMethod = args.predictionMethod
else:
    predictionMethod = params.predictionMethod

def accelerationDistribution(): 
    return random.triangular(-params.maxNormalAcceleration, params.maxNormalAcceleration, 0.)
def steeringDistribution():
    return random.triangular(-params.maxNormalSteering, params.maxNormalSteering, 0.)

if predictionMethod == 'cvd': # TODO add cve: constant velocity exact (Sohail's)
    predictionParameters = prediction.CVDirectPredictionParameters()
elif predictionMethod == 'cv':
    predictionParameters = prediction.ConstantPredictionParameters(params.maxPredictedSpeed)
elif predictionMethod == 'na':
    predictionParameters = prediction.NormalAdaptationPredictionParameters(params.maxPredictedSpeed,
                                                                           params.nPredictedTrajectories, 
                                                                           accelerationDistribution,
                                                                           steeringDistribution,
                                                                           params.useFeaturesForPrediction)
elif predictionMethod == 'ps':
    predictionParameters = prediction.PointSetPredictionParameters(params.maxPredictedSpeed)
# no else required, since parameters is required as argument

# evasiveActionPredictionParameters = prediction.EvasiveActionPredictionParameters(params.maxPredictedSpeed, 
#                                                                                  params.nPredictedTrajectories, 
#                                                                                  params.minExtremeAcceleration,
#                                                                                  params.maxExtremeAcceleration,
#                                                                                  params.maxExtremeSteering,
#                                                                                  params.useFeaturesForPrediction)

objects = storage.loadTrajectoriesFromSqlite(params.databaseFilename,'object')
if params.useFeaturesForPrediction:
    features = storage.loadTrajectoriesFromSqlite(params.databaseFilename,'feature') # needed if normal adaptation
    for obj in objects:
        obj.setFeatures(features)

interactions = events.createInteractions(objects)
for inter in interactions:
    inter.computeIndicators()
    inter.computeCrossingsCollisions(predictionParameters, params.collisionDistance, params.predictionTimeHorizon, params.crossingZones, nProcesses = args.nProcesses)

storage.saveIndicators(params.databaseFilename, interactions)

if args.displayCollisionPoints:
    plt.figure()
    allCollisionPoints = []
    for inter in interactions:
        for collisionPoints in inter.collisionPoints.values():
            allCollisionPoints += collisionPoints
    moving.Point.plotAll(allCollisionPoints)
    plt.axis('equal')
    
