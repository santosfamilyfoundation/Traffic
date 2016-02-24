#! /usr/bin/env python

import sys, argparse
import cvutils


parser = argparse.ArgumentParser(description='The program displays the video.')
parser.add_argument('-i', dest = 'videoFilename', help = 'name of the video file', required = True)
parser.add_argument('-f', dest = 'firstFrameNum', help = 'number of first frame number to display', default = 0, type = int)
parser.add_argument('--fps', dest = 'frameRate', help = 'approximate frame rate to replay', type = float)
parser.add_argument('-r', dest = 'rescale', help = 'rescaling factor for the displayed image', default = 1., type = float)
parser.add_argument('-s', dest = 'step', help = 'display every s image', default = 1, type = int)

args = parser.parse_args()

firstFrameNum = 0
if args.firstFrameNum is not None:
    firstFrameNum = args.firstFrameNum

frameRate = -1
if args.frameRate is not None:
    frameRate = args.frameRate

cvutils.playVideo(args.videoFilename, firstFrameNum, frameRate, rescale = args.rescale, step = args.step)
