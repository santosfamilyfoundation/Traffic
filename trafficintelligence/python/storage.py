#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''Various utilities to save and load data'''

import utils, moving, events, indicators, shutil
from base import VideoFilenameAddable

import sqlite3, logging
from numpy import log, min as npmin, max as npmax, round as npround, array, sum as npsum, loadtxt
from pandas import read_csv, merge


commentChar = '#'

delimiterChar = '%';

ngsimUserTypes = {'twowheels':1,
                  'car':2,
                  'truck':3}

#########################
# Sqlite
#########################

# utils
def printDBError(error):
    print('DB Error: {}'.format(error))

def dropTables(connection, tableNames):
    'deletes the table with names in tableNames'
    try:
        cursor = connection.cursor()
        for tableName in tableNames:
            cursor.execute('DROP TABLE IF EXISTS '+tableName)
    except sqlite3.OperationalError as error:
        printDBError(error)

def tableExists(filename, tableName):
    'indicates if the table exists in the database'
    try:
        connection = sqlite3.connect(filename)
        cursor = connection.cursor()
        cursor.execute('SELECT COUNT(*) FROM SQLITE_MASTER WHERE type = \'table\' AND name = \''+tableName+'\'')
        return cursor.fetchone()[0] == 1
    except sqlite3.OperationalError as error:
        printDBError(error)        

def createIndex(connection, tableName, columnName, unique = False):
    '''Creates an index for the column in the table
    I will make querying with a condition on this column faster'''
    try:
        #connection = sqlite3.connect(filename)
        cursor = connection.cursor()
        s = "CREATE "
        if unique:
            s += "UNIQUE "
        cursor.execute(s+"INDEX IF NOT EXISTS "+tableName+"_"+columnName+"_index ON "+tableName+"("+columnName+")")
        connection.commit()
        #connection.close()
    except sqlite3.OperationalError as error:
        printDBError(error)

def getNumberRowsTable(connection, tableName, columnName = None):
    '''Returns the number of rows for the table
    If columnName is not None, means we want the number of distinct values for that column
    (otherwise, we can just count(*))'''
    try:
        cursor = connection.cursor()
        if columnName is None:
            cursor.execute("SELECT COUNT(*) from "+tableName)
        else:
            cursor.execute("SELECT COUNT(DISTINCT "+columnName+") from "+tableName)
        return cursor.fetchone()[0]
    except sqlite3.OperationalError as error:
        printDBError(error)

def getMinMax(connection, tableName, columnName, minmax):
    '''Returns max/min or both for given column in table
    minmax must be string max, min or minmax'''
    try:
        cursor = connection.cursor()
        if minmax == 'min' or minmax == 'max':
            cursor.execute("SELECT "+minmax+"("+columnName+") from "+tableName)
        elif minmax == 'minmax':
            cursor.execute("SELECT MIN("+columnName+"), MAX("+columnName+") from "+tableName)
        else:
            print("Argument minmax unknown: {}".format(minmax))
        return cursor.fetchone()[0]
    except sqlite3.OperationalError as error:
        printDBError(error)

# TODO: add test if database connection is open
# IO to sqlite
def writeTrajectoriesToSqlite(objects, outputFilename, trajectoryType, objectNumbers = -1):
    """
    This function writers trajectories to a specified sqlite file
    @param[in] objects -> a list of trajectories
    @param[in] trajectoryType -
    @param[out] outputFilename -> the .sqlite file containting the written objects
    @param[in] objectNumber : number of objects loaded
    """
    connection = sqlite3.connect(outputFilename)
    cursor = connection.cursor()

    schema = "CREATE TABLE IF NOT EXISTS \"positions\"(trajectory_id INTEGER,frame_number INTEGER, x_coordinate REAL, y_coordinate REAL, PRIMARY KEY(trajectory_id, frame_number))"
    cursor.execute(schema)

    trajectory_id = 0
    frame_number = 0
    if trajectoryType == 'feature':
        if type(objectNumbers) == int and objectNumbers == -1:
            for trajectory in objects:
                trajectory_id += 1
                frame_number = 0
                for position in trajectory.getPositions():
                    frame_number += 1
                    query = "insert into positions (trajectory_id, frame_number, x_coordinate, y_coordinate) values (?,?,?,?)"
                    cursor.execute(query,(trajectory_id,frame_number,position.x,position.y))
                    
    connection.commit()
    connection.close()
    

def loadPrototypeMatchIndexesFromSqlite(filename):
    """
    This function loads the prototypes table in the database of name <filename>.
    It returns a list of tuples representing matching ids : [(prototype_id, matched_trajectory_id),...]
    """
    matched_indexes = []

    connection = sqlite3.connect(filename)
    cursor = connection.cursor()

    try:
        cursor.execute('SELECT * from prototypes order by prototype_id, trajectory_id_matched')
    except sqlite3.OperationalError as error:
        printDBError(error)
        return []

    for row in cursor:
        matched_indexes.append((row[0],row[1]))

    connection.close()
    return matched_indexes

def getObjectCriteria(objectNumbers):
    if objectNumbers is None:
        query = ''
    elif type(objectNumbers) == int:
        query = 'between 0 and {0}'.format(objectNumbers-1)
    elif type(objectNumbers) == list:
        query = 'in ('+', '.join([str(n) for n in objectNumbers])+')'
    else:
        print('objectNumbers {} are not a known type ({})'.format(objectNumbers, type(objectNumbers)))
        query = ''
    return query

def loadTrajectoriesFromTable(connection, tableName, trajectoryType, objectNumbers = None):
    '''Loads trajectories (in the general sense) from the given table
    can be positions or velocities

    returns a moving object'''
    cursor = connection.cursor()

    try:
        objectCriteria = getObjectCriteria(objectNumbers)
        queryStatement = None
        if trajectoryType == 'feature':
            queryStatement = 'SELECT * from '+tableName
            if objectNumbers is not None:
                queryStatement += ' WHERE trajectory_id '+objectCriteria
            queryStatement += ' ORDER BY trajectory_id, frame_number'
        elif trajectoryType == 'object':
            queryStatement = 'SELECT OF.object_id, P.frame_number, avg(P.x_coordinate), avg(P.y_coordinate) from '+tableName+' P, objects_features OF WHERE P.trajectory_id = OF.trajectory_id'
            if objectNumbers is not None:
                queryStatement += ' and OF.object_id '+objectCriteria
            queryStatement += ' GROUP BY OF.object_id, P.frame_number ORDER BY OF.object_id, P.frame_number'
        elif trajectoryType in ['bbtop', 'bbbottom']:
            if trajectoryType == 'bbtop':
                corner = 'top_left'
            elif trajectoryType == 'bbbottom':
                corner = 'bottom_right'
            queryStatement = 'SELECT object_id, frame_number, x_'+corner+', y_'+corner+' FROM '+tableName
            if objectNumbers is not None:
                queryStatement += ' WHERE object_id '+objectCriteria
            queryStatement += ' ORDER BY object_id, frame_number'
        else:
            print('no trajectory type was chosen')
        if queryStatement is not None:
            cursor.execute(queryStatement)
            logging.debug(queryStatement)
    except sqlite3.OperationalError as error:
        printDBError(error)
        return []

    objId = -1
    obj = None
    objects = []
    for row in cursor:
        if row[0] != objId:
            objId = row[0]
            if obj is not None and obj.length() == obj.positions.length():
                objects.append(obj)
            elif obj is not None:
                print('Object {} is missing {} positions'.format(obj.getNum(), int(obj.length())-obj.positions.length()))
            obj = moving.MovingObject(row[0], timeInterval = moving.TimeInterval(row[1], row[1]), positions = moving.Trajectory([[row[2]],[row[3]]]))
        else:
            obj.timeInterval.last = row[1]
            obj.positions.addPositionXY(row[2],row[3])

    if obj is not None and obj.length() == obj.positions.length():
        objects.append(obj)
    elif obj is not None:
        print('Object {} is missing {} positions'.format(obj.getNum(), int(obj.length())-obj.positions.length()))

    return objects

def loadUserTypesFromTable(cursor, trajectoryType, objectNumbers):
    objectCriteria = getObjectCriteria(objectNumbers)
    queryStatement = 'SELECT object_id, road_user_type from objects'
    if objectNumbers is not None:
        queryStatement += ' WHERE object_id '+objectCriteria
    cursor.execute(queryStatement)
    userTypes = {}
    for row in cursor:
        userTypes[row[0]] = row[1]
    return userTypes

def loadTrajectoriesFromSqlite(filename, trajectoryType, objectNumbers = None, withFeatures = False):
    '''Loads the first objectNumbers objects or the indices in objectNumbers from the database'''
    connection = sqlite3.connect(filename)

    objects = loadTrajectoriesFromTable(connection, 'positions', trajectoryType, objectNumbers)
    objectVelocities = loadTrajectoriesFromTable(connection, 'velocities', trajectoryType, objectNumbers)

    if len(objectVelocities) > 0:
        for o,v in zip(objects, objectVelocities):
            if o.getNum() == v.getNum():
                o.velocities = v.positions
                o.velocities.duplicateLastPosition() # avoid having velocity shorter by one position than positions
            else:
                print('Could not match positions {0} with velocities {1}'.format(o.getNum(), v.getNum()))

    if trajectoryType == 'object':
        cursor = connection.cursor()
        try:
            # attribute feature numbers to objects
            objectCriteria = getObjectCriteria(objectNumbers)
            queryStatement = 'SELECT P.trajectory_id, OF.object_id from positions P, objects_features OF WHERE P.trajectory_id = OF.trajectory_id'
            if objectNumbers is not None:
                queryStatement += ' and OF.object_id '+objectCriteria
            queryStatement += ' group by P.trajectory_id order by OF.object_id' # order is important to group all features per object
            cursor.execute(queryStatement) 
            logging.debug(queryStatement)

            featureNumbers = {}
            for row in cursor:
                objId = row[1]
                if objId not in featureNumbers:
                    featureNumbers[objId] = [row[0]]
                else:
                    featureNumbers[objId].append(row[0])
                    
            for obj in objects:
                obj.featureNumbers = featureNumbers[obj.getNum()]

            # load userType
            userTypes = loadUserTypesFromTable(cursor, trajectoryType, objectNumbers)
            for obj in objects:
                obj.userType = userTypes[obj.getNum()]

            if withFeatures:
                nFeatures = 0
                for obj in objects:
                    nFeatures = max(nFeatures, max(obj.featureNumbers))
                features = loadTrajectoriesFromSqlite(filename, 'feature', nFeatures+1)
                for obj in objects:
                    obj.setFeatures(features)
             
        except sqlite3.OperationalError as error:
            printDBError(error)
            objects = []

    connection.close()
    return objects

def savePrototypesToSqlite(filename, prototypes, trajectoryType = 'feature'):
    'Work in progress, do not use'
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    try:
        cursor.execute('CREATE TABLE IF NOT EXISTS prototypes (id INTEGER PRIMARY KEY, object_id INTEGER, trajectory_id INTEGER, nMatchings INTEGER, FOREIGN KEY(object_id) REFERENCES objects(id), FOREIGN KEY(trajectory_id) REFERENCES positions(trajectory_id))')
        #for inter in interactions:
        #    saveInteraction(cursor, inter)
    except sqlite3.OperationalError as error:
        printDBError(error)
    connection.commit()
    connection.close()

def loadPrototypesFromSqlite(filename):
    pass

def loadGroundTruthFromSqlite(filename, gtType = 'bb', gtNumbers = None):
    'Loads bounding box annotations (ground truth) from an SQLite '
    connection = sqlite3.connect(filename)
    gt = []

    if gtType == 'bb':
        topCorners = loadTrajectoriesFromTable(connection, 'bounding_boxes', 'bbtop', gtNumbers)
        bottomCorners = loadTrajectoriesFromTable(connection, 'bounding_boxes', 'bbbottom', gtNumbers)
        userTypes = loadUserTypesFromTable(connection.cursor(), 'object', gtNumbers) # string format is same as object
        
        for t, b in zip(topCorners, bottomCorners):
            num = t.getNum()
            if t.getNum() == b.getNum():
                annotation = moving.BBAnnotation(num, t.getTimeInterval(), t, b, userTypes[num])
                gt.append(annotation)
    else:
        print ('Unknown type of annotation {}'.format(gtType))

    connection.close()
    return gt

def deleteFromSqlite(filename, dataType):
    'Deletes (drops) some tables in the filename depending on type of data'
    import os
    if os.path.isfile(filename):
        connection = sqlite3.connect(filename)
        if dataType == 'object':
            dropTables(connection, ['objects', 'objects_features'])
        elif dataType == 'interaction':
            dropTables(connection, ['interactions', 'indicators'])
        elif dataType == 'bb':
            dropTables(connection, ['bounding_boxes'])
        else:
            print('Unknown data type {} to delete from database'.format(dataType))
        connection.close()
    else:
        print('{} does not exist'.format(filename))

def createInteractionTable(cursor):
    cursor.execute('CREATE TABLE IF NOT EXISTS interactions (id INTEGER PRIMARY KEY, object_id1 INTEGER, object_id2 INTEGER, first_frame_number INTEGER, last_frame_number INTEGER, FOREIGN KEY(object_id1) REFERENCES objects(id), FOREIGN KEY(object_id2) REFERENCES objects(id))')

def createIndicatorTables(cursor):
    # cursor.execute('CREATE TABLE IF NOT EXISTS indicators (id INTEGER PRIMARY KEY, interaction_id INTEGER, indicator_type INTEGER, FOREIGN KEY(interaction_id) REFERENCES interactions(id))')
    # cursor.execute('CREATE TABLE IF NOT EXISTS indicator_values (indicator_id INTEGER, frame_number INTEGER, value REAL, FOREIGN KEY(indicator_id) REFERENCES indicators(id), PRIMARY KEY(indicator_id, frame_number))')
    cursor.execute('CREATE TABLE IF NOT EXISTS indicators (interaction_id INTEGER, indicator_type INTEGER, frame_number INTEGER, value REAL, FOREIGN KEY(interaction_id) REFERENCES interactions(id), PRIMARY KEY(interaction_id, indicator_type, frame_number))')

def saveInteraction(cursor, interaction):
    roadUserNumbers = list(interaction.getRoadUserNumbers())
    cursor.execute('INSERT INTO interactions VALUES({}, {}, {}, {}, {})'.format(interaction.getNum(), roadUserNumbers[0], roadUserNumbers[1], interaction.getFirstInstant(), interaction.getLastInstant()))

def saveInteractions(filename, interactions):
    'Saves the interactions in the table'
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    try:
        createInteractionTable(cursor)
        for inter in interactions:
            saveInteraction(cursor, inter)
    except sqlite3.OperationalError as error:
        printDBError(error)
    connection.commit()
    connection.close()

def saveIndicator(cursor, interactionNum, indicator):
    for instant in indicator.getTimeInterval():
        if indicator[instant]:
            cursor.execute('INSERT INTO indicators VALUES({}, {}, {}, {})'.format(interactionNum, events.Interaction.indicatorNameToIndices[indicator.getName()], instant, indicator[instant]))

def saveIndicators(filename, interactions, indicatorNames = events.Interaction.indicatorNames):
    'Saves the indicator values in the table'
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    try:
        createInteractionTable(cursor)
        createIndicatorTables(cursor)
        for inter in interactions:
            saveInteraction(cursor, inter)
            for indicatorName in indicatorNames:
                indicator = inter.getIndicator(indicatorName)
                if indicator is not None:
                    saveIndicator(cursor, inter.getNum(), indicator)
    except sqlite3.OperationalError as error:
        printDBError(error)
    connection.commit()
    connection.close()

def loadInteractions(filename):
    '''Loads interaction and their indicators
    
    TODO choose the interactions to load'''
    interactions = []
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    try:
        cursor.execute('select INT.id, INT.object_id1, INT.object_id2, INT.first_frame_number, INT.last_frame_number, IND.indicator_type, IND.frame_number, IND.value from interactions INT, indicators IND WHERE INT.id = IND.interaction_id ORDER BY INT.id, IND.indicator_type, IND.frame_number')
        interactionNum = -1
        indicatorTypeNum = -1
        tmpIndicators = {}
        for row in cursor:
            if row[0] != interactionNum:
                interactionNum = row[0]
                interactions.append(events.Interaction(interactionNum, moving.TimeInterval(row[3],row[4]), row[1], row[2]))
                interactions[-1].indicators = {}
            if indicatorTypeNum != row[5] or row[0] != interactionNum:
                indicatorTypeNum = row[5]
                indicatorName = events.Interaction.indicatorNames[indicatorTypeNum]
                indicatorValues = {row[6]:row[7]}
                interactions[-1].indicators[indicatorName] = indicators.SeverityIndicator(indicatorName, indicatorValues, mostSevereIsMax = not indicatorName in events.Interaction.timeIndicators)
            else:
                indicatorValues[row[6]] = row[7]
                interactions[-1].indicators[indicatorName].timeInterval.last = row[6]
    except sqlite3.OperationalError as error:
        printDBError(error)
        return []
    connection.close()
    return interactions
# load first and last object instants
# CREATE TEMP TABLE IF NOT EXISTS object_instants AS SELECT OF.object_id, min(frame_number) as first_instant, max(frame_number) as last_instant from positions P, objects_features OF WHERE P.trajectory_id = OF.trajectory_id group by OF.object_id order by OF.object_id

def createBoundingBoxTable(filename, invHomography = None):
    '''Create the table to store the object bounding boxes in image space
    '''
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    try:
        cursor.execute('CREATE TABLE IF NOT EXISTS bounding_boxes (object_id INTEGER, frame_number INTEGER, x_top_left REAL, y_top_left REAL, x_bottom_right REAL, y_bottom_right REAL,  PRIMARY KEY(object_id, frame_number))')
        cursor.execute('INSERT INTO bounding_boxes SELECT object_id, frame_number, min(x), min(y), max(x), max(y) from '
              '(SELECT object_id, frame_number, (x*{}+y*{}+{})/w as x, (x*{}+y*{}+{})/w as y from '
              '(SELECT OF.object_id, P.frame_number, P.x_coordinate as x, P.y_coordinate as y, P.x_coordinate*{}+P.y_coordinate*{}+{} as w from positions P, objects_features OF WHERE P.trajectory_id = OF.trajectory_id)) '.format(invHomography[0,0], invHomography[0,1], invHomography[0,2], invHomography[1,0], invHomography[1,1], invHomography[1,2], invHomography[2,0], invHomography[2,1], invHomography[2,2])+
              'GROUP BY object_id, frame_number')
    except sqlite3.OperationalError as error:
        printDBError(error)
    connection.commit()
    connection.close()

def loadBoundingBoxTableForDisplay(filename):
    '''Loads bounding boxes from bounding_boxes table for display over trajectories'''
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    boundingBoxes = {} # list of bounding boxes for each instant
    try:
        cursor.execute('SELECT name FROM sqlite_master WHERE type=\'table\' AND name=\'bounding_boxes\'')
        result = [row for row in cursor]
        if len(result) > 0:
            cursor.execute('SELECT * FROM bounding_boxes')
            for row in cursor:
                boundingBoxes.setdefault(row[1], []).append([moving.Point(row[2], row[3]), moving.Point(row[4], row[5])])
    except sqlite3.OperationalError as error:
        printDBError(error)
        return boundingBoxes
    connection.close()
    return boundingBoxes

#########################
# saving and loading for scene interpretation (Mohamed Gomaa Mohamed's PhD)
#########################

def writeFeaturesToSqlite(objects, outputFilename, trajectoryType, objectNumbers = -1):
    '''write features trajectories maintain trajectory ID,velocities dataset  '''
    connection = sqlite3.connect(outputFilename)
    cursor = connection.cursor()

    cursor.execute("CREATE TABLE IF NOT EXISTS \"positions\"(trajectory_id INTEGER,frame_number INTEGER, x_coordinate REAL, y_coordinate REAL, PRIMARY KEY(trajectory_id, frame_number))")
    cursor.execute("CREATE TABLE IF NOT EXISTS \"velocities\"(trajectory_id INTEGER,frame_number INTEGER, x_coordinate REAL, y_coordinate REAL, PRIMARY KEY(trajectory_id, frame_number))")
    
    if trajectoryType == 'feature':
        if type(objectNumbers) == int and objectNumbers == -1:
            for trajectory in objects:
                trajectory_id = trajectory.num
                frame_number = trajectory.timeInterval.first
                for position,velocity in zip(trajectory.getPositions(),trajectory.getVelocities()):
                    cursor.execute("insert into positions (trajectory_id, frame_number, x_coordinate, y_coordinate) values (?,?,?,?)",(trajectory_id,frame_number,position.x,position.y))
                    cursor.execute("insert into velocities (trajectory_id, frame_number, x_coordinate, y_coordinate) values (?,?,?,?)",(trajectory_id,frame_number,velocity.x,velocity.y))
                    frame_number += 1
                    
    connection.commit()
    connection.close()
    
def writePrototypesToSqlite(prototypes,nMatching, outputFilename):
    """ prototype dataset is a dictionary with  keys== routes, values== prototypes Ids """
    connection = sqlite3.connect(outputFilename)
    cursor = connection.cursor()

    cursor.execute("CREATE TABLE IF NOT EXISTS \"prototypes\"(prototype_id INTEGER,routeIDstart INTEGER,routeIDend INTEGER, nMatching INTEGER, PRIMARY KEY(prototype_id))")
    
    for route in prototypes.keys():
        if prototypes[route]!=[]:
            for i in prototypes[route]:
                cursor.execute("insert into prototypes (prototype_id, routeIDstart,routeIDend, nMatching) values (?,?,?,?)",(i,route[0],route[1],nMatching[route][i]))
                    
    connection.commit()
    connection.close()
    
def readPrototypesFromSqlite(filename):
    """
    This function loads the prototype file in the database 
    It returns a dictionary for prototypes for each route and nMatching
    """
    prototypes = {}
    nMatching={}

    connection = sqlite3.connect(filename)
    cursor = connection.cursor()

    try:
        cursor.execute('SELECT * from prototypes order by prototype_id, routeIDstart,routeIDend, nMatching')
    except sqlite3.OperationalError as error:
        utils.printDBError(error)
        return []

    for row in cursor:
        route=(row[1],row[2])
        if route not in prototypes.keys():
            prototypes[route]=[]
        prototypes[route].append(row[0])
        nMatching[row[0]]=row[3]

    connection.close()
    return prototypes,nMatching
    
def writeLabelsToSqlite(labels, outputFilename):
    """ labels is a dictionary with  keys: routes, values: prototypes Ids
    """
    connection = sqlite3.connect(outputFilename)
    cursor = connection.cursor()

    cursor.execute("CREATE TABLE IF NOT EXISTS \"labels\"(object_id INTEGER,routeIDstart INTEGER,routeIDend INTEGER, prototype_id INTEGER, PRIMARY KEY(object_id))")
    
    for route in labels.keys():
        if labels[route]!=[]:
            for i in labels[route]:
                for j in labels[route][i]:
                    cursor.execute("insert into labels (object_id, routeIDstart,routeIDend, prototype_id) values (?,?,?,?)",(j,route[0],route[1],i))
                    
    connection.commit()
    connection.close()
    
def loadLabelsFromSqlite(filename):
    labels = {}

    connection = sqlite3.connect(filename)
    cursor = connection.cursor()

    try:
        cursor.execute('SELECT * from labels order by object_id, routeIDstart,routeIDend, prototype_id')
    except sqlite3.OperationalError as error:
        utils.printDBError(error)
        return []

    for row in cursor:
        route=(row[1],row[2])
        p=row[3]
        if route not in labels.keys():
            labels[route]={}
        if p not in labels[route].keys():
            labels[route][p]=[]
        labels[route][p].append(row[0])

    connection.close()
    return labels

def writeSpeedPrototypeToSqlite(prototypes,nmatching, outFilename):
    """ to match the format of second layer prototypes"""
    connection = sqlite3.connect(outFilename)
    cursor = connection.cursor()

    cursor.execute("CREATE TABLE IF NOT EXISTS \"speedprototypes\"(spdprototype_id INTEGER,prototype_id INTEGER,routeID_start INTEGER, routeID_end INTEGER, nMatching INTEGER, PRIMARY KEY(spdprototype_id))")
    
    for route in prototypes.keys():
        if prototypes[route]!={}:
            for i in prototypes[route]:
                if prototypes[route][i]!= []:
                    for j in prototypes[route][i]:
                        cursor.execute("insert into speedprototypes (spdprototype_id,prototype_id, routeID_start, routeID_end, nMatching) values (?,?,?,?,?)",(j,i,route[0],route[1],nmatching[j]))
                    
    connection.commit()
    connection.close()
    
def loadSpeedPrototypeFromSqlite(filename):
    """
    This function loads the prototypes table in the database of name <filename>.
    """
    prototypes = {}
    nMatching={}
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()

    try:
        cursor.execute('SELECT * from speedprototypes order by spdprototype_id,prototype_id, routeID_start, routeID_end, nMatching')
    except sqlite3.OperationalError as error:
        utils.printDBError(error)
        return []

    for row in cursor:
        route=(row[2],row[3])
        if route not in prototypes.keys():
            prototypes[route]={}
        if row[1] not in prototypes[route].keys():
            prototypes[route][row[1]]=[]
        prototypes[route][row[1]].append(row[0])
        nMatching[row[0]]=row[4]

    connection.close()
    return prototypes,nMatching


def writeRoutesToSqlite(Routes, outputFilename):
    """ This function writes the activity path define by start and end IDs"""
    connection = sqlite3.connect(outputFilename)
    cursor = connection.cursor()

    cursor.execute("CREATE TABLE IF NOT EXISTS \"routes\"(object_id INTEGER,routeIDstart INTEGER,routeIDend INTEGER, PRIMARY KEY(object_id))")
    
    for route in Routes.keys():
        if Routes[route]!=[]:
            for i in Routes[route]:
                cursor.execute("insert into routes (object_id, routeIDstart,routeIDend) values (?,?,?)",(i,route[0],route[1]))
                    
    connection.commit()
    connection.close()
    
def loadRoutesFromSqlite(filename):
    Routes = {}

    connection = sqlite3.connect(filename)
    cursor = connection.cursor()

    try:
        cursor.execute('SELECT * from routes order by object_id, routeIDstart,routeIDend')
    except sqlite3.OperationalError as error:
        utils.printDBError(error)
        return []

    for row in cursor:
        route=(row[1],row[2])
        if route not in Routes.keys():
            Routes[route]=[]
        Routes[route].append(row[0])

    connection.close()
    return Routes

def setRoutes(filename, objects):
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    for obj in objects:
        cursor.execute('update objects set startRouteID = {} WHERE object_id = {}'.format(obj.startRouteID, obj.getNum()))
        cursor.execute('update objects set endRouteID = {} WHERE object_id = {}'.format(obj.endRouteID, obj.getNum()))        
    connection.commit()
    connection.close()

def setRoadUserTypes(filename, objects):
    '''Saves the user types of the objects in the sqlite database stored in filename
    The objects should exist in the objects table'''
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    for obj in objects:
        cursor.execute('update objects set road_user_type = {} WHERE object_id = {}'.format(obj.getUserType(), obj.getNum()))
    connection.commit()
    connection.close()

#########################
# txt files
#########################

def openCheck(filename, option = 'r', quitting = False):
    '''Open file filename in read mode by default
    and checks it is open'''
    try:
        return open(filename, option)
    except IOError:
        print 'File %s could not be opened.' % filename
        if quitting:
            from sys import exit
            exit()
        return None

def readline(f, commentCharacters = commentChar):
    '''Modified readline function to skip comments
    Can take a list of characters or a string (in will work in both)'''
    s = f.readline()
    while (len(s) > 0) and s[0] in commentCharacters:
        s = f.readline()
    return s.strip()

def getLines(f, delimiterChar = delimiterChar, commentCharacters = commentChar):
    '''Gets a complete entry (all the lines) in between delimiterChar.'''
    dataStrings = []
    s = readline(f, commentCharacters)
    while len(s) > 0 and s[0] != delimiterChar:
        dataStrings += [s.strip()]
        s = readline(f, commentCharacters)
    return dataStrings

def writeList(filename, l):
    f = openCheck(filename, 'w')
    for x in l:
        f.write('{}\n'.format(x))
    f.close()

def loadListStrings(filename, commentCharacters = commentChar):
    f = openCheck(filename, 'r')
    result = getLines(f, commentCharacters)
    f.close()
    return result

def getValuesFromINIFile(filename, option, delimiterChar = '=', commentCharacters = commentChar):
    values = []
    for l in loadListStrings(filename, commentCharacters):
        if l.startswith(option):
            values.append(l.split(delimiterChar)[1].strip())
    return values

class FakeSecHead(object):
    '''Add fake section header [asection]

    from http://stackoverflow.com/questions/2819696/parsing-properties-file-in-python/2819788#2819788
    use read_file in Python 3.2+
    '''
    def __init__(self, fp):
        self.fp = fp
        self.sechead = '[main]\n'

    def readline(self):
        if self.sechead:
            try: return self.sechead
            finally: self.sechead = None
        else: return self.fp.readline()

def generatePDLaneColumn(data):
    data['LANE'] = data['LANE\LINK\NO'].astype(str)+'_'+data['LANE\INDEX'].astype(str)

def convertTrajectoriesVissimToSqlite(filename):
    '''Relies on a system call to sqlite3
    sqlite3 [file.sqlite] < import_fzp.sql'''
    sqlScriptFilename = "import_fzp.sql"
    # create sql file
    out = openCheck(sqlScriptFilename, "w")
    out.write(".separator \";\"\n"+
              "CREATE TABLE IF NOT EXISTS curvilinear_positions (t REAL, trajectory_id INTEGER, link_id INTEGER, lane_id INTEGER, s_coordinate REAL, y_coordinate REAL, speed REAL, PRIMARY KEY (t, trajectory_id));\n"+
              ".import "+filename+" curvilinear_positions\n"+
              "DELETE FROM curvilinear_positions WHERE trajectory_id IS NULL OR trajectory_id = \"NO\";\n")
    out.close()
    # system call
    from subprocess import check_call
    out = openCheck("err.log", "w")
    check_call("sqlite3 "+utils.removeExtension(filename)+".sqlite < "+sqlScriptFilename, stderr = out, shell = True)
    out.close()
    shutil.os.remove(sqlScriptFilename)

def loadObjectNumbersInLinkFromVissimFile(filename, linkIds):
    '''Finds the ids of the objects that go through any of the link in the list linkIds'''
    connection = sqlite3.connect(filename)
    cursor = connection.cursor()
    queryStatement = 'SELECT DISTINCT trajectory_id FROM curvilinear_positions where link_id IN ('+','.join([str(id) for id in linkIds])+')'
    try:
        cursor.execute(queryStatement)
        return [row[0] for row in cursor]
    except sqlite3.OperationalError as error:
        printDBError(error)


def loadTrajectoriesFromVissimFile(filename, simulationStepsPerTimeUnit, objectNumbers = None, warmUpLastInstant = None, usePandas = False, nDecimals = 2, lowMemory = True):
    '''Reads data from VISSIM .fzp trajectory file
    simulationStepsPerTimeUnit is the number of simulation steps per unit of time used by VISSIM (second)
    for example, there seems to be 10 simulation steps per simulated second in VISSIM, 
    so simulationStepsPerTimeUnit should be 10, 
    so that all times correspond to the number of the simulation step (and can be stored as integers)
    
    Objects positions will be considered only after warmUpLastInstant 
    (if the object has no such position, it won't be loaded)

    Assumed to be sorted over time
    Warning: if reading from SQLite a limited number of objects, objectNumbers will be the maximum object id'''
    objects = {} # dictionary of objects index by their id

    if usePandas:
        data = read_csv(filename, delimiter=';', comment='*', header=0, skiprows = 1, low_memory = lowMemory)
        generatePDLaneColumn(data)
        data['TIME'] = data['$VEHICLE:SIMSEC']*simulationStepsPerTimeUnit
        if warmUpLastInstant is not None:
            data = data[data['TIME']>=warmUpLastInstant]
        grouped = data.loc[:,['NO','TIME']].groupby(['NO'], as_index = False)
        instants = grouped['TIME'].agg({'first': npmin, 'last': npmax})
        for row_index, row in instants.iterrows():
            objNum = int(row['NO'])
            tmp = data[data['NO'] == objNum]
            objects[objNum] = moving.MovingObject(num = objNum, timeInterval = moving.TimeInterval(row['first'], row['last']))
            # positions should be rounded to nDecimals decimals only
            objects[objNum].curvilinearPositions = moving.CurvilinearTrajectory(S = npround(tmp['POS'].tolist(), nDecimals), Y = npround(tmp['POSLAT'].tolist(), nDecimals), lanes = tmp['LANE'].tolist())
            if objectNumbers is not None and objectNumbers > 0 and len(objects) >= objectNumbers:
                objects.values()
    else:
        if filename.endswith(".fzp"):
            inputfile = openCheck(filename, quitting = True)
            line = readline(inputfile, '*$')
            while len(line) > 0:#for line in inputfile:
                data = line.strip().split(';')
                objNum = int(data[1])
                instant = float(data[0])*simulationStepsPerTimeUnit
                s = float(data[4])
                y = float(data[5])
                lane = data[2]+'_'+data[3]
                if objNum not in objects:
                    if warmUpLastInstant is None or instant >= warmUpLastInstant:
                        if objectNumbers is None or len(objects) < objectNumbers:
                            objects[objNum] = moving.MovingObject(num = objNum, timeInterval = moving.TimeInterval(instant, instant))
                            objects[objNum].curvilinearPositions = moving.CurvilinearTrajectory()
                if (warmUpLastInstant is None or instant >= warmUpLastInstant) and objNum in objects:
                    objects[objNum].timeInterval.last = instant
                    objects[objNum].curvilinearPositions.addPositionSYL(s, y, lane)
                line = readline(inputfile, '*$')
        elif filename.endswith(".sqlite"):
            connection = sqlite3.connect(filename)
            cursor = connection.cursor()
            queryStatement = 'SELECT t, trajectory_id, link_id, lane_id, s_coordinate, y_coordinate FROM curvilinear_positions'
            if objectNumbers is not None:
                queryStatement += ' WHERE trajectory_id '+getObjectCriteria(objectNumbers)
            queryStatement += ' ORDER BY trajectory_id, t'
            try:
                cursor.execute(queryStatement)
                for row in cursor:
                    objNum = row[1]
                    instant = row[0]*simulationStepsPerTimeUnit
                    s = row[4]
                    y = row[5]
                    lane = '{}_{}'.format(row[2], row[3])
                    if objNum not in objects:
                        if warmUpLastInstant is None or instant >= warmUpLastInstant:
                            if objectNumbers is None or len(objects) < objectNumbers:
                                objects[objNum] = moving.MovingObject(num = objNum, timeInterval = moving.TimeInterval(instant, instant))
                                objects[objNum].curvilinearPositions = moving.CurvilinearTrajectory()
                    if (warmUpLastInstant is None or instant >= warmUpLastInstant) and objNum in objects:
                        objects[objNum].timeInterval.last = instant
                        objects[objNum].curvilinearPositions.addPositionSYL(s, y, lane)
            except sqlite3.OperationalError as error:
                printDBError(error)
        else:
            print("File type of "+filename+" not supported (only .sqlite and .fzp files)")
        return objects.values()

def selectPDLanes(data, lanes = None):
    '''Selects the subset of data for the right lanes

    Lane format is a string 'x_y' where x is link index and y is lane index'''
    if lanes is not None:
        if 'LANE' not in data.columns:
            generatePDLaneColumn(data)
        indices = (data['LANE'] == lanes[0])
        for l in lanes[1:]:
            indices = indices | (data['LANE'] == l)
        return data[indices]
    else:
        return data

def countStoppedVehiclesVissim(filename, lanes = None, proportionStationaryTime = 0.7):
    '''Counts the number of vehicles stopped for a long time in a VISSIM trajectory file
    and the total number of vehicles

    Vehicles are considered finally stationary
    if more than proportionStationaryTime of their total time
    If lanes is not None, only the data for the selected lanes will be provided
    (format as string x_y where x is link index and y is lane index)'''
    if filename.endswith(".fzp"):
        columns = ['NO', '$VEHICLE:SIMSEC', 'POS']
        if lanes is not None:
            columns += ['LANE\LINK\NO', 'LANE\INDEX']
        data = read_csv(filename, delimiter=';', comment='*', header=0, skiprows = 1, usecols = columns, low_memory = lowMemory)
        data = selectPDLanes(data, lanes)
        data.sort(['$VEHICLE:SIMSEC'], inplace = True)

        nStationary = 0
        nVehicles = 0
        for name, group in data.groupby(['NO'], sort = False):
            nVehicles += 1
            positions = array(group['POS'])
            diff = positions[1:]-positions[:-1]
            if npsum(diff == 0.) >= proportionStationaryTime*(len(positions)-1):
                nStationary += 1
    elif filename.endswith(".sqlite"):
        # select trajectory_id, t, s_coordinate, speed from curvilinear_positions where trajectory_id between 1860 and 1870 and speed < 0.1
        # pb of the meaning of proportionStationaryTime in arterial network? Why proportion of existence time?
        pass
    else:
        print("File type of "+filename+" not supported (only .sqlite and .fzp files)")

    return nStationary, nVehicles

def countCollisionsVissim(filename, lanes = None, collisionTimeDifference = 0.2, lowMemory = True):
    '''Counts the number of collisions per lane in a VISSIM trajectory file

    To distinguish between cars passing and collision, 
    one checks when the sign of the position difference inverts
    (if the time are closer than collisionTimeDifference)
    If lanes is not None, only the data for the selected lanes will be provided
    (format as string x_y where x is link index and y is lane index)'''
    data = read_csv(filename, delimiter=';', comment='*', header=0, skiprows = 1, usecols = ['LANE\LINK\NO', 'LANE\INDEX', '$VEHICLE:SIMSEC', 'NO', 'POS'], low_memory = lowMemory)
    data = selectPDLanes(data, lanes)
    data = data.convert_objects(convert_numeric=True)

    merged = merge(data, data, how='inner', left_on=['LANE\LINK\NO', 'LANE\INDEX', '$VEHICLE:SIMSEC'], right_on=['LANE\LINK\NO', 'LANE\INDEX', '$VEHICLE:SIMSEC'], sort = False)
    merged = merged[merged['NO_x']>merged['NO_y']]

    nCollisions = 0
    for name, group in merged.groupby(['LANE\LINK\NO', 'LANE\INDEX', 'NO_x', 'NO_y']):
        diff = group['POS_x']-group['POS_y']
        # diff = group['POS_x']-group['POS_y'] # to check the impact of convert_objects and the possibility of using type conversion in read_csv or function to convert strings if any
        if len(diff) >= 2 and npmin(diff) < 0 and npmax(diff) > 0:
            xidx = diff[diff < 0].argmax()
            yidx = diff[diff > 0].argmin()
            if abs(group.loc[xidx, '$VEHICLE:SIMSEC'] - group.loc[yidx, '$VEHICLE:SIMSEC']) <= collisionTimeDifference:
                nCollisions += 1

    # select TD1.link_id, TD1.lane_id from temp.diff_positions as TD1, temp.diff_positions as TD2 where TD1.link_id = TD2.link_id and TD1.lane_id = TD2.lane_id and TD1.id1 = TD2.id1 and TD1.id2 = TD2.id2 and TD1.t = TD2.t+0.1 and TD1.diff*TD2.diff < 0; # besoin de faire un group by??
    # create temp table diff_positions as select CP1.t as t, CP1.link_id as link_id, CP1.lane_id as lane_id, CP1.trajectory_id as id1, CP2.trajectory_id as id2, CP1.s_coordinate - CP2.s_coordinate as diff from curvilinear_positions CP1, curvilinear_positions CP2 where CP1.link_id = CP2.link_id and CP1.lane_id = CP2.lane_id and CP1.t = CP2.t and CP1.trajectory_id > CP2.trajectory_id;
    # SQL select link_id, lane_id, id1, id2, min(diff), max(diff) from (select CP1.t as t, CP1.link_id as link_id, CP1.lane_id as lane_id, CP1.trajectory_id as id1, CP2.trajectory_id as id2, CP1.s_coordinate - CP2.s_coordinate as diff from curvilinear_positions CP1, curvilinear_positions CP2 where CP1.link_id = CP2.link_id and CP1.lane_id = CP2.lane_id and CP1.t = CP2.t and CP1.trajectory_id > CP2.trajectory_id) group by link_id, lane_id, id1, id2 having min(diff)*max(diff) < 0
    return nCollisions
    
def loadTrajectoriesFromNgsimFile(filename, nObjects = -1, sequenceNum = -1):
    '''Reads data from the trajectory data provided by NGSIM project 
    and returns the list of Feature objects'''
    objects = []

    inputfile = openCheck(filename, quitting = True)

    def createObject(numbers):
        firstFrameNum = int(numbers[1])
        # do the geometry and usertype

        firstFrameNum = int(numbers[1])
        lastFrameNum = firstFrameNum+int(numbers[2])-1
        #time = moving.TimeInterval(firstFrameNum, firstFrameNum+int(numbers[2])-1)
        obj = moving.MovingObject(num = int(numbers[0]), 
                                  timeInterval = moving.TimeInterval(firstFrameNum, lastFrameNum), 
                                  positions = moving.Trajectory([[float(numbers[6])],[float(numbers[7])]]), 
                                  userType = int(numbers[10]))
        obj.userType = int(numbers[10])
        obj.laneNums = [int(numbers[13])]
        obj.precedingVehicles = [int(numbers[14])] # lead vehicle (before)
        obj.followingVehicles = [int(numbers[15])] # following vehicle (after)
        obj.spaceHeadways = [float(numbers[16])] # feet
        obj.timeHeadways = [float(numbers[17])] # seconds
        obj.curvilinearPositions = moving.CurvilinearTrajectory([float(numbers[5])],[float(numbers[4])], obj.laneNums) # X is the longitudinal coordinate
        obj.speeds = [float(numbers[11])]
        obj.size = [float(numbers[8]), float(numbers[9])] # 8 lengh, 9 width # TODO: temporary, should use a geometry object
        return obj

    numbers = readline(inputfile).strip().split()
    if (len(numbers) > 0):
        obj = createObject(numbers)

    for line in inputfile:
        numbers = line.strip().split()
        if obj.getNum() != int(numbers[0]):
            # check and adapt the length to deal with issues in NGSIM data
            if (obj.length() != obj.positions.length()):
                print 'length pb with object %s (%d,%d)' % (obj.getNum(),obj.length(),obj.positions.length())
                obj.last = obj.getFirstInstant()+obj.positions.length()-1
                #obj.velocities = utils.computeVelocities(f.positions) # compare norm to speeds ?
            objects.append(obj)
            if (nObjects>0) and (len(objects)>=nObjects):
                break
            obj = createObject(numbers)
        else:
            obj.laneNums.append(int(numbers[13]))
            obj.positions.addPositionXY(float(numbers[6]), float(numbers[7]))
            obj.curvilinearPositions.addPositionSYL(float(numbers[5]), float(numbers[4]), obj.laneNums[-1])
            obj.speeds.append(float(numbers[11]))
            obj.precedingVehicles.append(int(numbers[14]))
            obj.followingVehicles.append(int(numbers[15]))
            obj.spaceHeadways.append(float(numbers[16]))
            obj.timeHeadways.append(float(numbers[17]))

            if (obj.size[0] != float(numbers[8])):
                print 'changed length obj %d' % (obj.getNum())
            if (obj.size[1] != float(numbers[9])):
                print 'changed width obj %d' % (obj.getNum())
    
    inputfile.close()
    return objects

def convertNgsimFile(inputfile, outputfile, append = False, nObjects = -1, sequenceNum = 0):
    '''Reads data from the trajectory data provided by NGSIM project
    and converts to our current format.'''
    if append:
        out = openCheck(outputfile,'a')
    else:
        out = openCheck(outputfile,'w')
    nObjectsPerType = [0,0,0]

    features = loadNgsimFile(inputfile, sequenceNum)
    for f in features:
        nObjectsPerType[f.userType-1] += 1
        f.write(out)

    print nObjectsPerType
        
    out.close()

def writePositionsToCsv(f, obj):
    timeInterval = obj.getTimeInterval()
    positions = obj.getPositions()
    curvilinearPositions = obj.getCurvilinearPositions()
    for i in xrange(int(obj.length())):
        p1 = positions[i]
        s = '{},{},{},{}'.format(obj.num,timeInterval[i],p1.x,p1.y)
        if curvilinearPositions is not None:
            p2 = curvilinearPositions[i]
            s += ',{},{}'.format(p2[0],p2[1])
        f.write(s+'\n')

def writeTrajectoriesToCsv(filename, objects):
    f = openCheck(filename, 'w')
    for i,obj in enumerate(objects):
        writePositionsToCsv(f, obj)
    f.close()


#########################
# Utils to read .ini type text files for configuration, meta data...
#########################

class ProcessParameters(VideoFilenameAddable):
    '''Class for all parameters controlling data processing: input,
    method parameters, etc. for tracking, classification and safety

    Note: framerate is already taken into account'''

    def loadConfigFile(self, filename):
        from ConfigParser import ConfigParser
        from os import path

        config = ConfigParser()
        config.readfp(FakeSecHead(openCheck(filename)))
        self.sectionHeader = config.sections()[0]
        # Tracking/display parameters
        self.videoFilename = config.get(self.sectionHeader, 'video-filename')
        self.databaseFilename = config.get(self.sectionHeader, 'database-filename')
        self.homographyFilename = config.get(self.sectionHeader, 'homography-filename')
        if (path.exists(self.homographyFilename)):
            self.homography = loadtxt(self.homographyFilename)
        else:
            self.homography = None
        self.intrinsicCameraFilename = config.get(self.sectionHeader, 'intrinsic-camera-filename')
        if (path.exists(self.intrinsicCameraFilename)):
            self.intrinsicCameraMatrix = loadtxt(self.intrinsicCameraFilename)
        else:
            self.intrinsicCameraMatrix = None
        distortionCoefficients = getValuesFromINIFile(filename, 'distortion-coefficients', '=')        
        self.distortionCoefficients = [float(x) for x in distortionCoefficients]
        self.undistortedImageMultiplication  = config.getfloat(self.sectionHeader, 'undistorted-size-multiplication')
        self.undistort = config.getboolean(self.sectionHeader, 'undistort')
        self.firstFrameNum = config.getint(self.sectionHeader, 'frame1')
        self.videoFrameRate = config.getfloat(self.sectionHeader, 'video-fps')

        # Classification parameters
        self.speedAggregationMethod = config.get(self.sectionHeader, 'speed-aggregation-method')
        self.nFramesIgnoreAtEnds = config.getint(self.sectionHeader, 'nframes-ignore-at-ends')
        self.speedAggregationQuantile = config.getint(self.sectionHeader, 'speed-aggregation-quantile')
        self.minSpeedEquiprobable = config.getfloat(self.sectionHeader, 'min-speed-equiprobable')
        self.minNPixels = config.getint(self.sectionHeader, 'min-npixels-crop')
        self.pedBikeCarSVMFilename = config.get(self.sectionHeader, 'pbv-svm-filename')
        self.bikeCarSVMFilename = config.get(self.sectionHeader, 'bv-svm-filename')
        self.maxPedestrianSpeed = config.getfloat(self.sectionHeader, 'max-ped-speed')
        self.maxCyclistSpeed = config.getfloat(self.sectionHeader, 'max-cyc-speed')
        self.meanPedestrianSpeed = config.getfloat(self.sectionHeader, 'mean-ped-speed')
        self.stdPedestrianSpeed = config.getfloat(self.sectionHeader, 'std-ped-speed')
        self.locationCyclistSpeed = config.getfloat(self.sectionHeader, 'cyc-speed-loc')
        self.scaleCyclistSpeed = config.getfloat(self.sectionHeader, 'cyc-speed-scale')
        self.meanVehicleSpeed = config.getfloat(self.sectionHeader, 'mean-veh-speed')
        self.stdVehicleSpeed = config.getfloat(self.sectionHeader, 'std-veh-speed')

        # Safety parameters
        self.maxPredictedSpeed = config.getfloat(self.sectionHeader, 'max-predicted-speed')/3.6/self.videoFrameRate
        self.predictionTimeHorizon = config.getfloat(self.sectionHeader, 'prediction-time-horizon')*self.videoFrameRate
        self.collisionDistance = config.getfloat(self.sectionHeader, 'collision-distance')
        self.crossingZones = config.getboolean(self.sectionHeader, 'crossing-zones')
        self.predictionMethod = config.get(self.sectionHeader, 'prediction-method')
        self.nPredictedTrajectories = config.getint(self.sectionHeader, 'npredicted-trajectories')
        self.maxNormalAcceleration = config.getfloat(self.sectionHeader, 'max-normal-acceleration')/self.videoFrameRate**2
        self.maxNormalSteering = config.getfloat(self.sectionHeader, 'max-normal-steering')/self.videoFrameRate
        self.minExtremeAcceleration = config.getfloat(self.sectionHeader, 'min-extreme-acceleration')/self.videoFrameRate**2
        self.maxExtremeAcceleration = config.getfloat(self.sectionHeader, 'max-extreme-acceleration')/self.videoFrameRate**2
        self.maxExtremeSteering = config.getfloat(self.sectionHeader, 'max-extreme-steering')/self.videoFrameRate
        self.useFeaturesForPrediction = config.getboolean(self.sectionHeader, 'use-features-prediction')

    def __init__(self, filename = None):
        if filename is not None:
            self.loadConfigFile(filename)

    def convertToFrames(self, speedRatio = 3.6):
        '''Converts parameters with a relationship to time in 'native' frame time
        speedRatio is the conversion from the speed unit in the config file
        to the distance per second

        ie param(config file) = speedRatio x fps x param(used in program)
        eg km/h = 3.6 (m/s to km/h) x frame/s x m/frame'''
        denominator = self.videoFrameRate*speedRatio
        denominator2 = denominator**2
        self.minSpeedEquiprobable = self.minSpeedEquiprobable/denominator
        self.maxPedestrianSpeed = self.maxPedestrianSpeed/denominator
        self.maxCyclistSpeed = self.maxCyclistSpeed/denominator
        self.meanPedestrianSpeed = self.meanPedestrianSpeed/denominator
        self.stdPedestrianSpeed = self.stdPedestrianSpeed/denominator
        self.meanVehicleSpeed = self.meanVehicleSpeed/denominator
        self.stdVehicleSpeed = self.stdVehicleSpeed/denominator
        # special case for the lognormal distribution
        self.locationCyclistSpeed = self.locationCyclistSpeed-log(denominator)
        #self.scaleCyclistSpeed = self.scaleCyclistSpeed

class SceneParameters(object):
    def __init__(self, config, sectionName):
        from ConfigParser import NoOptionError
        from ast import literal_eval
        try:
            self.sitename = config.get(sectionName, 'sitename')
            self.databaseFilename = config.get(sectionName, 'data-filename')
            self.homographyFilename = config.get(sectionName, 'homography-filename')
            self.calibrationFilename = config.get(sectionName, 'calibration-filename') 
            self.videoFilename = config.get(sectionName, 'video-filename')
            self.frameRate = config.getfloat(sectionName, 'framerate')
            self.date = datetime.strptime(config.get(sectionName, 'date'), datetimeFormat) # 2011-06-22 11:00:39
            self.translation = literal_eval(config.get(sectionName, 'translation')) #         = [0.0, 0.0]
            self.rotation = config.getfloat(sectionName, 'rotation')
            self.duration = config.getint(sectionName, 'duration')
        except NoOptionError as e:
            print(e)
            print('Not a section for scene meta-data')

    @staticmethod
    def loadConfigFile(filename):
        from ConfigParser import ConfigParser
        config = ConfigParser()
        config.readfp(openCheck(filename))
        configDict = dict()
        for sectionName in config.sections():
            configDict[sectionName] = SceneParameters(config, sectionName) 
        return configDict


if __name__ == "__main__":
    import doctest
    import unittest
    suite = doctest.DocFileSuite('tests/storage.txt')
    unittest.TextTestRunner().run(suite)
#     #doctest.testmod()
#     #doctest.testfile("example.txt")
