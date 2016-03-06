# from moving import Point

'''
Sam comments:
File contains accessory functions and classes. 
Creates and connects to database.

Important classes:
    - Site:
    - EnvironementalFactors:
    - CameraView/Alignment:
'''


from datetime import datetime
from os import path

from sqlalchemy import create_engine, Column, Integer, Float, DateTime, String, ForeignKey
from sqlalchemy.orm import relationship, backref, sessionmaker
from sqlalchemy.ext.declarative import declarative_base

from utils import datetimeFormat

Base = declarative_base()

class Site(Base):
    __tablename__ = 'sites'
    idx = Column(Integer, primary_key=True)
    name = Column(String) # same as path, relative to the database position
    description = Column(String) # longer names, eg intersection of road1 and road2
    xcoordinate = Column(Float)  # ideally moving.Point, but needs to be 
    ycoordinate = Column(Float)
    
    def __init__(self, name, description = "", xcoordinate = None, ycoordinate = None):
        self.name = name
        self.description = description
        self.xcoordinate = xcoordinate
        self.ycoordinate = ycoordinate

    def getFilename(self):
        return self.name

class EnvironementalFactors(Base):
    '''Represents any environmental factors that may affect the results, in particular
    * changing weather conditions
    * changing road configuration, geometry, signalization, etc.
    ex: sunny, rainy, before counter-measure, after counter-measure'''
    __tablename__ = 'environmental_factors'
    idx = Column(Integer, primary_key=True)
    startTime = Column(DateTime)
    endTime = Column(DateTime)
    description = Column(String) # eg sunny, before, after
    siteIdx = Column(Integer, ForeignKey('sites.idx'))

    site = relationship("Site", backref=backref('environmental_factors', order_by = idx))

    def __init__(self, startTime, endTime, description, site):
        'startTime is passed as string in utils.datetimeFormat, eg 2011-06-22 10:00:39'
        self.startTime = datetime.strptime(startTime, datetimeFormat)
        self.endTime = datetime.strptime(endTime, datetimeFormat)
        self.description = description
        self.site = site

class CameraView(Base):
    __tablename__ = 'camera_views'
    idx = Column(Integer, primary_key=True)
    frameRate = Column(Float)
    homographyFilename = Column(String) # path to homograph filename, relative to the site name
    cameraCalibrationFilename = Column(String) # path to full camera calibration, relative to the site name
    siteIdx = Column(Integer, ForeignKey('sites.idx'))
    homographyDistanceUnit = Column(String, default = 'm') # make sure it is default in the database
    configurationFilename = Column(String) # path to configuration .cfg file, relative to site name

    site = relationship("Site", backref=backref('camera_views', order_by = idx))

    def __init__(self, frameRate, homographyFilename, cameraCalibrationFilename, site, configurationFilename):
        self.frameRate = frameRate
        self.homographyFilename = homographyFilename
        self.site = site
        self.configurationFilename = configurationFilename

    def getHomographyFilename(self, relativeToSiteFilename = True):
        if relativeToSiteFilename:
            return self.site.getFilename()+path.sep+self.homographyFilename
        else:
            return self.homographyFilename

class Alignment(Base):
    __tablename__ = 'alignments'
    idx = Column(Integer, primary_key=True)
    cameraViewIdx = Column(Integer, ForeignKey('camera_views.idx'))
    
    cameraView = relationship("CameraView", backref=backref('alignments', order_by = idx))

    def __init__(self, cameraView):
        self.cameraView = cameraView

class Point(Base):
    __tablename__ = 'points'
    alignmentIdx = Column(Integer, ForeignKey('alignments.idx'), primary_key=True)
    index = Column(Integer, primary_key=True) # order of points in this alignment
    x = Column(Float)
    y = Column(Float)

    alignment = relationship("Alignment", backref=backref('points', order_by = index))
    
    def __init__(self, alignmentIdx, index, x, y):
        self.alignmentIdx = alignmentIdx
        self.index = index
        self.x = x
        self.y = y

class VideoSequence(Base):
    __tablename__ = 'video_sequences'
    idx = Column(Integer, primary_key=True)
    name = Column(String) # path relative to the the site name
    startTime = Column(DateTime)
    duration = Column(Float) # video sequence duration
    durationUnit = Column(String, default = 's')
    siteIdx = Column(Integer, ForeignKey('sites.idx'))
    cameraViewIdx = Column(Integer, ForeignKey('camera_views.idx'))
    configurationFilename = Column(String)

    site = relationship("Site", backref=backref('video_sequences', order_by = idx))
    cameraView = relationship("CameraView", backref=backref('video_sequences', order_by = idx))

    def __init__(self, name, startTime, duration, site, cameraView, configurationFilename = None):
        'startTime is passed as string in utils.datetimeFormat, eg 2011-06-22 10:00:39'
        self.name = name
        self.startTime = datetime.strptime(startTime, datetimeFormat)
        self.duration = duration
        self.site = site
        self.cameraView = cameraView
        self.configurationFilename = configurationFilename

    def getVideoSequenceFilename(self, relativeToSiteFilename = True):
        if relativeToSiteFilename:
            return self.site.getFilename()+path.sep+self.name
        else:
            return self.name

        #def getConfigurationFilename(self):
        #'returns the local configuration filename, or the one of the camera view otherwise'

# add class for Analysis: foreign key VideoSequenceId, dataFilename, configFilename (get the one from camera view by default), mask? (no, can be referenced in the tracking cfg file)

# class SiteDescription(Base): # list of lines and polygons describing the site, eg for sidewalks, center lines

# class Analysis(Base): # parameters necessary for processing the data: free form
# eg bounding box depends on camera view, tracking configuration depends on camera view 
# results: sqlite

def createDatabase(filename):
    'creates a session to query the filename'
    engine = create_engine('sqlite:///'+filename)
    Base.metadata.create_all(engine)
    Session = sessionmaker(bind=engine)
    return Session()

def connectDatabase(filename):
    'creates a session to query the filename'
    engine = create_engine('sqlite:///'+filename)
    Session = sessionmaker(bind=engine)
    return Session()

def initializeSites(session, directoryName):
    '''Initializes default site objects and Camera Views
    
    eg somedirectory/montreal/ contains intersection1, intersection2, etc.
    The site names would be somedirectory/montreal/intersection1, somedirectory/montreal/intersection2, etc.'''
    from os import listdir, path
    sites = []
    cameraViews = []
    names = listdir(directoryName)
    for name in names:
        if path.isdir(directoryName+'/'+name):
            sites.append(Site(directoryName+'/'+name, None))
            cameraViews.append(CameraView(-1, None, None, sites[-1], None))
    session.add_all(sites)
    session.add_all(cameraViews)
    session.commit()
# TODO crawler for video files?
