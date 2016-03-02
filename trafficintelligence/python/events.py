#! /usr/bin/env python
'''Libraries for events
Interactions, pedestrian crossing...'''

'''
Sam comments:
Contains a bunch of cool event-tracking stuff. Interaction classification 
(must be road-user classification involved here somewhere!)

Important functions:
    - findRoute: computes routes for objects
    - createInteractions: given 2 road users, computes all interactions
    - aggregateSafetyPoints: combine all interactions into a list for display

Important classes:
    - Interaction: represents an interaction between two road users.
        - get/add indicator functions
        - play: plays video of two road users and their interaction. Use "setRoadUsers".
        Utilizes displayTrajectories from cvutils.py
        - computeCrossingsCollisions: computes all crossings/collisions for two road users
'''

import moving, prediction, indicators, utils, cvutils, ml
from base import VideoFilenameAddable

import numpy as np

import multiprocessing
import itertools


def findRoute(prototypes,objects,i,j,noiseEntryNums,noiseExitNums,minSimilarity= 0.3, spatialThreshold=1.0, delta=180):
    if i[0] not in noiseEntryNums: 
        prototypesRoutes= [ x for x in sorted(prototypes.keys()) if i[0]==x[0]]
    elif i[1] not in noiseExitNums:
        prototypesRoutes=[ x for x in sorted(prototypes.keys()) if i[1]==x[1]]
    else:
        prototypesRoutes=[x for x in sorted(prototypes.keys())]
    routeSim={}
    lcss = utils.LCSS(similarityFunc=lambda x,y: (distanceForLCSS(x,y) <= spatialThreshold),delta=delta)
    for y in prototypesRoutes: 
        if y in prototypes.keys():
            prototypesIDs=prototypes[y]
            similarity=[]
            for x in prototypesIDs:
                s=lcss.computeNormalized(objects[j].positions, objects[x].positions)
                similarity.append(s)
            routeSim[y]=max(similarity)
    route=max(routeSim, key=routeSim.get)
    if routeSim[route]>=minSimilarity:
        return route
    else:
        return i

def getRoute(obj,prototypes,objects,noiseEntryNums,noiseExitNums,useDestination=True):
    route=(obj.startRouteID,obj.endRouteID)
    if useDestination:
        if route not in prototypes.keys():
            route= findRoute(prototypes,objects,route,obj.getNum(),noiseEntryNums,noiseExitNums)
    return route

class Interaction(moving.STObject, VideoFilenameAddable):
    '''Class for an interaction between two road users 
    or a road user and an obstacle
    
    link to the moving objects
    contains the indicators in a dictionary with the names as keys
    '''

    categories = {'Head On': 0,
                  'rearend': 1,
                  'side': 2,
                  'parallel': 3}

    indicatorNames = ['Collision Course Dot Product',
                      'Collision Course Angle',
                      'Distance',
                      'Minimum Distance',
                      'Velocity Angle',
                      'Speed Differential',
                      'Collision Probability',
                      'Time to Collision', # 7
                      'Probability of Successful Evasive Action',
                      'predicted Post Encroachment Time']

    indicatorNameToIndices = utils.inverseEnumeration(indicatorNames)

    indicatorShortNames = ['CCDP',
                           'CCA',
                           'Dist',
                           'MinDist',
                           'VA',
                           'SD',
                           'PoC',
                           'TTC',
                           'P(SEA)',
                           'pPET']

    indicatorUnits = ['',
                      'rad',
                      'm',
                      'm',
                      'rad',
                      'm/s',
                      '',
                      's',
                      '',
                      '']

    timeIndicators = ['Time to Collision', 'predicted Post Encroachment Time']

    def __init__(self, num = None, timeInterval = None, roaduserNum1 = None, roaduserNum2 = None, roadUser1 = None, roadUser2 = None, categoryNum = None):
        moving.STObject.__init__(self, num, timeInterval)
        if timeInterval is None and roadUser1 is not None and roadUser2 is not None:
            self.timeInterval = roadUser1.commonTimeInterval(roadUser2)
        self.roadUser1 = roadUser1
        self.roadUser2 = roadUser2
        if roaduserNum1 is not None and roaduserNum2 is not None:
            self.roadUserNumbers = set([roaduserNum1, roaduserNum2])
        elif roadUser1 is not None and roadUser2 is not None:
            self.roadUserNumbers = set([roadUser1.getNum(), roadUser2.getNum()])
        else:
            self.roadUserNumbers = None
        self.categoryNum = categoryNum
        self.indicators = {}
        self.interactionInterval = None
         # list for collison points and crossing zones
        self.collisionPoints = None
        self.crossingZones = None

    def getRoadUserNumbers(self):
        return self.roadUserNumbers

    def setRoadUsers(self, objects):
        nums = sorted(list(self.getRoadUserNumbers()))
        if nums[0]<len(objects) and objects[nums[0]].getNum() == nums[0]:
            self.roadUser1 = objects[nums[0]]
        if nums[1]<len(objects) and objects[nums[1]].getNum() == nums[1]:
            self.roadUser2 = objects[nums[1]]

        if self.roadUser1 is None or self.roadUser2 is None:
            self.roadUser1 = None
            self.roadUser2 = None
            i = 0
            while i < len(objects) and self.roadUser2 is None:
                if objects[i].getNum() in nums:
                    if self.roadUser1 is None:
                        self.roadUser1 = objects[i]
                    else:
                        self.roadUser2 = objects[i]
                i += 1

    def getIndicator(self, indicatorName):
        return self.indicators.get(indicatorName, None)

    def addIndicator(self, indicator):
        if indicator is not None:
            self.indicators[indicator.name] = indicator

    def getIndicatorValueAtInstant(self, indicatorName, instant):
        indicator = self.getIndicator(indicatorName)
        if indicator is not None:
            return indicator[instant]
        else:
            return None

    def getIndicatorValuesAtInstant(self, instant):
        '''Returns list of indicator values at instant
        as dict (with keys from indicators dict)'''
        values = {}
        for k, indicator in self.indicators.iteritems():
            values[k] = indicator[instant]
        return values
        
    def plot(self, options = '', withOrigin = False, timeStep = 1, withFeatures = False, **kwargs):
        self.roadUser1.plot(options, withOrigin, timeStep, withFeatures, **kwargs)
        self.roadUser2.plot(options, withOrigin, timeStep, withFeatures, **kwargs)

    def plotOnWorldImage(self, nPixelsPerUnitDistance, options = '', withOrigin = False, timeStep = 1, **kwargs):
        self.roadUser1.plotOnWorldImage(nPixelsPerUnitDistance, options, withOrigin, timeStep, **kwargs)
        self.roadUser2.plotOnWorldImage(nPixelsPerUnitDistance, options, withOrigin, timeStep, **kwargs)

    def play(self, videoFilename, homography = None, undistort = False, intrinsicCameraMatrix = None, distortionCoefficients = None, undistortedImageMultiplication = 1.):
        if self.roadUser1 is not None and self.roadUser2 is not None:
            cvutils.displayTrajectories(videoFilename, [self.roadUser1, self.roadUser2], homography = homography, firstFrameNum = self.getFirstInstant(), lastFrameNumArg = self.getLastInstant(), undistort = undistort, intrinsicCameraMatrix = intrinsicCameraMatrix, distortionCoefficients = distortionCoefficients, undistortedImageMultiplication = undistortedImageMultiplication)
        else:
            print('Please set the interaction road user attributes roadUser1 and roadUser1 through the method setRoadUsers')

    def computeIndicators(self):
        '''Computes the collision course cosine only if the cosine is positive'''
        collisionCourseDotProducts = {}#[0]*int(self.timeInterval.length())
        collisionCourseAngles = {}
        velocityAngles = {}
        distances = {}#[0]*int(self.timeInterval.length())
        speedDifferentials = {}
        interactionInstants = []
        for instant in self.timeInterval:
            deltap = self.roadUser1.getPositionAtInstant(instant)-self.roadUser2.getPositionAtInstant(instant)
            v1 = self.roadUser1.getVelocityAtInstant(instant)
            v2 = self.roadUser2.getVelocityAtInstant(instant)
            deltav = v2-v1
            velocityAngles[instant] = np.arccos(moving.Point.dot(v1, v2)/(v1.norm2()*v2.norm2()))
            collisionCourseDotProducts[instant] = moving.Point.dot(deltap, deltav)
            distances[instant] = deltap.norm2()
            speedDifferentials[instant] = deltav.norm2()
            if collisionCourseDotProducts[instant] > 0:
                interactionInstants.append(instant)
            if distances[instant] != 0 and speedDifferentials[instant] != 0:
                collisionCourseAngles[instant] = np.arccos(collisionCourseDotProducts[instant]/(distances[instant]*speedDifferentials[instant]))

        if len(interactionInstants) >= 2:
            self.interactionInterval = moving.TimeInterval(interactionInstants[0], interactionInstants[-1])
        else:
            self.interactionInterval = moving.TimeInterval()
        self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[0], collisionCourseDotProducts))
        self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[1], collisionCourseAngles))
        self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[2], distances, mostSevereIsMax = False))
        self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[4], velocityAngles))
        self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[5], speedDifferentials))

        # if we have features, compute other indicators
        if self.roadUser1.hasFeatures() and self.roadUser2.hasFeatures():
            minDistances={}
            for instant in self.timeInterval:
                minDistances[instant] = moving.MovingObject.minDistance(self.roadUser1, self.roadUser2, instant)
            self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[3], minDistances, mostSevereIsMax = False))

    def computeCrossingsCollisions(self, predictionParameters, collisionDistanceThreshold, timeHorizon, computeCZ = False, debug = False, timeInterval = None, nProcesses = 1, usePrototypes=False, route1= (-1,-1), route2=(-1,-1), prototypes={}, secondStepPrototypes={}, nMatching={}, objects=[], noiseEntryNums=[], noiseExitNums=[], minSimilarity=0.1, mostMatched=None, useDestination=True, useSpeedPrototype=True, acceptPartialLength=30, step=1):
        '''Computes all crossing and collision points at each common instant for two road users. '''
        TTCs = {}
        if usePrototypes:
            route1= getRoute(self.roadUser1,prototypes,objects,noiseEntryNums,noiseExitNums,useDestination)
            route2= getRoute(self.roadUser2,prototypes,objects,noiseEntryNums,noiseExitNums,useDestination)

        if timeInterval is not None:
            commonTimeInterval = timeInterval
        else:
            commonTimeInterval = self.timeInterval
        self.collisionPoints, crossingZones = predictionParameters.computeCrossingsCollisions(self.roadUser1, self.roadUser2, collisionDistanceThreshold, timeHorizon, computeCZ, debug, commonTimeInterval, nProcesses,usePrototypes,route1,route2,prototypes,secondStepPrototypes,nMatching,objects,noiseEntryNums,noiseExitNums,minSimilarity,mostMatched,useDestination,useSpeedPrototype,acceptPartialLength, step)
        for i, cp in self.collisionPoints.iteritems():
            TTCs[i] = prediction.SafetyPoint.computeExpectedIndicator(cp)
        if len(TTCs) > 0:
            self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[7], TTCs, mostSevereIsMax=False))
        
        # crossing zones and pPET
        if computeCZ:
            self.crossingZones = crossingZones
            pPETs = {}
            for i, cz in self.crossingZones.iteritems():
                pPETs[i] = prediction.SafetyPoint.computeExpectedIndicator(cz)
            self.addIndicator(indicators.SeverityIndicator(Interaction.indicatorNames[9], pPETs, mostSevereIsMax=False))
        # TODO add probability of collision, and probability of successful evasive action

    def computePET(self, collisionDistanceThreshold):
        # TODO add crossing zone
        self.pet = moving.MovingObject.computePET(self.roadUser1, self.roadUser2, collisionDistanceThreshold)

    def setCollision(self, collision):
        '''indicates if it is a collision: argument should be boolean'''
        self.collision = collision

    def isCollision(self):
        if hasattr(self, 'collision'):
            return self.collision
        else:
            return None

    def getCollisionPoints(self):
        return self.collisionPoints

    def getCrossingZones(self):
        return self.crossingZones

def createInteractions(objects, _others = None):
    '''Create all interactions of two co-existing road users'''
    if _others is not None:
        others = _others

    interactions = []
    num = 0
    for i in xrange(len(objects)):
        if _others is None:
            others = objects[:i]
        for j in xrange(len(others)):
            commonTimeInterval = objects[i].commonTimeInterval(others[j])
            if not commonTimeInterval.empty():
                interactions.append(Interaction(num, commonTimeInterval, objects[i].num, others[j].num, objects[i], others[j]))
                num += 1
    return interactions

def findInteraction(interactions, roadUserNum1, roadUserNum2):
    'Returns the right interaction in the set'
    i=0
    while i<len(interactions) and set([roadUserNum1, roadUserNum2]) != interactions[i].getRoadUserNumbers():
        i+=1
    if i<len(interactions):
        return interactions[i]
    else:
        return None

def aggregateSafetyPoints(interactions, pointType = 'collision'):
    '''Put all collision points or crossing zones in a list for display'''
    allPoints = []
    if pointType == 'collision':
        for i in interactions:
            for points in i.collisionPoints.values():
                allPoints += points
    elif pointType == 'crossing':
        for i in interactions:
            for points in i.crossingZones.values():
                allPoints += points
    else:
        print('unknown type of point: '+pointType)
    return allPoints

def prototypeCluster(interactions, similarities, indicatorName, minSimilarity, similarityFunc = None, minClusterSize = None, randomInitialization = False):
    return ml.prototypeCluster([inter.getIndicator(indicatorName) for inter in interactions], similarities, minSimilarity, similarityFunc, minClusterSize, randomInitialization)

class Crossing(moving.STObject):
    '''Class for the event of a street crossing

    TODO: detecter passage sur la chaussee
    identifier origines et destination (ou uniquement chaussee dans FOV)
    carac traversee
    detecter proximite veh (retirer si trop similaire simultanement
    carac interaction'''
    
    def __init__(self, roaduserNum = None, num = None, timeInterval = None):
        moving.STObject.__init__(self, num, timeInterval)
        self.roaduserNum = roaduserNum

    

if __name__ == "__main__":
    import doctest
    import unittest
    suite = doctest.DocFileSuite('tests/events.txt')
    #suite = doctest.DocTestSuite()
    unittest.TextTestRunner().run(suite)
    
