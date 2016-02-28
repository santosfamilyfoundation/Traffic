#! /usr/bin/env python
'''Algorithms to process trajectories and moving objects'''

import moving

import numpy as np


def extractSpeeds(objects, zone):
    speeds = {}
    objectsNotInZone = []
    import matplotlib.nxutils as nx
    for o in objects:
        inPolygon = nx.points_inside_poly(o.getPositions().asArray().T, zone.T)
        if inPolygon.any():
            objspeeds = [o.getVelocityAt(i).norm2() for i in xrange(int(o.length() - 1)) if inPolygon[i]]
            speeds[o.num] = np.mean(objspeeds)  # km/h
        else:
            objectsNotInZone.append(o)
    return speeds.values(), speeds, objectsNotInZone
