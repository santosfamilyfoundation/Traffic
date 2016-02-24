#! /usr/bin/env python

import numpy as np
import sys, argparse
from cv2 import SVM_RBF, SVM_C_SVC

import cvutils, moving, ml

parser = argparse.ArgumentParser(description='The program processes indicators for all pairs of road users in the scene')
parser.add_argument('-d', dest = 'directoryName', help = 'parent directory name for the directories containing the samples for the different road users', required = True)
parser.add_argument('--kernel', dest = 'kernelType', help = 'kernel type for the support vector machine (SVM)', default = SVM_RBF, type = long)
parser.add_argument('--svm', dest = 'svmType', help = 'SVM type', default = SVM_C_SVC, type = long)
parser.add_argument('-s', dest = 'rescaleSize', help = 'rescale size of image samples', default = 64, type = int)
parser.add_argument('-o', dest = 'nOrientations', help = 'number of orientations in HoG', default = 9, type = int)
parser.add_argument('-p', dest = 'nPixelsPerCell', help = 'number of pixels per cell', default = 8, type = int)
parser.add_argument('-c', dest = 'nCellsPerBlock', help = 'number of cells per block', default = 2, type = int)
args = parser.parse_args()

rescaleSize = (args.rescaleSize, args.rescaleSize)
nPixelsPerCell = (args.nPixelsPerCell, args.nPixelsPerCell)
nCellsPerBlock = (args.nCellsPerBlock, args.nCellsPerBlock)

imageDirectories = {'pedestrian': args.directoryName + "/Pedestrians/",
                    'bicycle': args.directoryName + "/Cyclists/",
                    'car': args.directoryName + "/Vehicles/"}

#directory_model = args.directoryName
trainingSamplesPBV = {}
trainingLabelsPBV = {}
trainingSamplesBV = {}
trainingLabelsBV = {}
trainingSamplesPB = {}
trainingLabelsPB = {}
trainingSamplesPV = {}
trainingLabelsPV = {}

for k, v in imageDirectories.iteritems():
    print('Loading {} samples'.format(k))
    trainingSamples, trainingLabels = cvutils.createHOGTrainingSet(v, moving.userType2Num[k], rescaleSize, args.nOrientations, nPixelsPerCell, nCellsPerBlock)
    trainingSamplesPBV[k], trainingLabelsPBV[k] = trainingSamples, trainingLabels
    if k != 'pedestrian':
	trainingSamplesBV[k], trainingLabelsBV[k] = trainingSamples, trainingLabels
    if k != 'car':
	trainingSamplesPB[k], trainingLabelsPB[k] = trainingSamples, trainingLabels
    if k != 'bicycle':
	trainingSamplesPV[k], trainingLabelsPV[k] = trainingSamples, trainingLabels

# Training the Support Vector Machine
print "Training Pedestrian-Cyclist-Vehicle Model"
model = ml.SVM()
model.train(np.concatenate(trainingSamplesPBV.values()), np.concatenate(trainingLabelsPBV.values()), args.svmType, args.kernelType)
model.save(args.directoryName + "/modelPBV.xml")

print "Training Cyclist-Vehicle Model"
model = ml.SVM()
model.train(np.concatenate(trainingSamplesBV.values()), np.concatenate(trainingLabelsBV.values()), args.svmType, args.kernelType)
model.save(args.directoryName + "/modelBV.xml")

print "Training Pedestrian-Cyclist Model"
model = ml.SVM()
model.train(np.concatenate(trainingSamplesPB.values()), np.concatenate(trainingLabelsPB.values()), args.svmType, args.kernelType)
model.save(args.directoryName + "/modelPB.xml")

print "Training Pedestrian-Vehicle Model"
model = ml.SVM()
model.train(np.concatenate(trainingSamplesPV.values()), np.concatenate(trainingLabelsPV.values()), args.svmType, args.kernelType)
model.save(args.directoryName + "/modelPV.xml")
