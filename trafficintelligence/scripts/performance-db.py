#! /usr/bin/env python

import sys, shutil, os, sqlite3, timeit#, argparse

import storage

if len(sys.argv) >= 2:
    dbFilename = sys.argv[1]
else:
    dbFilename="./samples/laurier.sqlite"

print("Performance report using {}".format(dbFilename))
shutil.copy(dbFilename, dbFilename+'.test')
dbFilename=dbFilename+'.test'
connection = sqlite3.connect(dbFilename)

nFeatures=storage.getNumberRowsTable(connection, "positions", "trajectory_id")
print dbFilename, nFeatures

####
# test query tmp trajectory instant table
####
def selectTrajectoryIdInstant(connection, lastInstant):
    cursor = connection.cursor()
    for i in xrange(lastInstant):
	cursor.execute("select trajectory_id from trajectory_instants where last_instant = {}".format(lastInstant))
        cursor.fetchall()

def test2Setup(connection):
    cursor = connection.cursor()
    cursor.execute("CREATE TEMP TABLE IF NOT EXISTS trajectory_instants AS select trajectory_id, min(frame_number) as first_instant, max(frame_number) as last_instant, max(frame_number)-min(frame_number)+1 as length from positions group by trajectory_id")
    return storage.getMinMax(connection, "trajectory_instants", "last_instant", "max")

# lastInstant = test2Setup(connection)
# print timeit.timeit("selectTrajectoryIdInstant(connection, lastInstant)", setup="from __main__ import selectTrajectoryIdInstant, connection, lastInstant", number = 100)
# storage.createIndex(connection, "trajectory_instants", "last_instant")
# print timeit.timeit("selectTrajectoryIdInstant(connection, lastInstant)", setup="from __main__ import selectTrajectoryIdInstant, connection, lastInstant", number = 100)
# storage.createIndex(connection, "trajectory_instants", "trajectory_id", True)
# print timeit.timeit("selectTrajectoryIdInstant(connection, lastInstant)", setup="from __main__ import selectTrajectoryIdInstant, connection, lastInstant", number = 100)


####
# test query positions/velocities in feature grouping
####
def selectTrajectories(connection, nFeatures):
    cursor = connection.cursor()
    for i in xrange(nFeatures):
	cursor.execute("select * from positions where trajectory_id = {} order by frame_number".format(i))
        cursor.fetchall()

def dropIndices(connection):
    cursor = connection.cursor()
    cursor.execute("drop index if exists positions_trajectory_id_index")
    #sqlite3 $dbFilename "drop index if exists frame_number_index"
    #sqlite3 $dbFilename "drop index if exists tid_frame_index"
    #sqlite3 $dbFilename "drop index if exists of_trajectory_id_index"
    connection.commit()

def test1Positions():
    print('''####
    test query positions
    ####''')
    print("no index")
    dropIndices(connection)
    print timeit.timeit("selectTrajectories(connection, nFeatures)", setup="from __main__ import selectTrajectories, connection, nFeatures", number = 100)
    ####
    print("with index on trajectory_id")
    storage.createIndex(connection, "positions", "trajectory_id")#sqlite3 $dbFilename "create index trajectory_id_index on positions(trajectory_id)"
    print timeit.timeit("selectTrajectories(connection, nFeatures)", setup="from __main__ import selectTrajectories, connection, nFeatures", number = 100)

#### Cleanup
os.remove(dbFilename)
