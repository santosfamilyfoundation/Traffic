#! /usr/bin/env python

import sys, argparse, datetime

import storage, cvutils, utils

import matplotlib.pylab as pylab
import matplotlib.pyplot as plt
import numpy as np


annotations = pylab.csv2rec(sys.argv[1])

frameRate = 30
dirname = "/home/nicolas/Research/Data/montreal/infractions-pietons/"
videoDirnames = {'amherst': '2011-06-22-sherbrooke-amherst/',
                 'iberville': '2011-06-28-sherbrooke-iberville/'}

# for amherst, subtract 40 seconds: add a delta

for annotation in annotations:
    video = annotation['video_name'].lower()
    print('{} {}'.format(annotation['conflict_start_time'], annotation['conflict_end_time']))
    print(annotation['road_user_1']+' '+annotation['road_user_2']+' '+annotation['conflict_quality'])
    print(annotation['comments'])
    cvutils.playVideo(dirname+videoDirnames[video]+video+'-{}.avi'.format(annotation['video_start_time']), utils.timeToFrames(annotation['conflict_start_time']+datetime.timedelta(seconds=-40), frameRate), frameRate, True, False, annotation['road_user_1']+' '+annotation['road_user_2']+' '+annotation['conflict_quality'])
