#! /usr/bin/env python

from pdtv import TsaiCamera, ZipVideo, SyncedVideos, TrackSet, Track, State
import sys, os, datetime, argparse
import shutil
import sqlite3
import zipfile
import utils
import cvutils
import cv2


def zipFolder(inputFolder, outputFile):
    '''Method to compress the content of the inputFolder in the outputFile'''
    zip = zipfile.ZipFile(outputFile, 'w')
    for root, dirs, files in os.walk(inputFolder):
        for file in files:
            zip.write(root+file, file)
    zip.close()




def getTypeDict(cursor):
    '''Return a dictionnary with integer key and associated type string
    i.e.: "0"  -> "unknown"
          "1"  -> "car"
          "2"  -> "pedestrians"
          "3"  -> "motorcycle"
          "4"  -> "bicycle"
          "5"  -> "bus"
          "6"  -> "truck"
          ... and other type if the objects_type table is defined in SQLite'''
    typeDict = dict()
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='objects_type'")
    data = cursor.fetchone()

    if(data is None):
        typeDict["0"] = "unknown"
        typeDict["1"] = "car"
        typeDict["2"] = "pedestrians"
        typeDict["3"] = "motorcycle"
        typeDict["4"] = "bicycle"
        typeDict["5"] = "bus"
        typeDict["6"] = "truck"
        
    else:
        cursor.execute("SELECT road_user_type, type_string FROM objects_type")
        for row in cursor:
            typeDict[row[0]]= row[1]

    return typeDict

def extractFrames(videoFile, framePath, fps, time, firstFrameNum = 0, lastFrameNum = None):
    '''Method to extract all the frames of the video'''
    print('Extracting frame')
    deltaTimestamp = 1000.0/float(fps);
    time+=datetime.timedelta(microseconds=firstFrameNum*deltaTimestamp*1000)
    
    inc = 1000 #How many frame we fetch in the video at a time

    if lastFrameNum is not None:
        delta = lastFrameNum-firstFrameNum
        if delta < inc:
            inc = delta
    
    currentIdx = firstFrameNum
    frameList = cvutils.getImagesFromVideo(videoFile, firstFrameNum = currentIdx, nFrames = inc)
    
    while len(frameList) == inc and inc > 0:
        
        for f in frameList:
            cv2.imwrite(os.path.join(framePath,time.strftime("%Y%m%d-%H%M%S.%f")[:-3]+'.jpg'), f)
            time += datetime.timedelta(microseconds=deltaTimestamp*1000)
        currentIdx = currentIdx + inc

        if lastFrameNum is not None:            
            delta = lastFrameNum-currentIdx
            if delta < inc:
                inc = delta        
        if inc:
            frameList = cvutils.getImagesFromVideo(videoFile, firstFrameNum = currentIdx, nFrames = inc)
        print('Extracting frame ' + str(currentIdx))
    return len(frameList) > 0

    

def convertDatabase(workDirname, sectionName, sceneFilename = None, databaseFilename = None, videoFilename = None, videoFolderExist = False, firstFrameNum = 0, lastFrameNum = None, cameraCalibrationFilename = None, outputFileName = 'roaduser.json'):
    '''
    Method to convert database from polytrack to PDTV:
    workDirname  is the current working directory
    sceneFilename is the path to the .cfg file
    sectionName is the name of the section we want to process in this file
    videoFolderExist specifiy if we want to reextract the video frame or if they already exist at workdir/videoframes/
    firstFrameNum is the first frame we want to extract
    lastFrameNum is the last frame we want to extract (or None if we want to extract everything)
    '''
    error = False
    if sceneFilename is not None:
        scene = utils.SceneParameters.loadConfigFile(os.path.join(workDirname, sceneFilename))
        time = scene[sectionName].date
        inputDb = os.path.join(workDirname, scene[sectionName].databaseFilename)
        videoFile = os.path.join(workDirname, scene[sectionName].videoFilename)

    if databaseFilename is not None:
        inputDb = os.path.join(workDirname, databaseFilename)
    if videoFilename is not None:
        videoFile = os.path.join(workDirname, videoFilename)
    # elif videoFolderExist == False:            
    #     print('No video path specified')
    #     error = True
    
    videoFolderPath = os.path.join(workDirname, "videoframes/")
    fileName = sectionName
    
    if videoFile is not None:
        fps = cvutils.getFPS(videoFile)
        print('Video should run at ' + str(fps) + ' fps')
        deltaTimestamp = 1000.0/float(fps);
        if videoFolderExist == False:
            if os.path.exists(videoFolderPath):
                shutil.rmtree(videoFolderPath)
            utils.mkdir(videoFolderPath)
            if extractFrames(videoFile, videoFolderPath, fps, time, firstFrameNum, lastFrameNum) == 0:
                print("Error. Frame were not extracted")
                error = True

    
    if not error:
        masterTimestamp = 0.0
        masterTimestampList = list()
        video_timestringsList = list()
        frameNumberToMasterTimestamp = dict()
        for r,d,f in os.walk(videoFolderPath):
            i = firstFrameNum
            for files in f:
                name, ext = os.path.splitext(files)           
                video_timestringsList.append(name)
                masterTimestampList.append(masterTimestamp)            
                frameNumberToMasterTimestamp[i] = masterTimestamp
                i = i +1
                masterTimestamp = masterTimestamp+deltaTimestamp
        
        inputZipVideoName = fileName+".zip"
        print('Zipping files...')
        if not os.path.exists(inputZipVideoName) or not videoFolderExist:
            zipFolder(videoFolderPath, inputZipVideoName)
        print('Zipping files...Done.')
        #We generate the structure for ZipVideo
        if cameraCalibrationFilename is not None:
            calibrationFile = cameraCalibrationFilename
        else:
            calibrationFile = 'calib.json'
        zipVideo = ZipVideo(video_zip_file=inputZipVideoName,
                        time_offset=0.0, time_scale=1.0, master_timestamps=masterTimestampList, calibration_file=calibrationFile)
        zipVideo.save(os.path.join(workDirname, 'video.json'))

        print('ZipVideo saved')
        obj = SyncedVideos(master_timestamps = [masterTimestamp],
                               video_timestrings = [video_timestringsList],
                               video_files = ['video.json'],
                               fps=fps)
        obj.save(os.path.join(workDirname, 'syncedvideo.json'))

        print('SyncedVideos saved')

        print('Opening db')
        connection = sqlite3.connect(inputDb)
        cursor = connection.cursor()
      

        #Tracket database
        trackset = TrackSet(synced_file = ['syncedvideo.json'])

        
        #1) We build the type index dictionnary
        typeDict = getTypeDict(cursor)
        
        idToTrackDict = dict()
        #2) We read the object database    
        cursor.execute("SELECT object_id, road_user_type FROM objects")
        for row in cursor:
            objectId = row[0]
            objectType = row[1]
            t = Track(type=typeDict.get(objectType, "unknown"))
            idToTrackDict[objectId] = t;
            trackset.append(t)
        print('Reading boundingbox table')
        #3) We read the bounding box table    
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='bounding_boxes'")
        data = cursor.fetchone()
        if data is None:
            print('No bounding box table. Maybe it was not generated ?')
        else:
            cursor.execute("SELECT object_id, frame_number, x_top_left, y_top_left, x_bottom_right, y_bottom_right FROM bounding_boxes")
            for row in cursor:
                objectId = row[0]
                frameNumber = row[1]
                x_top_left = row[2]
                y_top_left = row[3]
                x_bottom_right = row[4]
                y_bottom_right = row[5]
                
                idToTrackDict[objectId].append(State(frame=int(frameNumber), world_position = [0.,0.], 
                                                 master_timestamp=frameNumberToMasterTimestamp[int(frameNumber)],
                                                 bounding_boxes=[[(x_top_left, x_bottom_right), (y_top_left, y_bottom_right)]]))
        print('Saving db in json')
        trackset.save(os.path.join(workDirname, outputFileName))
        connection.close()
        print('Done.')

    
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='The program convert polytrack.sqlite database to pdtv bounding boxes', epilog = 'Either the configuration filename or the other parameters (at least video and database filenames) need to be provided.')
    parser.add_argument('-w', dest = 'workDirname', help = 'use a different work directory', default = "./",type = str)
    parser.add_argument('--scene', dest = 'sceneFilename', help = 'name of the configuration file', type = str, default = None)
    parser.add_argument('--section', dest = 'sectionName', help = 'name of the section', type = str, default = None)
    parser.add_argument('-d', dest = 'databaseFilename', help = 'name of the Sqlite database file', type = str, default = None)
    parser.add_argument('-i', dest = 'videoFilename', help = 'name of the video file', type = str, default = None) 
    parser.add_argument('-c', dest = 'cameraCalibrationFilename', help = 'name of the camera json file', type = str, default = None)
    parser.add_argument('-o', dest = 'outputFilename', help = 'name of the output json file', type = str, default = 'roaduser.json')
    parser.add_argument('-s', dest = 'firstFrameNum', help = 'forced start frame', type = int, default = 0)
    parser.add_argument('-e', dest = 'lastFrameNum', help = 'forced end frame', type = int, default = None)
    #parser.add_argument('-t', dest = 'useDatabaseTimestamp', help = 'use the timestamp of the database', default= False, type = bool) 
    parser.add_argument('-u', dest = 'useCurrentVideoFile', help = 'use the previously generated video file', action = 'store_true')
    args = parser.parse_args()
    #convertDatabase(args)

    convertDatabase(args.workDirname, args.sectionName, args.sceneFilename, videoFilename = args.videoFilename, databaseFilename = args.databaseFilename, videoFolderExist = args.useCurrentVideoFile, firstFrameNum = args.firstFrameNum, lastFrameNum = args.lastFrameNum, cameraCalibrationFilename = args.cameraCalibrationFilename, outputFileName=args.outputFilename )
