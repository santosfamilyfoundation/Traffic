#! /usr/bin/env python
'''Various utilities to load data saved by the UBC tool(s)'''

import utils, events, storage
from moving import MovingObject, TimeInterval, Trajectory


fileTypeNames = ['feature',
                 'object',
                 'prototype',
                 'contoursequence']

severityIndicatorNames = ['Distance',
                          'Collision Course Cosine',
                          'Velocity Cosine',
                          'Speed Differential',
                          'Collision Probability',
                          'Severity Index',
                          'Time to Collision']

userTypeNames = ['car',
                 'pedestrian',
                 'twowheels',
                 'bus',
                 'truck']

# severityIndicator = {'Distance': 0,
#                      'Cosine': 1,
#                      'Velocity Cosine': 2,
#                      'Speed Differential': 3,
#                      'Collision Probability': 4,
#                      'Severity Index': 5,
#                      'TTC': 6}

mostSevereIsMax = [False, 
                   False, 
                   True, 
                   True, 
                   True, 
                   True, 
                   False]

ignoredValue = [None, None, None, None, None, None, -1]

def getFileType(s):
    'Finds the type in fileTypeNames'
    for fileType in fileTypeNames:
        if s.find(fileType)>0:
            return fileType
    return ''

def isFileType(s, fileType):
    return (s.find(fileType)>0)

def saveTrajectoryUserTypes(inFilename, outFilename, objects):
    '''The program saves the objects, 
    by just copying the corresponding trajectory and velocity data
    from the inFilename, and saving the characteristics in objects (first line)
    into outFilename'''
    infile = storage.openCheck(inFilename)
    outfile = storage.openCheck(outFilename,'w')

    if (inFilename.find('features') >= 0) or (not infile) or (not outfile):
        return

    lines = storage.getLines(infile)
    objNum = 0 # in inFilename
    while lines != []:
        # find object in objects (index i)
        i = 0
        while (i<len(objects)) and (objects[i].num != objNum):
            i+=1

        if i<len(objects):
            l = lines[0].split(' ')
            l[3] = str(objects[i].userType)
            outfile.write(' '.join(l)+'\n')
            for l in lines[1:]:
                outfile.write(l+'\n')
            outfile.write(utils.delimiterChar+'\n')
        # next object
        objNum += 1
        lines = storage.getLines(infile)

    print('read {0} objects'.format(objNum))

def modifyTrajectoryFile(modifyLines, filenameIn, filenameOut):
    '''Reads filenameIn, replaces the lines with the result of modifyLines and writes the result in filenameOut'''
    fileIn = storage.openCheck(filenameIn, 'r', True)
    fileOut = storage.openCheck(filenameOut, "w", True)

    lines = storage.getLines(fileIn)
    trajNum = 0
    while (lines != []):
        modifiedLines = modifyLines(trajNum, lines)
        if modifiedLines:
            for l in modifiedLines:
                fileOut.write(l+"\n")
            fileOut.write(utils.delimiterChar+"\n")
        lines = storage.getLines(fileIn)
        trajNum += 1
         
    fileIn.close()
    fileOut.close()

def copyTrajectoryFile(keepTrajectory, filenameIn, filenameOut):
    '''Reads filenameIn, keeps the trajectories for which the function keepTrajectory(trajNum, lines) is True
    and writes the result in filenameOut'''
    fileIn = storage.openCheck(filenameIn, 'r', True)
    fileOut = storage.openCheck(filenameOut, "w", True)

    lines = storage.getLines(fileIn)
    trajNum = 0
    while (lines != []):
        if keepTrajectory(trajNum, lines):
            for l in lines:
                fileOut.write(l+"\n")
            fileOut.write(utils.delimiterChar+"\n")
        lines = storage.getLines(fileIn)
        trajNum += 1
        
    fileIn.close()
    fileOut.close()

def loadTrajectories(filename, nObjects = -1):
    '''Loads trajectories'''

    file = storage.openCheck(filename)
    if (not file):
        return []

    objects = []
    objNum = 0
    objectType = getFileType(filename)
    lines = storage.getLines(file)
    while (lines != []) and ((nObjects<0) or (objNum<nObjects)):
        l = lines[0].split(' ')
        parsedLine = [int(n) for n in l[:4]]
        obj = MovingObject(num = objNum, timeInterval = TimeInterval(parsedLine[1],parsedLine[2]))
        #add = True
        if len(lines) >= 3:
            obj.positions = Trajectory.load(lines[1], lines[2])
            if len(lines) >= 5:
                obj.velocities = Trajectory.load(lines[3], lines[4])
                if objectType == 'object':
                    obj.userType = parsedLine[3]
                    obj.nObjects = float(l[4])
                    obj.featureNumbers = [int(n) for n in l[5:]]
                    
                    # load contour data if available
                    if len(lines) >= 6:
                        obj.contourType = utils.line2Floats(lines[6])
                        obj.contourOrigins = Trajectory.load(lines[7], lines[8])
                        obj.contourSizes = Trajectory.load(lines[9], lines[10])
                elif objectType == 'prototype':
                    obj.userType = parsedLine[3]
                    obj.nMatchings = int(l[4])

        if len(lines) != 2:
            objects.append(obj)
            objNum+=1
        else:
            print("Error two lines of data for feature %d"%(f.num))

        lines = storage.getLines(file)

    file.close()
    return objects
   
def getFeatureNumbers(objects):
    featureNumbers=[]
    for o in objects:
        featureNumbers += o.featureNumbers
    return featureNumbers

def loadInteractions(filename, nInteractions = -1):
    'Loads interactions from the old UBC traffic event format'
    from events import Interaction 
    from indicators import SeverityIndicator
    file = storage.openCheck(filename)
    if (not file):
        return []

    interactions = []
    interactionNum = 0
    lines = storage.getLines(file)
    while (lines != []) and ((nInteractions<0) or (interactionNum<nInteractions)):
        parsedLine = [int(n) for n in lines[0].split(' ')]
        inter = Interaction(interactionNum, TimeInterval(parsedLine[1],parsedLine[2]), parsedLine[3], parsedLine[4], categoryNum = parsedLine[5])
        
        indicatorFrameNums = [int(n) for n in lines[1].split(' ')]
        for indicatorNum,line in enumerate(lines[2:]):
            values = {}
            for i,v in enumerate([float(n) for n in line.split(' ')]):
                if not ignoredValue[indicatorNum] or v != ignoredValue[indicatorNum]:
                    values[indicatorFrameNums[i]] = v
            inter.addIndicator(SeverityIndicator(severityIndicatorNames[indicatorNum], values, None, mostSevereIsMax[indicatorNum]))

        interactions.append(inter)
        interactionNum+=1
        lines = storage.getLines(file)

    file.close()
    return interactions

def loadCollisionPoints(filename, nPoints = -1):
    '''Loads collision points and returns a dict
    with keys as a pair of the numbers of the two interacting objects'''
    file = storage.openCheck(filename)
    if (not file):
        return []

    points = {}
    num = 0
    lines = storage.getLines(file)
    while (lines != []) and ((nPoints<0) or (num<nPoints)):
        parsedLine = [int(n) for n in lines[0].split(' ')]
        protagonistNums = (parsedLine[0], parsedLine[1])
        points[protagonistNums] = [[float(n) for n in lines[1].split(' ')],
                                   [float(n) for n in lines[2].split(' ')]]

        num+=1
        lines = storage.getLines(file)

    file.close()
    return points
