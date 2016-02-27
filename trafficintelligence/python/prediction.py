#! /usr/bin/env python
'''Library for motion prediction methods'''

import moving
from utils import LCSS

import math
import random
import numpy as np
from multiprocessing import Pool


class PredictedTrajectory(object):
    '''Class for predicted trajectories with lazy evaluation
    if the predicted position has not been already computed, compute it

    it should also have a probability'''

    def __init__(self):
        self.probability = 0.
        self.predictedPositions = {}
        self.predictedSpeedOrientations = {}
        # self.collisionPoints = {}
        # self.crossingZones = {}

    def predictPosition(self, nTimeSteps):
        if nTimeSteps > 0 and not nTimeSteps in self.predictedPositions.keys():
            self.predictPosition(nTimeSteps - 1)
            self.predictedPositions[nTimeSteps], self.predictedSpeedOrientations[nTimeSteps] = moving.predictPosition(
                self.predictedPositions[
                    nTimeSteps - 1], self.predictedSpeedOrientations[nTimeSteps - 1],
                self.getControl(), self.maxSpeed)
        return self.predictedPositions[nTimeSteps]

    def getPredictedTrajectory(self):
        return moving.Trajectory.fromPointList(self.predictedPositions.values())

    def getPredictedSpeeds(self):
        return [so.norm for so in self.predictedSpeedOrientations.values()]

    def plot(self, options='', withOrigin=False, timeStep=1, **kwargs):
        self.getPredictedTrajectory().plot(
            options, withOrigin, timeStep, **kwargs)


class PredictedTrajectoryConstant(PredictedTrajectory):
    '''Predicted trajectory at constant speed or acceleration
    TODO generalize by passing a series of velocities/accelerations'''

    def __init__(self, initialPosition, initialVelocity, control=moving.NormAngle(0, 0), probability=1., maxSpeed=None):
        self.control = control
        self.maxSpeed = maxSpeed
        self.probability = probability
        self.predictedPositions = {0: initialPosition}
        self.predictedSpeedOrientations = {
            0: moving.NormAngle.fromPoint(initialVelocity)}

    def getControl(self):
        return self.control


def findNearestParams(initialPosition, prototypeTrajectory):
    ''' nearest parameters are the index of minDistance and the orientation  '''
    distances = []
    for position in prototypeTrajectory.positions:
        distances.append(moving.Point.distanceNorm2(initialPosition, position))
    minDistanceIndex = np.argmin(distances)
    return minDistanceIndex, moving.NormAngle.fromPoint(prototypeTrajectory.velocities[minDistanceIndex]).angle


class PredictedTrajectoryPrototype(PredictedTrajectory):
    '''Predicted trajectory that follows a prototype trajectory
    The prototype is in the format of a moving.Trajectory: it could be
    1. an observed trajectory (extracted from video)
    2. a generic polyline (eg the road centerline) that a vehicle is supposed to follow

    Prediction can be done
    1. at constant speed (the instantaneous user speed)
    2. following the trajectory path, at the speed of the user
    (applying a constant ratio equal 
    to the ratio of the user instantaneous speed and the trajectory closest speed)'''

    def __init__(self, initialPosition, initialVelocity, prototypeTrajectory, constantSpeed=True, probability=1.):
        self.prototypeTrajectory = prototypeTrajectory
        self.constantSpeed = constantSpeed
        self.probability = probability
        self.predictedPositions = {0: initialPosition}
        self.predictedSpeedOrientations = {0: moving.NormAngle(moving.NormAngle.fromPoint(initialVelocity).norm,
                                                               findNearestParams(initialPosition, prototypeTrajectory)[
                                                                   1])}  # moving.NormAngle.fromPoint(initialVelocity)}

    def predictPosition(self, nTimeSteps):
        if nTimeSteps > 0 and not nTimeSteps in self.predictedPositions.keys():
            if self.constantSpeed:
                # calculate cumulative distance
                # moving.NormAngle.fromPoint(initialVelocity).norm
                speedNorm = self.predictedSpeedOrientations[0].norm
                anglePrototype = findNearestParams(
                    self.predictedPositions[nTimeSteps - 1], self.prototypeTrajectory)[1]
                self.predictedSpeedOrientations[
                    nTimeSteps] = moving.NormAngle(speedNorm, anglePrototype)
                self.predictedPositions[nTimeSteps], tmp = moving.predictPosition(
                    self.predictedPositions[
                        nTimeSteps - 1], self.predictedSpeedOrientations[nTimeSteps - 1],
                    moving.NormAngle(0, 0), None)

            else:  # see c++ code, calculate ratio
                speedNorm = self.predictedSpeedOrientations[0].norm
                instant = findNearestParams(
                    self.predictedPositions[0], self.prototypeTrajectory)[0]
                prototypeSpeeds = self.prototypeTrajectory.getSpeeds()[
                    instant:]
                ratio = float(speedNorm) / prototypeSpeeds[0]
                resampledSpeeds = [sp * ratio for sp in prototypeSpeeds]
                anglePrototype = findNearestParams(
                    self.predictedPositions[nTimeSteps - 1], self.prototypeTrajectory)[1]
                if nTimeSteps < len(resampledSpeeds):
                    self.predictedSpeedOrientations[nTimeSteps] = moving.NormAngle(resampledSpeeds[nTimeSteps],
                                                                                   anglePrototype)
                    self.predictedPositions[nTimeSteps], tmp = moving.predictPosition(
                        self.predictedPositions[
                            nTimeSteps - 1], self.predictedSpeedOrientations[nTimeSteps - 1],
                        moving.NormAngle(0, 0), None)
                else:
                    self.predictedSpeedOrientations[nTimeSteps] = moving.NormAngle(
                        resampledSpeeds[-1], anglePrototype)
                    self.predictedPositions[nTimeSteps], tmp = moving.predictPosition(
                        self.predictedPositions[
                            nTimeSteps - 1], self.predictedSpeedOrientations[nTimeSteps - 1],
                        moving.NormAngle(0, 0), None)

        return self.predictedPositions[nTimeSteps]


class PredictedTrajectoryRandomControl(PredictedTrajectory):
    '''Random vehicle control: suitable for normal adaptation'''

    def __init__(self, initialPosition, initialVelocity, accelerationDistribution, steeringDistribution, probability=1.,
                 maxSpeed=None):
        '''Constructor
        accelerationDistribution and steeringDistribution are distributions 
        that return random numbers drawn from them'''
        self.accelerationDistribution = accelerationDistribution
        self.steeringDistribution = steeringDistribution
        self.maxSpeed = maxSpeed
        self.probability = probability
        self.predictedPositions = {0: initialPosition}
        self.predictedSpeedOrientations = {
            0: moving.NormAngle.fromPoint(initialVelocity)}

    def getControl(self):
        return moving.NormAngle(self.accelerationDistribution(), self.steeringDistribution())


class SafetyPoint(moving.Point):
    '''Can represent a collision point or crossing zone 
    with respective safety indicator, TTC or pPET'''

    def __init__(self, p, probability=1., indicator=-1):
        self.x = p.x
        self.y = p.y
        self.probability = probability
        self.indicator = indicator

    def __str__(self):
        return '{0} {1} {2} {3}'.format(self.x, self.y, self.probability, self.indicator)

    @staticmethod
    def save(out, points, predictionInstant, objNum1, objNum2):
        for p in points:
            out.write('{0} {1} {2} {3}\n'.format(
                objNum1, objNum2, predictionInstant, p))

    @staticmethod
    def computeExpectedIndicator(points):
        return np.sum([p.indicator * p.probability for p in points]) / sum([p.probability for p in points])


def computeCollisionTime(predictedTrajectory1, predictedTrajectory2, collisionDistanceThreshold, timeHorizon):
    '''Computes the first instant 
    at which two predicted trajectories are within some distance threshold
    Computes all the times including timeHorizon

    User has to check the first variable collision to know about a collision'''
    t = 1
    p1 = predictedTrajectory1.predictPosition(t)
    p2 = predictedTrajectory2.predictPosition(t)
    collision = (p1 - p2).norm2() <= collisionDistanceThreshold
    while t < timeHorizon and not collision:
        t += 1
        p1 = predictedTrajectory1.predictPosition(t)
        p2 = predictedTrajectory2.predictPosition(t)
        collision = (p1 - p2).norm2() <= collisionDistanceThreshold
    return collision, t, p1, p2


def savePredictedTrajectoriesFigure(currentInstant, obj1, obj2, predictedTrajectories1, predictedTrajectories2,
                                    timeHorizon):
    from matplotlib.pyplot import figure, axis, title, close, savefig
    figure()
    for et in predictedTrajectories1:
        et.predictPosition(int(np.round(timeHorizon)))
        et.plot('rx')

    for et in predictedTrajectories2:
        et.predictPosition(int(np.round(timeHorizon)))
        et.plot('bx')
    obj1.plot('r')
    obj2.plot('b')
    title('instant {0}'.format(currentInstant))
    axis('equal')
    savefig('predicted-trajectories-t-{0}.png'.format(currentInstant))
    close()


def calculateProbability(nMatching, similarity, objects):
    sumFrequencies = sum([nMatching[p] for p in similarity.keys()])
    prototypeProbability = {}
    for i in similarity.keys():
        prototypeProbability[i] = similarity[
            i] * float(nMatching[i]) / sumFrequencies
    sumProbabilities = sum([prototypeProbability[p]
                            for p in prototypeProbability.keys()])
    probabilities = {}
    for i in prototypeProbability.keys():
        probabilities[objects[i]] = float(
            prototypeProbability[i]) / sumProbabilities
    return probabilities


def findPrototypes(prototypes, nMatching, objects, route, partialObjPositions, noiseEntryNums, noiseExitNums,
                   minSimilarity=0.1, mostMatched=None, spatialThreshold=1.0, delta=180):
    ''' behaviour prediction first step'''
    if route[0] not in noiseEntryNums:
        prototypesRoutes = [
            x for x in sorted(prototypes.keys()) if route[0] == x[0]]
    elif route[1] not in noiseExitNums:
        prototypesRoutes = [
            x for x in sorted(prototypes.keys()) if route[1] == x[1]]
    else:
        prototypesRoutes = [x for x in sorted(prototypes.keys())]
    lcss = LCSS(similarityFunc=lambda x, y: (
        distanceForLCSS(x, y) <= spatialThreshold), delta=delta)
    similarity = {}
    for y in prototypesRoutes:
        if y in prototypes.keys():
            prototypesIDs = prototypes[y]
            for x in prototypesIDs:
                s = lcss.computeNormalized(
                    partialObjPositions, objects[x].positions)
                if s >= minSimilarity:
                    similarity[x] = s

    if mostMatched == None:
        probabilities = calculateProbability(nMatching, similarity, objects)
        return probabilities
    else:
        mostMatchedValues = sorted(
            similarity.values(), reverse=True)[:mostMatched]
        keys = [
            k for k in similarity.keys() if similarity[k] in mostMatchedValues]
        newSimilarity = {}
        for i in keys:
            newSimilarity[i] = similarity[i]
        probabilities = calculateProbability(nMatching, newSimilarity, objects)
        return probabilities


def findPrototypesSpeed(prototypes, secondStepPrototypes, nMatching, objects, route, partialObjPositions,
                        noiseEntryNums, noiseExitNums, minSimilarity=0.1, mostMatched=None, useDestination=True,
                        spatialThreshold=1.0, delta=180):
    if useDestination:
        prototypesRoutes = [route]
    else:
        if route[0] not in noiseEntryNums:
            prototypesRoutes = [
                x for x in sorted(prototypes.keys()) if route[0] == x[0]]
        elif route[1] not in noiseExitNums:
            prototypesRoutes = [
                x for x in sorted(prototypes.keys()) if route[1] == x[1]]
        else:
            prototypesRoutes = [x for x in sorted(prototypes.keys())]
    lcss = LCSS(similarityFunc=lambda x, y: (
        distanceForLCSS(x, y) <= spatialThreshold), delta=delta)
    similarity = {}
    for y in prototypesRoutes:
        if y in prototypes.keys():
            prototypesIDs = prototypes[y]
            for x in prototypesIDs:
                s = lcss.computeNormalized(
                    partialObjPositions, objects[x].positions)
                if s >= minSimilarity:
                    similarity[x] = s

    newSimilarity = {}
    for i in similarity.keys():
        if i in secondStepPrototypes.keys():
            for j in secondStepPrototypes[i]:
                newSimilarity[j] = similarity[i]
    probabilities = calculateProbability(nMatching, newSimilarity, objects)
    return probabilities


def getPrototypeTrajectory(obj, route, currentInstant, prototypes, secondStepPrototypes, nMatching, objects,
                           noiseEntryNums, noiseExitNums, minSimilarity=0.1, mostMatched=None, useDestination=True,
                           useSpeedPrototype=True):
    partialInterval = moving.Interval(obj.getFirstInstant(), currentInstant)
    partialObjPositions = obj.getObjectInTimeInterval(
        partialInterval).positions
    if useSpeedPrototype:
        prototypeTrajectories = findPrototypesSpeed(prototypes, secondStepPrototypes, nMatching, objects, route,
                                                    partialObjPositions, noiseEntryNums, noiseExitNums, minSimilarity,
                                                    mostMatched, useDestination)
    else:
        prototypeTrajectories = findPrototypes(prototypes, nMatching, objects, route, partialObjPositions,
                                               noiseEntryNums, noiseExitNums, minSimilarity, mostMatched)
    return prototypeTrajectories


def computeCrossingsCollisionsAtInstant(predictionParams, currentInstant, obj1, obj2, collisionDistanceThreshold,
                                        timeHorizon, computeCZ=False, debug=False, usePrototypes=False, route1=(
                                            -1, -1),
                                        route2=(-1, -1), prototypes={}, secondStepPrototypes={}, nMatching={},
                                        objects=[], noiseEntryNums=[
                                        ], noiseExitNums=[], minSimilarity=0.1,
                                        mostMatched=None, useDestination=True, useSpeedPrototype=True):
    '''returns the lists of collision points and crossing zones'''
    if usePrototypes:
        prototypeTrajectories1 = getPrototypeTrajectory(obj1, route1, currentInstant, prototypes, secondStepPrototypes,
                                                        nMatching, objects, noiseEntryNums, noiseExitNums,
                                                        minSimilarity, mostMatched, useDestination, useSpeedPrototype)
        prototypeTrajectories2 = getPrototypeTrajectory(obj2, route2, currentInstant, prototypes, secondStepPrototypes,
                                                        nMatching, objects, noiseEntryNums, noiseExitNums,
                                                        minSimilarity, mostMatched, useDestination, useSpeedPrototype)
        predictedTrajectories1 = predictionParams.generatePredictedTrajectories(obj1, currentInstant,
                                                                                prototypeTrajectories1)
        predictedTrajectories2 = predictionParams.generatePredictedTrajectories(obj2, currentInstant,
                                                                                prototypeTrajectories2)
    else:
        predictedTrajectories1 = predictionParams.generatePredictedTrajectories(
            obj1, currentInstant)
        predictedTrajectories2 = predictionParams.generatePredictedTrajectories(
            obj2, currentInstant)

    collisionPoints = []
    crossingZones = []
    for et1 in predictedTrajectories1:
        for et2 in predictedTrajectories2:
            collision, t, p1, p2 = computeCollisionTime(
                et1, et2, collisionDistanceThreshold, timeHorizon)

            if collision:
                collisionPoints.append(
                    SafetyPoint((p1 + p2).multiply(0.5), et1.probability * et2.probability, t))
            elif computeCZ:  # check if there is a crossing zone
                # TODO? zone should be around the points at which the traj are the closest
                # look for CZ at different times, otherwise it would be a collision
                # an approximation would be to look for close points at
                # different times, ie the complementary of collision points
                cz = None
                t1 = 0
                while not cz and t1 < timeHorizon:  # t1 <= timeHorizon-1
                    t2 = 0
                    while not cz and t2 < timeHorizon:
                        # if (et1.predictPosition(t1)-et2.predictPosition(t2)).norm2() < collisionDistanceThreshold:
                        #    cz = (et1.predictPosition(t1)+et2.predictPosition(t2)).multiply(0.5)
                        cz = moving.segmentIntersection(et1.predictPosition(t1), et1.predictPosition(t1 + 1),
                                                        et2.predictPosition(t2), et2.predictPosition(t2 + 1))
                        if cz is not None:
                            deltaV = (et1.predictPosition(t1) - et1.predictPosition(t1 + 1) - et2.predictPosition(
                                t2) + et2.predictPosition(t2 + 1)).norm2()
                            crossingZones.append(SafetyPoint(cz, et1.probability * et2.probability, abs(t1 - t2) - (
                                float(collisionDistanceThreshold) / deltaV)))
                        t2 += 1
                    t1 += 1

    if debug:
        savePredictedTrajectoriesFigure(currentInstant, obj1, obj2, predictedTrajectories1, predictedTrajectories2,
                                        timeHorizon)

    return currentInstant, collisionPoints, crossingZones


class PredictionParameters(object):
    def __init__(self, name, maxSpeed):
        self.name = name
        self.maxSpeed = maxSpeed

    def __str__(self):
        return '{0} {1}'.format(self.name, self.maxSpeed)

    def generatePredictedTrajectories(self, obj, instant):
        return []

    def computeCrossingsCollisionsAtInstant(self, currentInstant, obj1, obj2, collisionDistanceThreshold, timeHorizon,
                                            computeCZ=False, debug=False, usePrototypes=False, route1=(
                                                -1, -1),
                                            route2=(-1, -1), prototypes={}, secondStepPrototypes={}, nMatching={},
                                            objects=[], noiseEntryNums=[
                                            ], noiseExitNums=[], minSimilarity=0.1,
                                            mostMatched=None, useDestination=True, useSpeedPrototype=True):
        return computeCrossingsCollisionsAtInstant(self, currentInstant, obj1, obj2, collisionDistanceThreshold,
                                                   timeHorizon, computeCZ, debug, usePrototypes, route1, route2,
                                                   prototypes, secondStepPrototypes, nMatching, objects, noiseEntryNums,
                                                   noiseExitNums, minSimilarity, mostMatched, useDestination,
                                                   useSpeedPrototype)

    def computeCrossingsCollisions(self, obj1, obj2, collisionDistanceThreshold, timeHorizon, computeCZ=False,
                                   debug=False, timeInterval=None, nProcesses=1, usePrototypes=False, route1=(-1, -1),
                                   route2=(-1, -1), prototypes={}, secondStepPrototypes={}, nMatching={}, objects=[],
                                   noiseEntryNums=[], noiseExitNums=[], minSimilarity=0.1, mostMatched=None,
                                   useDestination=True, useSpeedPrototype=True, acceptPartialLength=30, step=1):
        # def computeCrossingsCollisions(predictionParams, obj1, obj2,
        # collisionDistanceThreshold, timeHorizon, computeCZ = False, debug =
        # False, timeInterval = None,nProcesses = 1, usePrototypes =
        # False,route1=
        # (-1,-1),route2=(-1,-1),prototypes={},secondStepPrototypes={},nMatching={},objects=[],noiseEntryNums=[],noiseExitNums=[],minSimilarity=0.1,mostMatched=None,useDestination=True,useSpeedPrototype=True,acceptPartialLength=30,
        # step=1):
        '''Computes all crossing and collision points at each common instant for two road users. '''
        collisionPoints = {}
        crossingZones = {}
        if timeInterval:
            commonTimeInterval = timeInterval
        else:
            commonTimeInterval = obj1.commonTimeInterval(obj2)
        if nProcesses == 1:
            if usePrototypes:
                firstInstant = next((x for x in xrange(commonTimeInterval.first, commonTimeInterval.last) if
                                     x - obj1.getFirstInstant() >= acceptPartialLength and x - obj2.getFirstInstant() >= acceptPartialLength),
                                    commonTimeInterval.last)
                commonTimeIntervalList1 = range(firstInstant,
                                                commonTimeInterval.last - 1)  # do not look at the 1 last position/velocities, often with errors
                commonTimeIntervalList2 = range(firstInstant, commonTimeInterval.last - 1,
                                                step)  # do not look at the 1 last position/velocities, often with errors
                for i in commonTimeIntervalList2:
                    i, cp, cz = self.computeCrossingsCollisionsAtInstant(i, obj1, obj2, collisionDistanceThreshold,
                                                                         timeHorizon, computeCZ, debug, usePrototypes,
                                                                         route1, route2, prototypes,
                                                                         secondStepPrototypes, nMatching, objects,
                                                                         noiseEntryNums, noiseExitNums, minSimilarity,
                                                                         mostMatched, useDestination, useSpeedPrototype)
                    if len(cp) != 0:
                        collisionPoints[i] = cp
                    if len(cz) != 0:
                        crossingZones[i] = cz
                if collisionPoints != {} or crossingZones != {}:
                    for i in commonTimeIntervalList1:
                        if i not in commonTimeIntervalList2:
                            i, cp, cz = self.computeCrossingsCollisionsAtInstant(i, obj1, obj2,
                                                                                 collisionDistanceThreshold,
                                                                                 timeHorizon, computeCZ, debug,
                                                                                 usePrototypes, route1, route2,
                                                                                 prototypes, secondStepPrototypes,
                                                                                 nMatching, objects, noiseEntryNums,
                                                                                 noiseExitNums, minSimilarity,
                                                                                 mostMatched, useDestination,
                                                                                 useSpeedPrototype)
                            if len(cp) != 0:
                                collisionPoints[i] = cp
                            if len(cz) != 0:
                                crossingZones[i] = cz
            else:
                for i in list(commonTimeInterval)[
                        :-1]:  # do not look at the 1 last position/velocities, often with errors
                    i, cp, cz = self.computeCrossingsCollisionsAtInstant(i, obj1, obj2, collisionDistanceThreshold,
                                                                         timeHorizon, computeCZ, debug, usePrototypes,
                                                                         route1, route2, prototypes,
                                                                         secondStepPrototypes, nMatching, objects,
                                                                         noiseEntryNums, noiseExitNums, minSimilarity,
                                                                         mostMatched, useDestination, useSpeedPrototype)
                    if len(cp) != 0:
                        collisionPoints[i] = cp
                    if len(cz) != 0:
                        crossingZones[i] = cz
        else:
            pool = Pool(processes=nProcesses)
            jobs = [pool.apply_async(computeCrossingsCollisionsAtInstant, args=(
                self, i, obj1, obj2, collisionDistanceThreshold, timeHorizon, computeCZ, debug, usePrototypes, route1,
                route2, prototypes, secondStepPrototypes, nMatching, objects, noiseEntryNums, noiseExitNums, minSimilarity,
                mostMatched, useDestination, useSpeedPrototype)) for i in list(commonTimeInterval)[:-1]]
            # results = [j.get() for j in jobs]
            # results.sort()
            for j in jobs:
                i, cp, cz = j.get()
                # if len(cp) != 0 or len(cz) != 0:
                if len(cp) != 0:
                    collisionPoints[i] = cp
                if len(cz) != 0:
                    crossingZones[i] = cz
            pool.close()
        return collisionPoints, crossingZones

    # return computeCrossingsCollisions(self, obj1, obj2,
    # collisionDistanceThreshold, timeHorizon, computeCZ, debug, timeInterval,
    # nProcesses,usePrototypes,route1,route2,prototypes,secondStepPrototypes,nMatching,objects,noiseEntryNums,noiseExitNums,minSimilarity,mostMatched,useDestination,useSpeedPrototype,acceptPartialLength,
    # step)

    def computeCollisionProbability(self, obj1, obj2, collisionDistanceThreshold, timeHorizon, debug=False,
                                    timeInterval=None):
        '''Computes only collision probabilities
        Returns for each instant the collision probability and number of samples drawn'''
        collisionProbabilities = {}
        if timeInterval:
            commonTimeInterval = timeInterval
        else:
            commonTimeInterval = obj1.commonTimeInterval(obj2)
        for i in list(commonTimeInterval)[:-1]:
            nCollisions = 0
            predictedTrajectories1 = self.generatePredictedTrajectories(
                obj1, i)
            predictedTrajectories2 = self.generatePredictedTrajectories(
                obj2, i)
            for et1 in predictedTrajectories1:
                for et2 in predictedTrajectories2:
                    collision, t, p1, p2 = computeCollisionTime(
                        et1, et2, collisionDistanceThreshold, timeHorizon)
                    if collision:
                        nCollisions += 1
            # take into account probabilities ??
            nSamples = float(
                len(predictedTrajectories1) * len(predictedTrajectories2))
            collisionProbabilities[i] = [
                nSamples, float(nCollisions) / nSamples]

            if debug:
                savePredictedTrajectoriesFigure(i, obj1, obj2, predictedTrajectories1, predictedTrajectories2,
                                                timeHorizon)

        return collisionProbabilities


class ConstantPredictionParameters(PredictionParameters):
    def __init__(self, maxSpeed):
        PredictionParameters.__init__(self, 'constant velocity', maxSpeed)

    def generatePredictedTrajectories(self, obj, instant):
        return [PredictedTrajectoryConstant(obj.getPositionAtInstant(instant), obj.getVelocityAtInstant(instant),
                                            maxSpeed=self.maxSpeed)]


class NormalAdaptationPredictionParameters(PredictionParameters):
    def __init__(self, maxSpeed, nPredictedTrajectories, accelerationDistribution, steeringDistribution,
                 useFeatures=False):
        '''An example of acceleration and steering distributions is
        lambda: random.triangular(-self.maxAcceleration, self.maxAcceleration, 0.)
        '''
        if useFeatures:
            name = 'point set normal adaptation'
        else:
            name = 'normal adaptation'
        PredictionParameters.__init__(self, name, maxSpeed)
        self.nPredictedTrajectories = nPredictedTrajectories
        self.useFeatures = useFeatures
        self.accelerationDistribution = accelerationDistribution
        self.steeringDistribution = steeringDistribution

    def __str__(self):
        return PredictionParameters.__str__(self) + ' {0} {1} {2}'.format(self.nPredictedTrajectories,
                                                                          self.maxAcceleration,
                                                                          self.maxSteering)

    def generatePredictedTrajectories(self, obj, instant):
        predictedTrajectories = []
        if self.useFeatures and obj.hadFeatures():
            features = [
                f for f in obj.getFeatures() if f.existsAtInstant(instant)]
            positions = [f.getPositionAtInstant(instant) for f in features]
            velocities = [f.getVelocityAtInstant(instant) for f in features]
        else:
            positions = [obj.getPositionAtInstant(instant)]
            velocities = [obj.getVelocityAtInstant(instant)]
        for i in xrange(self.nPredictedTrajectories):
            for initialPosition, initialVelocity in zip(positions, velocities):
                predictedTrajectories.append(PredictedTrajectoryRandomControl(initialPosition,
                                                                              initialVelocity,
                                                                              self.accelerationDistribution,
                                                                              self.steeringDistribution,
                                                                              maxSpeed=self.maxSpeed))
        return predictedTrajectories


class PointSetPredictionParameters(PredictionParameters):
    # todo generate several trajectories with normal adaptatoins from each
    # position (feature)
    def __init__(self, maxSpeed):
        PredictionParameters.__init__(self, 'point set', maxSpeed)
        # self.nPredictedTrajectories = nPredictedTrajectories

    def generatePredictedTrajectories(self, obj, instant):
        predictedTrajectories = []
        if obj.hasFeatures():
            features = [
                f for f in obj.getFeatures() if f.existsAtInstant(instant)]
            positions = [f.getPositionAtInstant(instant) for f in features]
            velocities = [f.getVelocityAtInstant(instant) for f in features]
            # for i in xrange(self.nPredictedTrajectories):
            for initialPosition, initialVelocity in zip(positions, velocities):
                predictedTrajectories.append(PredictedTrajectoryConstant(initialPosition, initialVelocity,
                                                                         maxSpeed=self.maxSpeed))
            return predictedTrajectories
        else:
            print('Object {} has no features'.format(obj.getNum()))
            return None


class EvasiveActionPredictionParameters(PredictionParameters):
    def __init__(self, maxSpeed, nPredictedTrajectories, accelerationDistribution, steeringDistribution,
                 useFeatures=False):
        '''Suggested acceleration distribution may not be symmetric, eg
        lambda: random.triangular(self.minAcceleration, self.maxAcceleration, 0.)'''

        if useFeatures:
            name = 'point set evasive action'
        else:
            name = 'evasive action'
        PredictionParameters.__init__(self, name, maxSpeed)
        self.nPredictedTrajectories = nPredictedTrajectories
        self.useFeatures = useFeatures
        self.accelerationDistribution = accelerationDistribution
        self.steeringDistribution = steeringDistribution

    def __str__(self):
        return PredictionParameters.__str__(self) + ' {0} {1} {2} {3}'.format(self.nPredictedTrajectories,
                                                                              self.minAcceleration,
                                                                              self.maxAcceleration, self.maxSteering)

    def generatePredictedTrajectories(self, obj, instant):
        predictedTrajectories = []
        if self.useFeatures and obj.hasFeatures():
            features = [
                f for f in obj.getFeatures() if f.existsAtInstant(instant)]
            positions = [f.getPositionAtInstant(instant) for f in features]
            velocities = [f.getVelocityAtInstant(instant) for f in features]
        else:
            positions = [obj.getPositionAtInstant(instant)]
            velocities = [obj.getVelocityAtInstant(instant)]
        for i in xrange(self.nPredictedTrajectories):
            for initialPosition, initialVelocity in zip(positions, velocities):
                predictedTrajectories.append(PredictedTrajectoryConstant(initialPosition,
                                                                         initialVelocity,
                                                                         moving.NormAngle(
                                                                             self.accelerationDistribution(),
                                                                             self.steeringDistribution()),
                                                                         maxSpeed=self.maxSpeed))
        return predictedTrajectories


class CVDirectPredictionParameters(PredictionParameters):
    '''Prediction parameters of prediction at constant velocity
    using direct computation of the intersecting point
    Warning: the computed time to collision may be higher than timeHorizon (not used)'''

    def __init__(self):
        PredictionParameters.__init__(
            self, 'constant velocity (direct computation)', None)

    def computeCrossingsCollisionsAtInstant(self, currentInstant, obj1, obj2, collisionDistanceThreshold, timeHorizon,
                                            computeCZ=False, debug=False, *kwargs):
        collisionPoints = []
        crossingZones = []

        p1 = obj1.getPositionAtInstant(currentInstant)
        p2 = obj2.getPositionAtInstant(currentInstant)
        if (p1 - p2).norm2() <= collisionDistanceThreshold:
            collisionPoints = [SafetyPoint((p1 + p1).multiply(0.5), 1., 0.)]
        else:
            v1 = obj1.getVelocityAtInstant(currentInstant)
            v2 = obj2.getVelocityAtInstant(currentInstant)
            intersection = moving.intersection(p1, p1 + v1, p2, p2 + v2)

            if intersection is not None:
                dp1 = intersection - p1
                dp2 = intersection - p2
                dot1 = moving.Point.dot(dp1, v1)
                dot2 = moving.Point.dot(dp2, v2)
                # print dot1, dot2
                # (computeCZ and (dot1 > 0 or dot2 > 0)) or (
                if (computeCZ and (dot1 > 0 or dot2 > 0)) or (
                        dot1 > 0 and dot2 > 0):  # if the road users are moving towards the intersection or if computing pPET
                    dist1 = dp1.norm2()
                    dist2 = dp2.norm2()
                    s1 = math.copysign(v1.norm2(), dot1)
                    s2 = math.copysign(v2.norm2(), dot2)
                    halfCollisionDistanceThreshold = collisionDistanceThreshold / \
                        2.
                    timeInterval1 = moving.TimeInterval(max(0, dist1 - halfCollisionDistanceThreshold) / s1,
                                                        (dist1 + halfCollisionDistanceThreshold) / s1)
                    timeInterval2 = moving.TimeInterval(max(0, dist2 - halfCollisionDistanceThreshold) / s2,
                                                        (dist2 + halfCollisionDistanceThreshold) / s2)
                    collisionTimeInterval = moving.TimeInterval.intersection(
                        timeInterval1, timeInterval2)

                    if collisionTimeInterval.empty():
                        if computeCZ:
                            crossingZones = [
                                SafetyPoint(intersection, 1., timeInterval1.distance(timeInterval2))]
                    else:
                        collisionPoints = [
                            SafetyPoint(intersection, 1., collisionTimeInterval.center())]

        if debug and intersection is not None:
            from matplotlib.pyplot import plot, figure, axis, title
            figure()
            plot([p1.x, intersection.x], [p1.y, intersection.y], 'r')
            plot([p2.x, intersection.x], [p2.y, intersection.y], 'b')
            intersection.plot()
            obj1.plot('r')
            obj2.plot('b')
            title('instant {0}'.format(currentInstant))
            axis('equal')

        return currentInstant, collisionPoints, crossingZones


class CVExactPredictionParameters(PredictionParameters):
    '''Prediction parameters of prediction at constant velocity
    using direct computation of the intersecting point (solving the equation)
    Warning: the computed time to collision may be higher than timeHorizon (not used)'''

    def __init__(self):
        PredictionParameters.__init__(
            self, 'constant velocity (direct exact computation)', None)

    def computeCrossingsCollisionsAtInstant(self, currentInstant, obj1, obj2, collisionDistanceThreshold, timeHorizon,
                                            computeCZ=False, debug=False, *kwargs):
        'TODO add collision point coordinates, compute pPET'
        # collisionPoints = []
        # crossingZones = []

        p1 = obj1.getPositionAtInstant(currentInstant)
        p2 = obj2.getPositionAtInstant(currentInstant)
        v1 = obj1.getVelocityAtInstant(currentInstant)
        v2 = obj2.getVelocityAtInstant(currentInstant)
        intersection = moving.intersection(p1, p1 + v1, p2, p2 + v2)

        if intersection is not None:
            ttc = moving.Point.timeToCollision(
                p1, p2, v1, v2, collisionDistanceThreshold)
            if ttc:
                return currentInstant, [
                    SafetyPoint(intersection, 1., ttc)], []  # (p1+v1.multiply(ttc)+p2+v2.multiply(ttc)).multiply(0.5)
            else:
                return currentInstant, [], []


####
# Other Methods
####
class PrototypePredictionParameters(PredictionParameters):
    def __init__(self, maxSpeed, nPredictedTrajectories, constantSpeed=True):
        name = 'prototype'
        PredictionParameters.__init__(self, name, maxSpeed)
        self.nPredictedTrajectories = nPredictedTrajectories
        self.constantSpeed = constantSpeed

    def generatePredictedTrajectories(self, obj, instant, prototypeTrajectories):
        predictedTrajectories = []
        initialPosition = obj.getPositionAtInstant(instant)
        initialVelocity = obj.getVelocityAtInstant(instant)
        for prototypeTraj in prototypeTrajectories.keys():
            predictedTrajectories.append(PredictedTrajectoryPrototype(initialPosition, initialVelocity, prototypeTraj,
                                                                      constantSpeed=self.constantSpeed,
                                                                      probability=prototypeTrajectories[prototypeTraj]))
        return predictedTrajectories


if __name__ == "__main__":
    import doctest
    import unittest

    suite = doctest.DocFileSuite('tests/prediction.txt')
    # suite = doctest.DocTestSuite()
    unittest.TextTestRunner().run(suite)
    # doctest.testmod()
    # doctest.testfile("example.txt")
