#/python directory
This directory contains a number of supporting python scripts for working with TrafficIntelligence.

## Summaries
### objectsmoothing.py
This file contains functions for smoothing the trajectories of `MovingObject` instances (as defined in `moving.py`).
The main function in this file appears to be `smoothObject()`, which takes a `MovingObject` and returns a new
`MovingObject` with smoothed trajectories and optionally smoothed velocities. The rest of this function appears to
generally be support functions for performing aspects of smoothing, managing features, and plotting results.

### pavement.py
This file contains functions for analyzing observed road markings and classifying roads. It appears that these
functions may have been used with a dataset of observations of paint markings on a variety of roads over time
(especially the winter). Weather & road condition reports appear to be used to correlate observed paint data (which
has been collected and tabulated before using these routines) with environmental conditions. Little information seems
 to be available about the makeup of the data used in these routines. This appears to be almost entirely disconnected
from the video analysis tools of TrafficIntelligence.

### poly-utils.py
This claims to contain various utilities to load data saved by a traffic event format called POLY. The source of
these data are unclear, but the functions described within seem to deal with looking at specific interactions between
 two road users.

I'm not sure how this was meant to be used on the most basic level--it's name, with a hyphen, is not importable in
Python via the standard `import x` or `from x import y` syntaxes. This can be imported via `poly_utils = __import__
("poly-utils")` if necessary.

Many functions in here appear to be out-of-step with the current code. For example, in the function
`loadNewInteractions()`, a call to `utils.openCheck()` is made. The `utils` module does not contain an `openCheck()`
function; the `storage` module does, however. This function also creates an instance of an `Interaction` object from
`events`. This exists, but calls are made to methods of this instance which are not defined in the `Interaction`
definition nor in any of `Interaction`'s parent classes.  The methods in question are `addVideoFilename
(videoFilename)` and `addInteractionType(interactionType)`.

This file is almost entirely functions with the exception of a handful of lines which lay outside the scope of
functions. There are no function calls in these orphaned lines, nor is there any `if __name__=="__main__"` routine,
so it would seem that these lines are some combination of typos and useless.

Some comments in this code leave me to believe this was used with Nicolas' Kentucky traffic incident video dataset.

### prediction.py
This file contains classes for configuring and generating trajectory predictions. There appear to be a variety of
child classes meant to be used with different kinds of trajectories. For example, using the
`PredictedTrajectoryPrototype()` class, one can create a prototype trajectory from a moving.Trajectory() instance.
The authors note that this could come from an observed trajectory (from video) or a generic "polyline" which a
vehicle is supposed to follow (e.g., a road's centerline). The prediction using this prototype trajectory can be
simulated at a constant speed or at a speed dictated by a computed ratio (see `.predictPosition()` method for more on
 this ratio).

There are classes for generating parameters for various types of predictions, each providing a unique
`.generatePredictedTrajectories()` method.

Given two predicted trajectories, this also contains a function for estimating the time until collision.

It contains an `if __name__=="__main__"` routine which will run test(s) specified in the `tests/prediction.txt`
file.

This file may be related to work on a probabilistic framework for identifying road-user conflicts.

### processing.py
Billed as a place for "algorithms to process trajectories and moving objects", this file contains one function which
takes a list of objects, and a sequence of vertices which define a polygonal region ["zone"]. The function returns the
speeds of objects within the zone as well as objects which never in the zone.

###storage.py
This contains many utility functions for loading and saving data from across the TrafficIntelligence. This uses the
sqlite3 library to execute generated SQL queries in various databases.
For example, `loadTrajectoriesFromSqlite()` can be used to read back object positions and velocities from a
database with tables 'positions' and 'velocities', optionally also returning features.

See the appropriate part of the Functions section for all the I/O opportunities afforded by `storage.py`

## Functions
### storage.py
```
printDBError(error):
dropTables(connection, tableNames):
tableExists(filename, tableName):
createIndex(connection, tableName, columnName, unique=False):
getNumberRowsTable(connection, tableName, columnName=None):
getMinMax(connection, tableName, columnName, minmax):
writeTrajectoriesToSqlite(objects, outputFilename, trajectoryType, objectNumbers=-1):
loadPrototypeMatchIndexesFromSqlite(filename):
getObjectCriteria(objectNumbers):
loadTrajectoriesFromTable(connection, tableName, trajectoryType, objectNumbers=None):
loadUserTypesFromTable(cursor, trajectoryType, objectNumbers):
loadTrajectoriesFromSqlite(filename, trajectoryType, objectNumbers=None, withFeatures=False):
savePrototypesToSqlite(filename, prototypes, trajectoryType='feature'):
loadPrototypesFromSqlite(filename):
loadGroundTruthFromSqlite(filename, gtType='bb', gtNumbers=None):
deleteFromSqlite(filename, dataType):
createInteractionTable(cursor):
createIndicatorTables(cursor):
saveInteraction(cursor, interaction):
saveInteractions(filename, interactions):
saveIndicator(cursor, interactionNum, indicator):
saveIndicators(filename, interactions, indicatorNames=events.Interaction.indicatorNames):
loadInteractions(filename):
createBoundingBoxTable(filename, invHomography=None):
loadBoundingBoxTableForDisplay(filename):
writeFeaturesToSqlite(objects, outputFilename, trajectoryType, objectNumbers=-1):
writePrototypesToSqlite(prototypes, nMatching, outputFilename):
readPrototypesFromSqlite(filename):
writeLabelsToSqlite(labels, outputFilename):
loadLabelsFromSqlite(filename):
writeSpeedPrototypeToSqlite(prototypes, nmatching, outFilename):
loadSpeedPrototypeFromSqlite(filename):
writeRoutesToSqlite(Routes, outputFilename):
loadRoutesFromSqlite(filename):
setRoutes(filename, objects):
setRoadUserTypes(filename, objects):
openCheck(filename, option='r', quitting=False):
readline(f, commentCharacters=commentChar):
getLines(f, delimiterChar=delimiterChar, commentCharacters=commentChar):
writeList(filename, l):
loadListStrings(filename, commentCharacters=commentChar):
getValuesFromINIFile(filename, option, delimiterChar='=', commentCharacters=commentChar):
generatePDLaneColumn(data):
convertTrajectoriesVissimToSqlite(filename):
loadObjectNumbersInLinkFromVissimFile(filename, linkIds):
loadTrajectoriesFromVissimFile(filename, simulationStepsPerTimeUnit, objectNumbers=None, warmUpLastInstant=None,
selectPDLanes(data, lanes=None):
countStoppedVehiclesVissim(filename, lanes=None, proportionStationaryTime=0.7):
countCollisionsVissim(filename, lanes=None, collisionTimeDifference=0.2, lowMemory=True):
loadTrajectoriesFromNgsimFile(filename, nObjects=-1, sequenceNum=-1):
convertNgsimFile(inputfile, outputfile, append=False, nObjects=-1, sequenceNum=0):
writePositionsToCsv(f, obj):
writeTrajectoriesToCsv(filename, objects):
```
