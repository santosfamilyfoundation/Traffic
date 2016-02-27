#! /usr/bin/env python
''' Traffic Engineering Tools and Examples'''

import prediction

from math import ceil


#########################
# Simulation
#########################

def generateTimeHeadways(meanTimeHeadway, simulationTime):
    '''Generates the time headways between arrivals
    given the meanTimeHeadway and the negative exponential distribution
    over a time interval of length simulationTime (assumed to be in same time unit as headway'''
    from random import expovariate
    headways = []
    totalTime = 0
    flow = 1 / meanTimeHeadway
    while totalTime < simulationTime:
        h = expovariate(flow)
        headways.append(h)
        totalTime += h
    return headways


class RoadUser(object):
    '''Simple example of inheritance to plot different road users '''

    def __init__(self, position, velocity):
        'Both fields are 2D numpy arrays'
        self.position = position.astype(float)
        self.velocity = velocity.astype(float)

    def move(self, deltaT):
        self.position += deltaT * self.velocity

    def draw(self, init=False):
        from matplotlib.pyplot import plot
        if init:
            self.plotLine = plot(
                self.position[0], self.position[1], self.getDescriptor())[0]
        else:
            self.plotLine.set_data(self.position[0], self.position[1])


class PassengerVehicle(RoadUser):
    def getDescriptor(self):
        return 'dr'


class Pedestrian(RoadUser):
    def getDescriptor(self):
        return 'xb'


class Cyclist(RoadUser):
    def getDescriptor(self):
        return 'og'


#########################
# fundamental diagram
#########################

class FundamentalDiagram(object):
    ''' '''

    def __init__(self, name):
        self.name = name

    def q(self, k):
        return k * self.v(k)

    @staticmethod
    def meanHeadway(k):
        return 1 / k

    @staticmethod
    def meanSpacing(q):
        return 1 / q

    def plotVK(self, language='fr', units={}):
        from numpy import arange
        from matplotlib.pyplot import figure, plot, xlabel, ylabel
        densities = [k for k in arange(1, self.kj + 1)]
        figure()
        plot(densities, [self.v(k) for k in densities])
        xlabel('Densite (veh/km)')  # todo other languages and adapt to units
        ylabel('Vitesse (km/h)')

    def plotQK(self, language='fr', units={}):
        from numpy import arange
        from matplotlib.pyplot import figure, plot, xlabel, ylabel
        densities = [k for k in arange(1, self.kj + 1)]
        figure()
        plot(densities, [self.q(k) for k in densities])
        xlabel('Densite (veh/km)')  # todo other languages and adapt to units
        ylabel('Debit (km/h)')


class GreenbergFD(FundamentalDiagram):
    '''Speed is the logarithm of density'''

    def __init__(self, vc, kj):
        FundamentalDiagram.__init__(self, 'Greenberg')
        self.vc = vc
        self.kj = kj

    def v(self, k):
        from numpy import log
        return self.vc * log(self.kj / k)

    def criticalDensity(self):
        from numpy import e
        self.kc = self.kj / e
        return self.kc

    def capacity(self):
        self.qmax = self.kc * self.vc
        return self.qmax

#########################
# intersection
#########################


class FourWayIntersection(object):
    '''Simple class for simple intersection outline'''

    def __init__(self, dimension, coordX, coordY):
        self.dimension = dimension
        self.coordX = coordX
        self.coordY = coordY

    def plot(self, options='k'):
        from matplotlib.pyplot import plot, axis

        minX = min(self.dimension[0])
        maxX = max(self.dimension[0])
        minY = min(self.dimension[1])
        maxY = max(self.dimension[1])

        plot([minX, self.coordX[0], self.coordX[0]],
             [self.coordY[0], self.coordY[0], minY], options)
        plot([self.coordX[1], self.coordX[1], maxX],
             [minY, self.coordY[0], self.coordY[0]], options)
        plot([minX, self.coordX[0], self.coordX[0]],
             [self.coordY[1], self.coordY[1], maxY], options)
        plot([self.coordX[1], self.coordX[1], maxX],
             [maxY, self.coordY[1], self.coordY[1]], options)
        axis('equal')

#########################
# traffic signals
#########################


class Volume(object):
    '''Class to represent volumes with varied vehicule types '''

    def __init__(self, volume, types=['pc'], proportions=[1], equivalents=[1], nLanes=1):
        '''mvtEquivalent is the equivalent if the movement is right of left turn'''

        # check the sizes of the lists
        if sum(proportions) == 1:
            self.volume = volume
            self.types = types
            self.proportions = proportions
            self.equivalents = equivalents
            self.nLanes = nLanes
        else:
            print('Proportions do not sum to 1')
            pass

    def checkProtected(self, opposedThroughMvt):
        '''Checks if this left movement should be protected,
        ie if one of the main two conditions on left turn is verified'''
        return self.volume >= 200 or self.volume * opposedThroughMvt.volume / opposedThroughMvt.nLanes > 50000

    def getPCUVolume(self):
        '''Returns the passenger-car equivalent for the input volume'''
        v = 0
        for p, e in zip(self.proportions, self.equivalents):
            v += p * e
        return v * self.volume


class IntersectionMovement(object):
    '''Represents an intersection movement
    with a volume, a type (through, left or right)
    and an equivalent for movement type'''

    def __init__(self, volume, mvtEquivalent=1):
        self.volume = volume
        self.mvtEquivalent = mvtEquivalent

    def getTVUVolume(self):
        return self.mvtEquivalent * self.volume.getPCUVolume()


class LaneGroup(object):
    '''Class that represents a group of mouvements'''

    def __init__(self, movements, nLanes):
        self.movements = movements
        self.nLanes = nLanes

    def getTVUVolume(self):
        return sum([mvt.getTVUVolume() for mvt in self.movements])

    def getCharge(self, saturationVolume):
        return self.getTVUVolume() / (self.nLanes * saturationVolume)


def optimalCycle(lostTime, criticalCharge):
    return (1.5 * lostTime + 5) / (1 - criticalCharge)


def minimumCycle(lostTime, criticalCharge, degreeSaturation=1.):
    'degree of saturation can be used as the peak hour factor too'
    return lostTime / (1 - criticalCharge / degreeSaturation)


class Cycle(object):
    '''Class to compute optimal cycle and the split of effective green times'''

    def __init__(self, phases, lostTime, saturationVolume):
        '''phases is a list of phases
        a phase is a list of lanegroups'''
        self.phases = phases
        self.lostTime = lostTime
        self.saturationVolume = saturationVolume

    def computeCriticalCharges(self):
        self.criticalCharges = [max(
            [lg.getCharge(self.saturationVolume) for lg in phase]) for phase in self.phases]
        self.criticalCharge = sum(self.criticalCharges)

    def computeOptimalCycle(self):
        self.computeCriticalCharges()
        self.C = optimalCycle(self.lostTime, self.criticalCharge)
        return self.C

    def computeMinimumCycle(self, degreeSaturation=1.):
        self.computeCriticalCharges()
        self.C = minimumCycle(
            self.lostTime, self.criticalCharge, degreeSaturation)
        return self.C

    def computeEffectiveGreen(self):
        #from numpy import round
        # self.computeCycle() # in case it was not done before
        effectiveGreenTime = self.C - self.lostTime
        self.effectiveGreens = [round(
            c * effectiveGreenTime / self.criticalCharge, 1) for c in self.criticalCharges]
        return self.effectiveGreens


def computeInterGreen(perceptionReactionTime, initialSpeed, intersectionLength, vehicleAverageLength=6, deceleration=3):
    '''Computes the intergreen time (yellow/amber plus all red time)
    Deceleration is positive
    All variables should be in the same units'''
    if deceleration > 0:
        return [perceptionReactionTime + float(initialSpeed) / (2 * deceleration), float(intersectionLength + vehicleAverageLength) / initialSpeed]
    else:
        print 'Issue deceleration should be strictly positive'
        return None


def uniformDelay(cycleLength, effectiveGreen, saturationDegree):
    '''Computes the uniform delay'''
    return 0.5 * cycleLength * (1 - float(effectiveGreen) / cycleLength) / (1 - float(effectiveGreen * saturationDegree) / cycleLength)


def overflowDelay(T, X, c, k=0.5, I=1):
    '''Computes the overflow delay (HCM)
    T in hours
    c capacity of the lane group
    k default for fixed time signal
    I=1 for isolated intersection (Poisson arrival)'''
    from math import sqrt
    return 900 * T * (X - 1 + sqrt((X - 1)**2 + 8 * k * I * X / (c * T)))

#########################
# misc
#########################


def timeChangingSpeed(v0, vf, a, TPR):
    return TPR + (vf - v0) / a


def distanceChangingSpeed(v0, vf, a, TPR):
    return TPR * v0 + (vf * vf - v0 * v0) / (2 * a)
