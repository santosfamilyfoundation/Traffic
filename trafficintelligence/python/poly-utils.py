#! /usr/bin/env python
'''Various utilities to load data saved by the POLY new output(s)'''

from moving import  TimeInterval
from indicators import SeverityIndicator

import sys, utils
import numpy as np


def loadNewInteractions(videoFilename,interactionType,dirname, extension, indicatorsNames, roaduserNum1,roaduserNum2, selectedIndicators=[]):
    '''Loads interactions from the POLY traffic event format'''
    from events import Interaction 
    filename= dirname + videoFilename + extension
    #filename= dirname + interactionType+ '-' + videoFilename + extension # case of min distance todo: change the saving format to be matched with all outputs
    file = utils.openCheck(filename)
    if (not file):
        return []
    #interactions = []
    interactionNum = 0
    data= np.loadtxt(filename)
    indicatorFrameNums= data[:,0]
    inter = Interaction(interactionNum, TimeInterval(indicatorFrameNums[0],indicatorFrameNums[-1]), roaduserNum1, roaduserNum2) 
    inter.addVideoFilename(videoFilename)
    inter.addInteractionType(interactionType)
    for key in indicatorsNames.keys():
        values= {}
        for i,t in enumerate(indicatorFrameNums):
            values[t] = data[i,key]
        inter.addIndicator(SeverityIndicator(indicatorsNames[key], values))
    if selectedIndicators !=[]:
        values= {}
        for i,t in enumerate(indicatorFrameNums):
            values[t] = [data[i,index] for index in selectedIndicators]
        inter.addIndicator(SeverityIndicator('selectedIndicators', values))    
        
    #interactions.append(inter)
    file.close()
    #return interactions
    return inter

# Plotting results

frameRate = 15.

# To run in directory that contains the directories that contain the results (Miss-xx and Incident-xx)
#dirname = '/home/nicolas/Research/Data/kentucky-db/'

interactingRoadUsers = {'Miss/0404052336': [(0,3)] # 0,2 and 1 vs 3
                        #,
                        #'Incident/0306022035': [(1,3)]
                        #,
                        #'Miss/0208030956': [(4,5),(5,7)]
                        }


def getIndicatorName(filename, withUnit = False):
    if withUnit:
        unit = ' (s)'
    else:
        unit = ''
    if 'collision-point' in filename:
        return 'TTC'+unit
    elif 'crossing' in filename:
        return 'pPET'+unit
    elif 'probability' in filename:
        return 'P(UEA)'

def getMethodName(fileprefix):
    if fileprefix == 'constant-velocity':
        return 'Con. Vel.'
    elif fileprefix == 'normal-adaptation':
        return 'Norm. Ad.'
    elif fileprefix == 'point-set':
        return 'Pos. Set'
    elif fileprefix == 'evasive-action':
        return 'Ev. Act.'
    elif fileprefix == 'point-set-evasive-action':
        return 'Pos. Set'

indicator2TimeIdx = {'TTC':2,'pPET':2, 'P(UEA)':3}

def getDataAtInstant(data, i):
    return data[data[:,2] == i]

def getPointsAtInstant(data, i):
    return getDataAtInstant(i)[3:5]

def getIndicator(data, roadUserNumbers, indicatorName):
    if data.ndim ==1:
        data.shape = (1,data.shape[0])

    # find the order for the roadUserNumbers
    uniqueObj1 = np.unique(data[:,0])
    uniqueObj2 = np.unique(data[:,1])
    found = False
    if roadUserNumbers[0] in uniqueObj1 and roadUserNumbers[1] in uniqueObj2:
        objNum1 = roadUserNumbers[0]
        objNum2 = roadUserNumbers[1]
        found = True
    if roadUserNumbers[1] in uniqueObj1 and roadUserNumbers[0] in uniqueObj2:
        objNum1 = roadUserNumbers[1]
        objNum2 = roadUserNumbers[0]
        found = True

    # get subset of data for road user numbers
    if found:
        roadUserData = data[np.logical_and(data[:,0] == objNum1, data[:,1] == objNum2),:]
        if roadUserData.size > 0:
            time = np.unique(roadUserData[:,indicator2TimeIdx[indicatorName]])
            values = {}
            if indicatorName == 'P(UEA)':
                tmp = roadUserData[:,4]
                for k,v in zip(time, tmp):
                    values[k]=v
                return SeverityIndicator(indicatorName, values, mostSevereIsMax = False, maxValue = 1.), roadUserData
            else:
                for i in xrange(time[0],time[-1]+1):
                    try:
                        tmp = getDataAtInstant(roadUserData, i)
                        values[i] = np.sum(tmp[:,5]*tmp[:,6])/np.sum(tmp[:,5])/frameRate
                    except IOError:
                        values[i] = np.inf
                return SeverityIndicator(indicatorName, values, mostSevereIsMax = False), roadUserData
    return None, None
