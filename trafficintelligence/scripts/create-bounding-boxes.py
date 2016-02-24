#! /usr/bin/env python

import argparse

import storage

from numpy.linalg.linalg import inv
from numpy import loadtxt

parser = argparse.ArgumentParser(description='The program creates bounding boxes in image space around all features (for display and for comparison to ground truth in the form of bouding boxes.')
parser.add_argument('-d', dest = 'databaseFilename', help = 'name of the Sqlite database file', required = True)
parser.add_argument('-o', dest = 'homography', help = 'name of the image to world homography')

args = parser.parse_args()

homography = None
if args.homography is not None:
    homography = inv(loadtxt(args.homography))

storage.createBoundingBoxTable(args.databaseFilename, homography)

