#!/bin/bash
if [ $# -ge 1 ];
then
    dbFilename=$1
else
    dbFilename="./samples/laurier.sqlite"
fi
echo "Performance report using "$dbFilename
cp $dbFilename $dbFilename.test
dbFilename=$dbFilename.test
nFeatures=`sqlite3 $dbFilename "select count(distinct trajectory_id) from positions"`

####
# test query positions/velocities in feature grouping
####
echo "####"
echo "test query positions"
echo "####"

selectTrajectories() {
    for (( i=1; i<=$nFeatures; i++ ))
    do
	sqlite3 $dbFilename "select * from positions where trajectory_id = ${i} order by frame_number" > /dev/null
    done
}

dropIndices() {
    sqlite3 $dbFilename "drop index if exists trajectory_id_index"
    sqlite3 $dbFilename "drop index if exists frame_number_index"
    sqlite3 $dbFilename "drop index if exists tid_frame_index"
    sqlite3 $dbFilename "drop index if exists of_trajectory_id_index"
}

echo "no index"
dropIndices
time selectTrajectories
####
echo "with index on trajectory_id"
sqlite3 $dbFilename "create index trajectory_id_index on positions(trajectory_id)"
time selectTrajectories
####
echo "with index on trajectory_id and frame_number"
sqlite3 $dbFilename "create index frame_number_index on positions(frame_number)"
time selectTrajectories
####
echo "with index on the 2 columns trajectory_id"
dropIndices
sqlite3 $dbFilename "create index tid_frame_index on positions(trajectory_id,frame_number)"
time selectTrajectories
####
echo "query all feature positions, order by id and frame_number (index/no index)"
time (sqlite3 $dbFilename "select * from positions order by trajectory_id, frame_number" > /dev/null)
dropIndices
time (sqlite3 $dbFilename "select * from positions order by trajectory_id, frame_number" > /dev/null)
# no effect: probably because already ordered

####
# test query positions/velocities in feature grouping
####
echo "####"
echo "test query object positions"
echo "####"

selectObjectTrajectories() {
    sqlite3 $dbFilename "SELECT OF.object_id, P.frame_number, avg(P.x_coordinate), avg(P.y_coordinate) from positions P, objects_features OF where P.trajectory_id = OF.trajectory_id group by OF.object_id, P.frame_number ORDER BY OF.object_id, P.frame_number" > /dev/null
}
echo "no index"
dropIndices
time selectObjectTrajectories
####
echo "index on trajectory_id"
sqlite3 $dbFilename "create index trajectory_id_index on positions(trajectory_id)"
time selectObjectTrajectories
####
echo "index on trajectory_id in positions and objects_features"
sqlite3 $dbFilename "create index of_trajectory_id_index on objects_features(trajectory_id)"
time selectObjectTrajectories


###
echo "Cleaning up!"
#dropIndices
rm $dbFilename
