#!/usr/bin/env python

import sys

import matplotlib.mlab as pylab
import matplotlib.pyplot as plt
import numpy as np

import cv
import utils
import cvutils
import ubc_utils
import moving

# use something like getopt to manage arguments if necessary

if len(sys.argv) < 3:
    print('Usage: {0} <video-filename> <n-objects>'.format(sys.argv[0]))
    sys.exit()

if sys.argv[1].endswith('.avi'):
    videoFilenamePrefix = utils.removeExtension(sys.argv[1],'.')
else:
    videoFilenamePrefix = sys.argv[1]

objectNum = int(sys.argv[2])

objects = ubc_utils.loadTrajectories(videoFilenamePrefix+'-objects.txt', objectNum+1)
obj = objects[objectNum]
features = ubc_utils.loadTrajectories(videoFilenamePrefix+'-features.txt', max(obj.featureNumbers)+1)
h = np.loadtxt(videoFilenamePrefix+'-homography.txt')

invh = cvutils.invertHomography(h)

def computeGroundTrajectory(features, homography, timeInterval = None):
    '''Computes a trajectory for the set of features as the closes point to the ground
    using the homography in image space'''
    if not timeInterval:
        raise Exception('not implemented') # compute from the features
    
    yCoordinates = -np.ones((len(features),int(timeInterval.length())))
    for i,f in enumerate(features):
        traj = f.getPositions().asArray()
        imgTraj = cvutils.projectArray(homography, traj)
        yCoordinates[i,f.getFirstInstant()-timeInterval.first:f.getLastInstant()+1-timeInterval.first] = imgTraj[1,:]

    indices = np.argmax(yCoordinates,0)
    newTraj = moving.Trajectory()
    for j,idx in enumerate(indices):
        newTraj.addPosition(features[idx].getPositionAtInstant(j+timeInterval.first))
        #newVelocities.addPosition(features[obj.featureNumbers[idx]].getVelocityAtInstant(j+obj.getFirstInstant()))

    return newTraj

def computeMedianTrajectory(features, timeInterval = None):
    if not timeInterval:
        raise Exception('not implemented') # compute from the features
    
    newTraj = moving.Trajectory()
    for t in timeInterval:
        points = []
        for f in features:
            if f.existsAtInstant(t):
                points.append(f.getPositionAtInstant(t).aslist())
        med = np.median(np.array(points), 0)
        newTraj.addPositionXY(med[0], med[1])

    return newTraj

# TODO version median: conversion to large matrix will not work, have to do it frame by frame

def kalmanFilter(positions, velocities, processNoiseCov, measurementNoiseCov):
    kalman=cv.CreateKalman(6, 4)
    kalman.transition_matrix[0,2]=1
    kalman.transition_matrix[0,4]=1./2
    kalman.transition_matrix[1,3]=1
    kalman.transition_matrix[1,5]=1./2
    kalman.transition_matrix[2,4]=1
    kalman.transition_matrix[3,5]=1

    cv.SetIdentity(kalman.measurement_matrix, 1.)
    cv.SetIdentity(kalman.process_noise_cov, processNoiseCov)
    cv.SetIdentity(kalman.measurement_noise_cov, measurementNoiseCov)
    cv.SetIdentity(kalman.error_cov_post, 1.)

    p = positions[0]
    v = velocities[0]
    v2 = velocities[2]
    a = (v2-v).multiply(0.5)
    kalman.state_post[0,0]=p.x
    kalman.state_post[1,0]=p.y
    kalman.state_post[2,0]=v.x
    kalman.state_post[3,0]=v.y
    kalman.state_post[4,0]=a.x
    kalman.state_post[5,0]=a.y
    
    filteredPositions = moving.Trajectory()
    filteredVelocities = moving.Trajectory()
    measurement = cv.CreateMat(4,1,cv.CV_32FC1)
    for i in xrange(positions.length()):
        cv.KalmanPredict(kalman) # no control
        p = positions[i]
        v = velocities[i]
        measurement[0,0] = p.x
        measurement[1,0] = p.y
        measurement[2,0] = v.x
        measurement[3,0] = v.y
        cv.KalmanCorrect(kalman, measurement)
        filteredPositions.addPositionXY(kalman.state_post[0,0], kalman.state_post[1,0])
        filteredVelocities.addPositionXY(kalman.state_post[2,0], kalman.state_post[3,0])

    return (filteredPositions, filteredVelocities)

groundTrajectory = computeGroundTrajectory([features[i] for i in obj.featureNumbers], invh, obj.getTimeInterval())
(filteredPositions, filteredVelocities) = kalmanFilter(groundTrajectory, obj.getVelocities(), 0.1, 0.1)

#medianTrajectory = computeMedianTrajectory([features[i] for i in obj.featureNumbers], obj.getTimeInterval())

delta = []
for t in obj.getTimeInterval():
    p1 = obj.getPositionAtInstant(t)
    p2 = groundTrajectory[t-obj.getFirstInstant()]
    delta.append((p1-p2).aslist())

delta = np.median(delta, 0)

translated = moving.Trajectory()
for t in obj.getTimeInterval():
    p1 = obj.getPositionAtInstant(t)
    p1.x -= delta[0]
    p1.y -= delta[1]
    translated.addPosition(p1)

plt.clf()
obj.draw('rx-')
for fnum in obj.featureNumbers: features[fnum].draw()
groundTrajectory.draw('bx-')
filteredPositions.draw('gx-')
translated.draw('kx-')
#medianTrajectory.draw('kx-')
plt.axis('equal')
