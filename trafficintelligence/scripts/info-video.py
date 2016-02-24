#! /usr/bin/env python

import sys, argparse
import cvutils


parser = argparse.ArgumentParser(description='The program displays the video.')
parser.add_argument('-i', dest = 'videoFilename', help = 'name of the video file', required = True)

args = parser.parse_args()

cvutils.infoVideo(args.videoFilename)
