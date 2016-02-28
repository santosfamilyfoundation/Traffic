'''Module for few base classes to avoid issues of circular import'''

'''
Sam comments:
This object is found in:
     - events.py
     - moving.py
     - storage.py
     
It seems to be a class that is extended upon to allow the ability to add a video filename. 

'''

class VideoFilenameAddable(object):
    'Base class with the capability to attach a video filename'

    def setVideoFilename(self, videoFilename):
        self.videoFilename = videoFilename
