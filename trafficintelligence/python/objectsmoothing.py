import storage
import moving
import utils

from math import atan2, degrees, sin, cos, pi
from numpy import median

import matplotlib.pyplot as plt


def findNearest(feat, featureSet, t, reverse=True):
    dist = {}
    for f in featureSet:
        if reverse:
            dist[f] = moving.Point.distanceNorm2(
                feat.getPositionAtInstant(t + 1), f.getPositionAtInstant(t))
        else:
            dist[f] = moving.Point.distanceNorm2(
                feat.getPositionAtInstant(t - 1), f.getPositionAtInstant(t))
    return min(dist, key=dist.get)  # = utils.argmaxDict(dist)


def getFeatures(obj, featureID):
    currentFeature = obj.getFeature(featureID)
    first = currentFeature.getFirstInstant()
    last = currentFeature.getLastInstant()
    featureList = [[currentFeature, first, last, moving.Point(0, 0)]]
    # find the features to fill in the beginning of the object existence
    while first != obj.getFirstInstant():
        delta = featureList[-1][3]
        featureSet = [
            f for f in obj.getFeatures() if f.existsAtInstant(first - 1)]
        feat = findNearest(currentFeature, featureSet, first - 1, reverse=True)
        if feat.existsAtInstant(first):
            featureList.append([feat, feat.getFirstInstant(
            ), first - 1, (currentFeature.getPositionAtInstant(first) - feat.getPositionAtInstant(first)) + delta])
        else:
            featureList.append([feat, feat.getFirstInstant(
            ), first - 1, (currentFeature.getPositionAtInstant(first) - feat.getPositionAtInstant(first - 1)) + delta])
        currentFeature = feat
        first = feat.getFirstInstant()
    # find the features to fill in the end of the object existence
    delta = moving.Point(0, 0)
    currentFeature = obj.getFeature(featureID)  # need to reinitialize
    while last != obj.getLastInstant():
        featureSet = [
            f for f in obj.getFeatures() if f.existsAtInstant(last + 1)]
        feat = findNearest(currentFeature, featureSet, last + 1, reverse=False)
        if feat.existsAtInstant(last):
            featureList.append([feat, last + 1, feat.getLastInstant(
            ), (currentFeature.getPositionAtInstant(last) - feat.getPositionAtInstant(last)) + delta])
        else:
            featureList.append([feat, last + 1, feat.getLastInstant(
            ), (currentFeature.getPositionAtInstant(last) - feat.getPositionAtInstant(last + 1)) + delta])
        currentFeature = feat
        last = feat.getLastInstant()
        delta = featureList[-1][3]
    return featureList


def buildFeature(obj, featureID, num=1):
    featureList = getFeatures(obj, featureID)
    tmp = {}
    delta = {}
    for i in featureList:
        for t in xrange(i[1], i[2] + 1):
            tmp[t] = [i[0], i[3]]
    newTraj = moving.Trajectory()

    for instant in obj.getTimeInterval():
        newTraj.addPosition(
            tmp[instant][0].getPositionAtInstant(instant) + tmp[instant][1])
    newFeature = moving.MovingObject(
        num, timeInterval=obj.getTimeInterval(), positions=newTraj)
    return newFeature


def getBearing(p1, p2, p3):
    angle = degrees(atan2(p3.y - p1.y, p3.x - p1.x))
    bearing1 = (90 - angle) % 360
    angle2 = degrees(atan2(p2.y - p1.y, p2.x - p1.x))
    bearing2 = (90 - angle2) % 360
    dist = moving.Point.distanceNorm2(p1, p2)
    return [dist, bearing1, bearing2, bearing2 - bearing1]

# Quantitative analysis "CSJ" functions


# compute velocities from positions
def computeVelocities(obj, smoothing=True, halfWidth=3):
    velocities = {}
    for i in list(obj.timeInterval)[:-1]:
        p1 = obj.getPositionAtInstant(i)
        p2 = obj.getPositionAtInstant(i + 1)
        velocities[i] = p2 - p1
    # duplicate last point
    velocities[obj.getLastInstant()] = velocities[obj.getLastInstant() - 1]
    if smoothing:
        velX = [velocities[y].aslist()[0] for y in sorted(velocities.keys())]
        velY = [velocities[y].aslist()[1] for y in sorted(velocities.keys())]
        v1 = list(utils.filterMovingWindow(velX, halfWidth))
        v2 = list(utils.filterMovingWindow(velY, halfWidth))
        smoothedVelocity = {}
        for t, i in enumerate(sorted(velocities.keys())):
            smoothedVelocity[i] = moving.Point(v1[t], v2[t])
        velocities = smoothedVelocity
    return velocities


def computeAcceleration(obj, fromPosition=True):
    acceleration = {}
    if fromPosition:
        velocities = computeVelocities(obj, False, 1)
        for i in sorted(velocities.keys()):
            if i != sorted(velocities.keys())[-1]:
                acceleration[i] = velocities[i + 1] - velocities[i]
    else:
        for i in list(obj.timeInterval)[:-1]:
            v1 = obj.getVelocityAtInstant(i)
            v2 = obj.getVelocityAtInstant(i + 1)
            acceleration[i] = v2 - v1
    return acceleration


def computeJerk(obj, fromPosition=True):
    jerk = {}
    acceleration = computeAcceleration(obj, fromPosition=fromPosition)
    for i in sorted(acceleration.keys()):
        if i != sorted(acceleration.keys())[-1]:
            jerk[i] = (acceleration[i + 1] - acceleration[i]).norm2()
    return jerk


def sumSquaredJerk(obj, fromPosition=True):
    jerk = computeJerk(obj, fromPosition=fromPosition)
    t = 0
    for i in sorted(jerk.keys()):
        t += jerk[i] * jerk[i]
    return t


def smoothObjectTrajectory(obj, featureID, newNum, smoothing=False, halfWidth=3, create=False):
    results = []
    bearing = {}
    if create:
        feature = buildFeature(obj, featureID, num=1)  # why num=1
    else:
        feature = obj.getFeature(featureID)
    for t in feature.getTimeInterval():
        p1 = feature.getPositionAtInstant(t)
        p2 = obj.getPositionAtInstant(t)
        if t != feature.getLastInstant():
            p3 = feature.getPositionAtInstant(t + 1)
        else:
            p1 = feature.getPositionAtInstant(t - 1)
            p3 = feature.getPositionAtInstant(t)
        bearing[t] = getBearing(p1, p2, p3)[1]
        results.append(getBearing(p1, p2, p3))

    medianResults = median(results, 0)
    dist = medianResults[0]
    angle = medianResults[3]

    for i in sorted(bearing.keys()):
        bearing[i] = bearing[i] + angle

    if smoothing:
        bearingInput = []
        for i in sorted(bearing.keys()):
            bearingInput.append(bearing[i])
        import utils
        bearingOut = utils.filterMovingWindow(bearingInput, halfWidth)
        for t, i in enumerate(sorted(bearing.keys())):
            bearing[i] = bearingOut[t]

        # solve a smoothing problem in case of big drop in computing bearing
        # (0,360)
        for t, i in enumerate(sorted(bearing.keys())):
            if i != max(bearing.keys()) and abs(bearingInput[t] - bearingInput[t + 1]) >= 340:
                for x in xrange(max(i - halfWidth, min(bearing.keys())), min(i + halfWidth, max(bearing.keys())) + 1):
                    bearing[x] = bearingInput[t - i + x]

    translated = moving.Trajectory()
    for t in feature.getTimeInterval():
        p1 = feature.getPositionAtInstant(t)
        p1.x = p1.x + dist * sin(bearing[t] * pi / 180)
        p1.y = p1.y + dist * cos(bearing[t] * pi / 180)
        translated.addPosition(p1)

    # modify first and last un-smoothed positions (half width)
    if smoothing:
        d1 = translated[halfWidth] - feature.positions[halfWidth]
        d2 = translated[-halfWidth - 1] - feature.positions[-halfWidth - 1]
        for i in xrange(halfWidth):
            p1 = feature.getPositionAt(i) + d1
            p2 = feature.getPositionAt(-i - 1) + d2
            translated.setPosition(i, p1)
            translated.setPosition(-i - 1, p2)

    newObj = moving.MovingObject(
        newNum, timeInterval=feature.getTimeInterval(), positions=translated)
    return newObj


def smoothObject(obj, newNum, minLengthParam=0.7, smoothing=False, plotResults=True, halfWidth=3, _computeVelocities=True, optimize=True, create=False):
    '''Computes a smoother trajectory for the object
    and optionnally smoother velocities

    The object should have its features in obj.features
    TODO: check whether features are necessary'''
    if not obj.hasFeatures():
        print('Object {} has an empty list of features: please load and add them using obj.setFeatures(features)'.format(
            obj.getNum()))
        from sys import exit
        exit()

    featureList = [i for i, f in enumerate(
        obj.getFeatures()) if f.length() >= minLengthParam * obj.length()]
    if featureList == []:
        featureList.append(
            utils.argmaxDict({i: f.length() for i, f in enumerate(obj.getFeatures())}))
        create = True
    newObjects = []
    # featureID should be the index in the list of obj.features
    for featureID in featureList:
        newObjects.append(smoothObjectTrajectory(
            obj, featureID, newNum, smoothing=smoothing, halfWidth=halfWidth, create=create))

    newTranslated = moving.Trajectory()
    newInterval = []
    for t in obj.getTimeInterval():
        xCoord = []
        yCoord = []
        for i in newObjects:
            if i.existsAtInstant(t):
                p1 = i.getPositionAtInstant(t)
                xCoord.append(p1.x)
                yCoord.append(p1.y)
        if xCoord != []:
            tmp = moving.Point(median(xCoord), median(yCoord))
            newInterval.append(t)
            newTranslated.addPosition(tmp)

    newObj = moving.MovingObject(newNum, timeInterval=moving.TimeInterval(
        min(newInterval), max(newInterval)), positions=newTranslated)

    if _computeVelocities:
        tmpTraj = moving.Trajectory()
        velocities = computeVelocities(newObj, True, 5)
        for i in sorted(velocities.keys()):
            tmpTraj.addPosition(velocities[i])
        newObj.velocities = tmpTraj
    else:
        newObj.velocities = obj.velocities

    if optimize:
        csj1 = sumSquaredJerk(obj, fromPosition=True)
        csj2 = sumSquaredJerk(newObj, fromPosition=True)
        if csj1 < csj2:
            newObj = obj
            newObj.velocities = obj.velocities
        if _computeVelocities and csj1 >= csj2:
            csj3 = sumSquaredJerk(obj, fromPosition=False)
            csj4 = sumSquaredJerk(newObj, fromPosition=False)
            if csj4 <= csj3:
                newObj.velocities = obj.velocities

    newObj.featureNumbers = obj.featureNumbers
    newObj.features = obj.getFeatures()
    newObj.userType = obj.userType

    if plotResults:
        plt.figure()
        plt.title('objects_id = {}'.format(obj.num))
        for i in featureList:
            obj.getFeature(i).plot('cx-')
        obj.plot('rx-')
        newObj.plot('gx-')
    return newObj
