'''Module for few base classes to avoid issues of circular import'''

class VideoFilenameAddable(object):
    'Base class with the capability to attach a video filename'

    def setVideoFilename(self, videoFilename):
        self.videoFilename = videoFilename
