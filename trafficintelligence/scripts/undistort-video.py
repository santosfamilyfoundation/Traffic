#! /usr/bin/env python

import sys, argparse

import numpy as np
import cv2

import cvutils
from math import ceil, log10

parser = argparse.ArgumentParser(description='''The program converts a video into a series of images corrected for distortion. One can then use mencoder to generate a movie, eg
$ mencoder 'mf://./*.png' -mf fps=[framerate]:type=png -ovc xvid -xvidencopts bitrate=[bitrate] -nosound -o [output.avi]''')

parser.add_argument('-i', dest = 'videoFilename', help = 'filename of the video sequence')
parser.add_argument('--intrinsic', dest = 'intrinsicCameraMatrixFilename', help = 'name of the intrinsic camera file')
parser.add_argument('--distortion-coefficients', dest = 'distortionCoefficients', help = 'distortion coefficients', nargs = '*', type = float)
parser.add_argument('--undistorted-multiplication', dest = 'undistortedImageMultiplication', help = 'undistorted image multiplication', type = float)
parser.add_argument('-f', dest = 'firstFrameNum', help = 'number of first frame number to display', type = int)
parser.add_argument('-l', dest = 'lastFrameNum', help = 'number of last frame number to save', type = int)

args = parser.parse_args()

intrinsicCameraMatrix = np.loadtxt(args.intrinsicCameraMatrixFilename)
#distortionCoefficients = args.distortionCoefficients
#undistortedImageMultiplication = args.undistortedImageMultiplication
#firstFrameNum = params.firstFrameNum

capture = cv2.VideoCapture(args.videoFilename)
width = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
height = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))
[map1, map2] = cvutils.computeUndistortMaps(width, height, args.undistortedImageMultiplication, intrinsicCameraMatrix, args.distortionCoefficients)
if capture.isOpened():
    ret = True
    frameNum = args.firstFrameNum
    capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, args.firstFrameNum)
    if args.lastFrameNum is None:
        from sys import maxint
        lastFrameNum = maxint
    else:
        lastFrameNum = args.lastFrameNum
        nZerosFilename = int(ceil(log10(lastFrameNum)))
        while ret and frameNum < lastFrameNum:
            ret, img = capture.read()
            if ret:
                img = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR)
                cv2.imwrite('undistorted-{{:0{}}}.png'.format(nZerosFilename).format(frameNum), img)
            frameNum += 1
