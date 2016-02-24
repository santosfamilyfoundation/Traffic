#! /usr/bin/env python

import sys, argparse

import matplotlib.pyplot as plt
import numpy as np
import cv2

import cvutils, utils, storage

# TODO add option to use RANSAC or other robust homography estimation method?

parser = argparse.ArgumentParser(description='The program computes the homography matrix from at least 4 non-colinear point correspondences inputed in the same order in a video frame and a aerial photo/ground map, or from the list of corresponding points in the two planes.', epilog = '''The point correspondence file contains at least 4 non-colinear point coordinates 
with the following format:
 - the first two lines are the x and y coordinates in the projected space (usually world space)
 - the last two lines are the x and y coordinates in the origin space (usually image space)

If providing video and world images, with a number of points to input
and a ration to convert pixels to world distance unit (eg meters per pixel), 
the images will be shown in turn and the user should click 
in the same order the corresponding points in world and image spaces.''', formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument('-p', dest = 'pointCorrespondencesFilename', help = 'name of the text file containing the point correspondences')
parser.add_argument('--tsai', dest = 'tsaiCameraFilename', help = 'name of the text file containing the camera parameter following the pinhole camera model (Lund format)') # caution, this is Aliaksei's format
parser.add_argument('-i', dest = 'videoFrameFilename', help = 'filename of the video frame')
parser.add_argument('-w', dest = 'worldFilename', help = 'filename of the aerial photo/ground map')
parser.add_argument('-n', dest = 'nPoints', help = 'number of corresponding points to input', default = 4, type = int)
parser.add_argument('-u', dest = 'unitsPerPixel', help = 'number of units per pixel', default = 1., type = float)
parser.add_argument('-o', dest = 'homographyFilename', help = 'filename of the homography matrix', default = 'homography.txt')
parser.add_argument('--display', dest = 'displayPoints', help = 'display original and projected points on both images', action = 'store_true')
parser.add_argument('--intrinsic', dest = 'intrinsicCameraMatrixFilename', help = 'name of the intrinsic camera file')
parser.add_argument('--distortion-coefficients', dest = 'distortionCoefficients', help = 'distortion coefficients', nargs = '*', type = float)
parser.add_argument('--undistorted-multiplication', dest = 'undistortedImageMultiplication', help = 'undistorted image multiplication', type = float)
parser.add_argument('--undistort', dest = 'undistort', help = 'undistort the video (because features have been extracted that way', action = 'store_true')
parser.add_argument('--save', dest = 'saveImages', help = 'save the undistorted video frame (display option must be chosen)', action = 'store_true')

args = parser.parse_args()

# TODO process camera intrinsic and extrinsic parameters to obtain image to world homography, taking example from Work/src/python/generate-homography.py script
# cameraMat = load(videoFilenamePrefix+'-camera.txt');
# T1 = cameraMat[3:6,:].copy();
# A = cameraMat[0:3,0:3].copy();

# # pay attention, rotation may be the transpose
# # R = T1[:,0:3].T;
# R = T1[:,0:3];
# rT = dot(R, T1[:,3]/1000);
# T = zeros((3,4),'f');
# T[:,0:3] = R[:];
# T[:,3] = rT;

# AT = dot(A,T);

# nPoints = 4;
# worldPoints = cvCreateMat(nPoints, 3, CV_64FC1);
# imagePoints = cvCreateMat(nPoints, 3, CV_64FC1);

# # extract homography from the camera calibration
# worldPoints = cvCreateMat(4, 3, CV_64FC1);
# imagePoints = cvCreateMat(4, 3, CV_64FC1);

# worldPoints[0,:] = [[1, 1, 0]];
# worldPoints[1,:] = [[1, 2, 0]];
# worldPoints[2,:] = [[2, 1, 0]];
# worldPoints[3,:] = [[2, 2, 0]];

# wPoints = [[1,1,2,2],
#            [1,2,1,2],
#            [0,0,0,0]];
# iPoints = utils.worldToImage(AT, wPoints);

# for i in range(nPoints):
#     imagePoints[i,:] = [iPoints[:,i].tolist()];

# H = cvCreateMat(3, 3, CV_64FC1);

# cvFindHomography(imagePoints, worldPoints, H);


homography = np.array([])
if args.pointCorrespondencesFilename is not None:
    worldPts, videoPts = cvutils.loadPointCorrespondences(args.pointCorrespondencesFilename)
    homography, mask = cv2.findHomography(videoPts, worldPts) # method=0, ransacReprojThreshold=3
elif args.tsaiCameraFilename is not None: # hack using PDTV
    from pdtv import TsaiCamera
    f = storage.openCheck(args.tsaiCameraFilename, quitting = True)
    content = storage.getLines(f)
    cameraData = {}
    for l in content:
        tmp = l.split(':')
        cameraData[tmp[0]] = float(tmp[1].strip().replace(',','.'))
    camera = TsaiCamera(Cx=cameraData['Cx'], Cy=cameraData['Cy'], Sx=cameraData['Sx'], Tx=cameraData['Tx'], Ty=cameraData['Ty'], Tz=cameraData['Tz'], dx=cameraData['dx'], dy=cameraData['dy'], f=cameraData['f'], k=cameraData['k'], r1=cameraData['r1'], r2=cameraData['r2'], r3=cameraData['r3'], r4=cameraData['r4'], r5=cameraData['r5'], r6=cameraData['r6'], r7=cameraData['r7'], r8=cameraData['r8'], r9=cameraData['r9'])
    homography = cvutils.computeHomographyFromPDTV(camera)
elif args.videoFrameFilename is not None and args.worldFilename is not None:
    worldImg = plt.imread(args.worldFilename)
    videoImg = plt.imread(args.videoFrameFilename)
    if args.undistort:        
        [map1, map2] = cvutils.computeUndistortMaps(videoImg.shape[1], videoImg.shape[0], args.undistortedImageMultiplication, np.loadtxt(args.intrinsicCameraMatrixFilename), args.distortionCoefficients)
        videoImg = cv2.remap(videoImg, map1, map2, interpolation=cv2.INTER_LINEAR)
    print('Click on {0} points in the video frame'.format(args.nPoints))
    plt.figure()
    plt.imshow(videoImg)
    videoPts = np.array(plt.ginput(args.nPoints, timeout=3000))
    print('Click on {0} points in the world image'.format(args.nPoints))
    plt.figure()
    plt.imshow(worldImg)
    worldPts = args.unitsPerPixel*np.array(plt.ginput(args.nPoints, timeout=3000))
    plt.close('all')
    homography, mask = cv2.findHomography(videoPts, worldPts)
    # save the points in file
    f = open('point-correspondences.txt', 'a')
    np.savetxt(f, worldPts.T)
    np.savetxt(f, videoPts.T)
    f.close()

if homography.size>0:
    np.savetxt(args.homographyFilename,homography)

if args.displayPoints and args.videoFrameFilename is not None and args.worldFilename is not None and homography.size>0 and args.tsaiCameraFilename is None:
    worldImg = cv2.imread(args.worldFilename)
    videoImg = cv2.imread(args.videoFrameFilename)
    if args.undistort:
        [map1, map2] = cvutils.computeUndistortMaps(videoImg.shape[1], videoImg.shape[0], args.undistortedImageMultiplication, np.loadtxt(args.intrinsicCameraMatrixFilename), args.distortionCoefficients)
        videoImg = cv2.remap(videoImg, map1, map2, interpolation=cv2.INTER_LINEAR)
        if args.saveImages:
            cv2.imwrite(utils.removeExtension(args.videoFrameFilename)+'-undistorted.png', videoImg)
    invHomography = np.linalg.inv(homography)
    projectedWorldPts = cvutils.projectArray(invHomography, worldPts.T).T
    projectedVideoPts = cvutils.projectArray(homography, videoPts.T).T
    for i in range(worldPts.shape[0]):
        # world image
        cv2.circle(worldImg,tuple(np.int32(np.round(worldPts[i]/args.unitsPerPixel))),2,cvutils.cvBlue)
        cv2.circle(worldImg,tuple(np.int32(np.round(projectedVideoPts[i]/args.unitsPerPixel))),2,cvutils.cvRed)
        cv2.putText(worldImg, str(i+1), tuple(np.int32(np.round(worldPts[i]/args.unitsPerPixel))+5), cv2.FONT_HERSHEY_PLAIN, 2., cvutils.cvBlue, 2)
        # video image
        cv2.circle(videoImg,tuple(np.int32(np.round(videoPts[i]))),2,cvutils.cvBlue)
        cv2.circle(videoImg,tuple(np.int32(np.round(projectedWorldPts[i]))),2,cvutils.cvRed)
        cv2.putText(videoImg, str(i+1), tuple(np.int32(np.round(videoPts[i])+5)), cv2.FONT_HERSHEY_PLAIN, 2., cvutils.cvBlue, 2)
    cv2.imshow('video frame',videoImg)
    cv2.imshow('world image',worldImg)
    cv2.waitKey()
    cv2.destroyAllWindows()
