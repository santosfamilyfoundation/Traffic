#! /usr/bin/env python

import sys, argparse
from numpy import loadtxt
from numpy.linalg import inv
import moving, storage, cvutils

# TODO: need to trim objects to same mask ?

parser = argparse.ArgumentParser(description='The program computes the CLEAR MOT metrics between ground truth and tracker output (in Polytrack format)', epilog='''CLEAR MOT metrics information:
Keni, Bernardin, and Stiefelhagen Rainer. "Evaluating multiple object tracking performance: the CLEAR MOT metrics." EURASIP Journal on Image and Video Processing 2008 (2008)

Polytrack format: 
JP. Jodoin\'s MSc thesis (in french)
see examples on http://www.jpjodoin.com/urbantracker/dataset.html''', formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('-d', dest = 'trackerDatabaseFilename', help = 'name of the Sqlite database containing the tracker output', required = True)
parser.add_argument('-g', dest = 'groundTruthDatabaseFilename', help = 'name of the Sqlite database containing the ground truth', required = True)
parser.add_argument('-o', dest = 'homographyFilename', help = 'name of the filename for the homography (if tracking was done using the homography)')
parser.add_argument('-m', dest = 'matchingDistance', help = 'matching distance between tracker and ground truth trajectories', required = True, type = float)
parser.add_argument('--mask', dest = 'maskFilename', help = 'filename of the mask file used to define the where objects were tracked')
parser.add_argument('-f', dest = 'firstInstant', help = 'first instant for measurement', required = True, type = int)
parser.add_argument('-l', dest = 'lastInstant', help = 'last instant for measurement', required = True, type = int)
parser.add_argument('--display', dest = 'display', help = 'display the ground truth to object matches (graphically)', action = 'store_true')
parser.add_argument('-i', dest = 'videoFilename', help = 'name of the video file (for display)')
args = parser.parse_args()

if args.homographyFilename is not None:
    homography = loadtxt(args.homographyFilename)
else:
    homography = None

objects = storage.loadTrajectoriesFromSqlite(args.trackerDatabaseFilename, 'object')

if args.maskFilename is not None:
    maskObjects = []
    from matplotlib.pyplot import imread
    mask = imread(args.maskFilename)
    if len(mask) > 1:
        mask = mask[:,:,0]
    for obj in objects:
        maskObjects += obj.getObjectsInMask(mask, inv(homography), 2) # TODO add option to keep object if at least one feature in mask
    objects = maskObjects    

annotations = storage.loadGroundTruthFromSqlite(args.groundTruthDatabaseFilename)
for a in annotations:
    a.computeCentroidTrajectory(homography)

if args.display:
    motp, mota, mt, mme, fpt, gt, gtMatches, toMatches = moving.computeClearMOT(annotations, objects, args.matchingDistance, args.firstInstant, args.lastInstant, True)
else:
    motp, mota, mt, mme, fpt, gt = moving.computeClearMOT(annotations, objects, args.matchingDistance, args.firstInstant, args.lastInstant)


print 'MOTP: {}'.format(motp)
print 'MOTA: {}'.format(mota)
print 'Number of missed objects.frames: {}'.format(mt)
print 'Number of mismatches: {}'.format(mme)
print 'Number of false alarms.frames: {}'.format(fpt)
if args.display:
    cvutils.displayTrajectories(args.videoFilename, objects, {}, inv(homography), args.firstInstant, args.lastInstant, annotations = annotations, gtMatches = gtMatches, toMatches = toMatches)#, rescale = args.rescale, nFramesStep = args.nFramesStep, saveAllImages = args.saveAllImages, undistort = (undistort or args.undistort), intrinsicCameraMatrix = intrinsicCameraMatrix, distortionCoefficients = distortionCoefficients, undistortedImageMultiplication = undistortedImageMultiplication)

    #print('Ground truth matches')
    #print(gtMatches)
    #print('Object matches')
    #rint toMatches
