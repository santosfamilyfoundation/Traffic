#! /usr/bin/env python

import sys, argparse

#import matplotlib.pyplot as plt
import numpy as np

import ml, utils, storage

parser = argparse.ArgumentParser(description='The program learns prototypes for the motion patterns') #, epilog = ''
#parser.add_argument('--cfg', dest = 'configFilename', help = 'name of the configuration file')
parser.add_argument('-d', dest = 'databaseFilename', help = 'name of the Sqlite database file', required = True)
parser.add_argument('-t', dest = 'trajectoryType', help = 'type of trajectories to display', choices = ['objectfeatures', 'feature', 'object'], default = 'objectfeatures')
parser.add_argument('-n', dest = 'nTrajectories', help = 'number of the object or feature trajectories to load', type = int, default = None)
parser.add_argument('-e', dest = 'epsilon', help = 'distance for the similarity of trajectory points', type = float, required = True)
parser.add_argument('--metric', dest = 'metric', help = 'metric for the similarity of trajectory points', default = 'cityblock') # default is manhattan distance
parser.add_argument('-s', dest = 'minSimilarity', help = 'minimum similarity to put a trajectory in a cluster', type = float, required = True)
parser.add_argument('-c', dest = 'minClusterSize', help = 'minimum cluster size', type = int, default = None)
parser.add_argument('--display', dest = 'display', help = 'display trajectories', action = 'store_true') # default is manhattan distance

args = parser.parse_args()

# TODO parameters (random init?) and what to learn from: objects, features, longest features from objects
# TODO add possibility to cluter with velocities

trajectoryType = args.trajectoryType
if args.trajectoryType == 'objectfeatures':
    trajectoryType = 'object'

#features = storage.loadTrajectoriesFromSqlite(databaseFilename, args.trajectoryType)
objects = storage.loadTrajectoriesFromSqlite(args.databaseFilename, trajectoryType, withFeatures = (args.trajectoryType == 'objectfeatures'), objectNumbers = args.nTrajectories)

if args.trajectoryType == 'objectfeatures':
    features = []
    for o in objects:
        tmp = utils.sortByLength(o.getFeatures(), reverse = True)
        features += tmp[:min(len(tmp), 3)]
    objects = features

trajectories = [o.getPositions().asArray().T for o in objects]

lcss = utils.LCSS(metric = args.metric, epsilon = args.epsilon)
nTrajectories = len(trajectories)

similarities = -np.ones((nTrajectories, nTrajectories))
# for i in xrange(nTrajectories):
#     for j in xrange(i):
#         similarities[i,j] = lcss.computeNormalized(trajectories[i], trajectories[j])
#         similarities[j,i] = similarities[i,j]

prototypeIndices, labels = ml.prototypeCluster(trajectories, similarities, args.minSimilarity, lambda x,y : lcss.computeNormalized(x, y), args.minClusterSize) # this line can be called again without reinitializing similarities

if args.display:
    from matplotlib.pyplot import figure
    figure()
    for i,o in enumerate(objects):
        if i not in prototypeIndices:
            if labels[i] < 0:
                o.plot('kx')
            else:
                o.plot(utils.colors[labels[i]])
    for i in prototypeIndices:
            objects[i].plot(utils.colors[i]+'o')

# TODO store the prototypes (if features, easy, if objects, info must be stored about the type)
