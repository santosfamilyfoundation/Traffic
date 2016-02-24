#! /usr/bin/env python

import cvutils, moving, ml, storage

import numpy as np
import sys, argparse
#from cv2 import SVM_RBF, SVM_C_SVC
import cv2
from scipy.stats import norm, lognorm

# TODO add mode detection live

parser = argparse.ArgumentParser(description='The program processes indicators for all pairs of road users in the scene')
parser.add_argument('--cfg', dest = 'configFilename', help = 'name of the configuration file', required = True)
parser.add_argument('-d', dest = 'databaseFilename', help = 'name of the Sqlite database file (overrides the configuration file)')
parser.add_argument('-i', dest = 'videoFilename', help = 'name of the video file (overrides the configuration file)')
parser.add_argument('-n', dest = 'nObjects', help = 'number of objects to classify', type = int, default = None)
parser.add_argument('--plot-speed-distributions', dest = 'plotSpeedDistribution', help = 'simply plots the distributions used for each user type', action = 'store_true')
parser.add_argument('--max-speed-distribution-plot', dest = 'maxSpeedDistributionPlot', help = 'if plotting the user distributions, the maximum speed to display', type = float, default = 50.)

args = parser.parse_args()
params = storage.ProcessParameters(args.configFilename)

if args.videoFilename is not None:
    videoFilename = args.videoFilename
else:
    videoFilename = params.videoFilename
if args.databaseFilename is not None:
    databaseFilename = args.databaseFilename
else:
    databaseFilename = params.databaseFilename

params.convertToFrames(3.6)
if params.homography is not None:
    invHomography = np.linalg.inv(params.homography)

if params.speedAggregationMethod == 'median':
    speedAggregationFunc = np.median
elif params.speedAggregationMethod == 'mean':
    speedAggregationFunc = np.mean
elif params.speedAggregationMethod == 'quantile':
    speedAggregationFunc = lambda speeds: np.percentile(speeds, args.speedAggregationQuantile)
else:
    print('Unknown speed aggregation method: {}. Exiting'.format(params.speedAggregationMethod))
    sys.exit()

pedBikeCarSVM = ml.SVM()
pedBikeCarSVM.load(params.pedBikeCarSVMFilename)
bikeCarSVM = ml.SVM()
bikeCarSVM.load(params.bikeCarSVMFilename)

# log logistic for ped and bik otherwise ((pedBeta/pedAlfa)*((sMean/pedAlfa)**(pedBeta-1)))/((1+(sMean/pedAlfa)**pedBeta)**2.)
speedProbabilities = {'car': lambda s: norm(params.meanVehicleSpeed, params.stdVehicleSpeed).pdf(s),
                      'pedestrian': lambda s: norm(params.meanPedestrianSpeed, params.stdPedestrianSpeed).pdf(s), 
                      'bicycle': lambda s: lognorm(params.scaleCyclistSpeed, loc = 0., scale = np.exp(params.locationCyclistSpeed)).pdf(s)} # numpy lognorm shape, loc, scale: shape for numpy is scale (std of the normal) and scale for numpy is location (mean of the normal)

if args.plotSpeedDistribution:
    import matplotlib.pyplot as plt
    plt.figure()
    for k in speedProbabilities:
        plt.plot(np.arange(0.1, args.maxSpeedDistributionPlot, 0.1), [speedProbabilities[k](s/3.6/25) for s in np.arange(0.1, args.maxSpeedDistributionPlot, 0.1)], label = k)
    plt.xlabel('Speed (km/h)')
    plt.ylabel('Probability')
    plt.legend()
    plt.title('Probability Density Function')
    plt.show()
    sys.exit()

objects = storage.loadTrajectoriesFromSqlite(databaseFilename, 'object', args.nObjects, withFeatures = True)
#features = storage.loadTrajectoriesFromSqlite(databaseFilename, 'feature')
intervals = []
for obj in objects:
    #obj.setFeatures(features)
    intervals.append(obj.getTimeInterval())
timeInterval = moving.unionIntervals(intervals)

capture = cv2.VideoCapture(videoFilename)
width = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
height = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))

pastObjects = []
if params.undistort: # setup undistortion
    [map1, map2] = computeUndistortMaps(width, height, undistortedImageMultiplication, intrinsicCameraMatrix, distortionCoefficients)
if capture.isOpened():
    ret = True
    frameNum = timeInterval.first
    capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, frameNum)
    lastFrameNum = timeInterval.last

    while ret and frameNum <= lastFrameNum:
        ret, img = capture.read()
        if ret:
            if frameNum%50 == 0:
                print('frame number: {}'.format(frameNum))
                currentObjects = []
                for obj in objects:
                    if obj.getLastInstant() < frameNum:
                        obj.classifyUserTypeHoGSVM(minSpeedEquiprobable = params.minSpeedEquiprobable, speedProbabilities = speedProbabilities)
                        pastObjects.append(obj)
                    else:
                        currentObjects.append(obj)
                objects = currentObjects
            if params.undistort:
                img = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR)
            for obj in objects:
                if obj.existsAtInstant(frameNum):
                    if obj.getFirstInstant() == frameNum:
                        obj.initClassifyUserTypeHoGSVM(speedAggregationFunc, pedBikeCarSVM, bikeCarSVM, params.maxPedestrianSpeed, params.maxCyclistSpeed, params.nFramesIgnoreAtEnds)
                    obj.classifyUserTypeHoGSVMAtInstant(img, frameNum, invHomography, width, height, 0.2, 0.2, 800) # px, py, pixelThreshold
        frameNum += 1
    
    for obj in objects:
        obj.classifyUserTypeHoGSVM(minSpeedEquiprobable = params.minSpeedEquiprobable, speedProbabilities = speedProbabilities)
        pastObjects.append(obj)
    print('Saving user types')
    storage.setRoadUserTypes(databaseFilename, pastObjects)
