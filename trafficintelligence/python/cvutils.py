#! /usr/bin/env python
'''Image/Video utilities'''

import utils

try:
    import cv2
    opencvAvailable = True
except ImportError:
    print('OpenCV library could not be loaded (video replay functions will not be available)') # TODO change to logging module
    opencvAvailable = False
try:
    import skimage
    skimageAvailable = True
except ImportError:
    print('Scikit-image library could not be loaded (HoG-based classification methods will not be available)')
    skimageAvailable = False
    
from sys import stdout
from numpy import dot, array, append, float32

#import aggdraw # agg on top of PIL (antialiased drawing)


cvRed = (0,0,255)
cvGreen = (0,255,0)
cvBlue = (255,0,0)
cvCyan = (255, 255, 0)
cvYellow = (0, 255, 255)
cvMagenta = (255, 0, 255)
cvWhite = (255, 255, 255)
cvBlack = (0,0,0)
cvColors3 = utils.PlottingPropertyValues([cvRed,
                                          cvGreen,
                                          cvBlue])
cvColors = utils.PlottingPropertyValues([cvRed,
                                         cvGreen,
                                         cvBlue,
                                         cvCyan,
                                         cvYellow,
                                         cvMagenta,
                                         cvWhite,
                                         cvBlack])

def quitKey(key):
    return chr(key&255)== 'q' or chr(key&255) == 'Q'

def saveKey(key):
    return chr(key&255) == 's'

def int2FOURCC(x):
    fourcc = ''
    for i in xrange(4):
        fourcc += unichr((x >> 8*i)&255)
    return fourcc

def plotLines(filename, origins, destinations, w = 1, resultFilename='image.png'):
    '''Draws lines over the image '''
    import Image, ImageDraw # PIL
    
    img = Image.open(filename)

    draw = ImageDraw.Draw(img)
    #draw = aggdraw.Draw(img)
    #pen = aggdraw.Pen("red", width)
    for p1, p2 in zip(origins, destinations):
        draw.line([p1.x, p1.y, p2.x, p2.y], width = w, fill = (256,0,0))
        #draw.line([p1.x, p1.y, p2.x, p2.y], pen)
    del draw

    #out = utils.openCheck(resultFilename)
    img.save(resultFilename)

def rgb2gray(rgb):
    return dot(rgb[...,:3], [0.299, 0.587, 0.144])

def matlab2PointCorrespondences(filename):
    '''Loads and converts the point correspondences saved 
    by the matlab camera calibration tool'''
    from numpy.lib.io import loadtxt, savetxt
    from numpy.lib.function_base import append
    points = loadtxt(filename, delimiter=',')
    savetxt(utils.removeExtension(filename)+'-point-correspondences.txt',append(points[:,:2].T, points[:,3:].T, axis=0))

def loadPointCorrespondences(filename):
    '''Loads and returns the corresponding points in world (first 2 lines) and image spaces (last 2 lines)'''
    from numpy import loadtxt, float32
    points = loadtxt(filename, dtype=float32)
    return  (points[:2,:].T, points[2:,:].T) # (world points, image points)

def cvMatToArray(cvmat):
    '''Converts an OpenCV CvMat to numpy array.'''
    print('Deprecated, use new interface')
    from numpy import zeros
    a = zeros((cvmat.rows, cvmat.cols))#array([[0.0]*cvmat.width]*cvmat.height)
    for i in xrange(cvmat.rows):
        for j in xrange(cvmat.cols):
            a[i,j] = cvmat[i,j]
    return a

if opencvAvailable:
    def computeHomography(srcPoints, dstPoints, method=0, ransacReprojThreshold=3.0):
        '''Returns the homography matrix mapping from srcPoints to dstPoints (dimension Nx2)'''
        H, mask = cv2.findHomography(srcPoints, dstPoints, method, ransacReprojThreshold)
        return H

    def arrayToCvMat(a, t = cv2.CV_64FC1):
        '''Converts a numpy array to an OpenCV CvMat, with default type CV_64FC1.'''
        print('Deprecated, use new interface')
        cvmat = cv2.cv.CreateMat(a.shape[0], a.shape[1], t)
        for i in range(cvmat.rows):
            for j in range(cvmat.cols):
                cvmat[i,j] = a[i,j]
        return cvmat

    def cvPlot(img, positions, color, lastCoordinate = None, **kwargs):
        if lastCoordinate is None:
            last = positions.length()-1
        elif lastCoordinate >=0:
            last = min(positions.length()-1, lastCoordinate)
        for i in range(0, last):
            cv2.line(img, positions[i].asint().astuple(), positions[i+1].asint().astuple(), color, **kwargs)

    def cvImshow(windowName, img, rescale = 1.0):
        'Rescales the image (in particular if too large)'
        from cv2 import resize
        if rescale != 1.:
            size = (int(round(img.shape[1]*rescale)), int(round(img.shape[0]*rescale)))
            resizedImg = resize(img, size)
            cv2.imshow(windowName, resizedImg)
        else:
            cv2.imshow(windowName, img)

    def computeUndistortMaps(width, height, undistortedImageMultiplication, intrinsicCameraMatrix, distortionCoefficients):
        from copy import deepcopy
        from numpy import identity
        newImgSize = (int(round(width*undistortedImageMultiplication)), int(round(height*undistortedImageMultiplication)))
        newCameraMatrix = deepcopy(intrinsicCameraMatrix)
        newCameraMatrix[0,2] = newImgSize[0]/2.
        newCameraMatrix[1,2] = newImgSize[1]/2.
        return cv2.initUndistortRectifyMap(intrinsicCameraMatrix, array(distortionCoefficients), identity(3), newCameraMatrix, newImgSize, cv2.CV_32FC1)

    def playVideo(filename, firstFrameNum = 0, frameRate = -1, interactive = False, printFrames = True, text = None, rescale = 1., step = 1):
        '''Plays the video'''
        windowName = 'frame'
        if rescale == 1.:
            cv2.namedWindow(windowName, cv2.WINDOW_NORMAL)
        wait = 5
        if frameRate > 0:
            wait = int(round(1000./frameRate))
        if interactive:
            wait = 0
        capture = cv2.VideoCapture(filename)
        if capture.isOpened():
            key = -1
            ret = True
            frameNum = firstFrameNum
            capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, firstFrameNum)
            while ret and not quitKey(key):
                #ret, img = capture.read()
                for i in xrange(step):
                    ret, img = capture.read()
                if ret:
                    if printFrames:
                        print('frame {0}'.format(frameNum))
                    frameNum+=step
                    if text is not None:
                       cv2.putText(img, text, (10,50), cv2.cv.CV_FONT_HERSHEY_PLAIN, 1, cvRed) 
                    cvImshow(windowName, img, rescale)
                    key = cv2.waitKey(wait)
                    if saveKey(key):
                        cv2.imwrite('image-{}.png'.format(frameNum), img)
            cv2.destroyAllWindows()
        else:
            print('Video capture for {} failed'.format(filename))

    def infoVideo(filename):
        '''Provides all available info on video '''
        cvPropertyNames = {cv2.cv.CV_CAP_PROP_FORMAT: "format",
                           cv2.cv.CV_CAP_PROP_FOURCC: "codec (fourcc)",
                           cv2.cv.CV_CAP_PROP_FPS: "fps",
                           cv2.cv.CV_CAP_PROP_FRAME_COUNT: "number of frames",
                           cv2.cv.CV_CAP_PROP_FRAME_HEIGHT: "heigh",
                           cv2.cv.CV_CAP_PROP_FRAME_WIDTH: "width",
                           cv2.cv.CV_CAP_PROP_RECTIFICATION: "rectification",
                           cv2.cv.CV_CAP_PROP_SATURATION: "saturation"}
        capture = cv2.VideoCapture(filename)
        if capture.isOpened():
            for cvprop in [#cv2.cv.CV_CAP_PROP_BRIGHTNESS
                    #cv2.cv.CV_CAP_PROP_CONTRAST
                    #cv2.cv.CV_CAP_PROP_CONVERT_RGB
                    #cv2.cv.CV_CAP_PROP_EXPOSURE
                    cv2.cv.CV_CAP_PROP_FORMAT,
                    cv2.cv.CV_CAP_PROP_FOURCC,
                    cv2.cv.CV_CAP_PROP_FPS,
                    cv2.cv.CV_CAP_PROP_FRAME_COUNT,
                    cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,
                    cv2.cv.CV_CAP_PROP_FRAME_WIDTH,
                    #cv2.cv.CV_CAP_PROP_GAIN,
                    #cv2.cv.CV_CAP_PROP_HUE
                    #cv2.cv.CV_CAP_PROP_MODE
                    #cv2.cv.CV_CAP_PROP_POS_AVI_RATIO
                    #cv2.cv.CV_CAP_PROP_POS_FRAMES
                    #cv2.cv.CV_CAP_PROP_POS_MSEC
                    #cv2.cv.CV_CAP_PROP_RECTIFICATION,
                    #cv2.cv.CV_CAP_PROP_SATURATION
            ]:
                prop = capture.get(cvprop)
                if cvprop == cv2.cv.CV_CAP_PROP_FOURCC and prop > 0:
                    prop = int2FOURCC(int(prop))
                print('Video {}: {}'.format(cvPropertyNames[cvprop], prop))
        else:
            print('Video capture for {} failed'.format(filename))

    def getImagesFromVideo(videoFilename, firstFrameNum = 0, nFrames = 1, saveImage = False, outputPrefix = 'image'):
        '''Returns nFrames images from the video sequence'''
        from math import floor, log10
        images = []
        capture = cv2.VideoCapture(videoFilename)
        if capture.isOpened():
            rawCount = capture.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT)
            if rawCount < 0:
                rawCount = firstFrameNum+nFrames+1
            nDigits = int(floor(log10(rawCount)))+1
            ret = False
            capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, firstFrameNum)
            imgNum = 0
            while imgNum<nFrames:
                ret, img = capture.read()
                i = 0
                while not ret and i<10:
                    ret, img = capture.read()
                    i += 1
                if img.size>0:
                    if saveImage:
                        imgNumStr = format(firstFrameNum+imgNum, '0{}d'.format(nDigits))
                        cv2.imwrite(outputPrefix+imgNumStr+'.png', img)
                    else:
                        images.append(img)
                    imgNum +=1
            capture.release()
        else:
            print('Video capture for {} failed'.format(videoFilename))
        return images
    
    def getFPS(videoFilename):
        capture = cv2.VideoCapture(videoFilename)
        if capture.isOpened():
            fps = capture.get(cv2.cv.CV_CAP_PROP_FPS)
            capture.release()
            return fps
        else:
            print('Video capture for {} failed'.format(videoFilename))
            return None

    def imageBox(img, obj, frameNum, homography, width, height, px = 0.2, py = 0.2, minNPixels = 800):
        'Computes the bounding box of object at frameNum'
        x = []
        y = []
        if obj.hasFeatures():
            for f in obj.getFeatures():
                if f.existsAtInstant(frameNum):
                    projectedPosition = f.getPositionAtInstant(frameNum).project(homography)
                    x.append(projectedPosition.x)
                    y.append(projectedPosition.y)
        xmin = min(x)
        xmax = max(x)
        ymin = min(y)
        ymax = max(y)
        xMm = px * (xmax - xmin)
        yMm = py * (ymax - ymin)
        a = max(ymax - ymin + (2 * yMm), xmax - (xmin + 2 * xMm))
        yCropMin = int(max(0, .5 * (ymin + ymax - a)))
        yCropMax = int(min(height - 1, .5 * (ymin + ymax + a)))
        xCropMin = int(max(0, .5 * (xmin + xmax - a)))
        xCropMax = int(min(width - 1, .5 * (xmin + xmax + a)))
        if yCropMax != yCropMin and xCropMax != xCropMin and (yCropMax - yCropMin) * (xCropMax - xCropMin) > minNPixels:
            croppedImg = img[yCropMin : yCropMax, xCropMin : xCropMax]
        else:
            croppedImg = None
        return croppedImg, yCropMin, yCropMax, xCropMin, xCropMax


    def displayTrajectories(videoFilename, objects, boundingBoxes = {}, homography = None, firstFrameNum = 0, lastFrameNumArg = None, printFrames = True, rescale = 1., nFramesStep = 1, saveAllImages = False, undistort = False, intrinsicCameraMatrix = None, distortionCoefficients = None, undistortedImageMultiplication = 1., annotations = [], gtMatches = {}, toMatches = {}):
        '''Displays the objects overlaid frame by frame over the video '''
        from moving import userTypeNames
        from math import ceil, log10

        capture = cv2.VideoCapture(videoFilename)
        width = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
        height = int(capture.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))

        windowName = 'frame'
        if rescale == 1.:
            cv2.namedWindow(windowName, cv2.WINDOW_NORMAL)

        if undistort: # setup undistortion
            [map1, map2] = computeUndistortMaps(width, height, undistortedImageMultiplication, intrinsicCameraMatrix, distortionCoefficients)
        if capture.isOpened():
            key = -1
            ret = True
            frameNum = firstFrameNum
            capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, firstFrameNum)
            if lastFrameNumArg is None:
                from sys import maxint
                lastFrameNum = maxint
            else:
                lastFrameNum = lastFrameNumArg
            nZerosFilename = int(ceil(log10(lastFrameNum)))
            objectToDeleteIds = []
            while ret and not quitKey(key) and frameNum <= lastFrameNum:
                ret, img = capture.read()
                if ret:
                    if undistort:
                        img = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR)
                    if printFrames:
                        print('frame {0}'.format(frameNum))
                    if len(objectToDeleteIds) > 0:
                        objects = [o for o in objects if o.getNum() not in objectToDeleteIds]
                        objectToDeleteIds = []
                    # plot objects
                    for obj in objects:
                        if obj.existsAtInstant(frameNum):
                            if obj.getLastInstant() == frameNum:
                                objectToDeleteIds.append(obj.getNum())
                            if not hasattr(obj, 'projectedPositions'):
                                if homography is not None:
                                    obj.projectedPositions = obj.positions.project(homography)
                                else:
                                    obj.projectedPositions = obj.positions
                            cvPlot(img, obj.projectedPositions, cvColors[obj.getNum()], frameNum-obj.getFirstInstant())
                            if frameNum not in boundingBoxes.keys() and obj.hasFeatures():
                                imgcrop, yCropMin, yCropMax, xCropMin, xCropMax = imageBox(img, obj, frameNum, homography, width, height)
                                cv2.rectangle(img, (xCropMin, yCropMin), (xCropMax, yCropMax), cvBlue, 1)
                            objDescription = '{} '.format(obj.num)
                            if userTypeNames[obj.userType] != 'unknown':
                                objDescription += userTypeNames[obj.userType][0].upper()
                            if len(annotations) > 0: # if we loaded annotations, but there is no match
                                if frameNum not in toMatches[obj.getNum()]:
                                    objDescription += " FA"
                            cv2.putText(img, objDescription, obj.projectedPositions[frameNum-obj.getFirstInstant()].asint().astuple(), cv2.cv.CV_FONT_HERSHEY_PLAIN, 1, cvColors[obj.getNum()])
                    # plot object bounding boxes
                    if frameNum in boundingBoxes.keys():
                        for rect in boundingBoxes[frameNum]:
                            cv2.rectangle(img, rect[0].asint().astuple(), rect[1].asint().astuple(), cvColors[obj.getNum()])
                    # plot ground truth
                    if len(annotations) > 0:
                        for gt in annotations:
                            if gt.existsAtInstant(frameNum):
                                if frameNum in gtMatches[gt.getNum()]:
                                    color = cvColors[gtMatches[gt.getNum()][frameNum]] # same color as object
                                else:
                                    color = cvRed
                                    cv2.putText(img, 'Miss', gt.topLeftPositions[frameNum-gt.getFirstInstant()].asint().astuple(), cv2.cv.CV_FONT_HERSHEY_PLAIN, 1, cvRed)
                                cv2.rectangle(img, gt.topLeftPositions[frameNum-gt.getFirstInstant()].asint().astuple(), gt.bottomRightPositions[frameNum-gt.getFirstInstant()].asint().astuple(), color)
                    # saving images and going to next
                    if not saveAllImages:
                        cvImshow(windowName, img, rescale)
                        key = cv2.waitKey()
                    if saveAllImages or saveKey(key):
                        cv2.imwrite('image-{{:0{}}}.png'.format(nZerosFilename).format(frameNum), img)
                    frameNum += nFramesStep
                    if nFramesStep > 1:
                        capture.set(cv2.cv.CV_CAP_PROP_POS_FRAMES, frameNum)
            cv2.destroyAllWindows()
        else:
            print 'Cannot load file ' + videoFilename

    def computeHomographyFromPDTV(camera):
        '''Returns the homography matrix at ground level from PDTV camera
        https://bitbucket.org/hakanardo/pdtv'''
        # camera = pdtv.load(cameraFilename)
        srcPoints = [[x,y] for x, y in zip([1.,2.,2.,1.],[1.,1.,2.,2.])] # need floats!!
        dstPoints = []
        for srcPoint in srcPoints:
            projected = camera.image_to_world(tuple(srcPoint))
            dstPoints.append([projected[0], projected[1]])
        H, mask = cv2.findHomography(array(srcPoints), array(dstPoints), method = 0) # No need for different methods for finding homography
        return H

    def undistortedCoordinates(map1, map2, x, y, maxDistance = 1.):
        '''Returns the coordinates of a point in undistorted image
        map1 and map2 are the mapping functions from undistorted image
        to distorted (original image)
        map1(x,y) = originalx, originaly'''
        from numpy import abs, logical_and, unravel_index, sum
        from matplotlib.mlab import find
        distx = abs(map1-x)
        disty = abs(map2-y)
        indices = logical_and(distx<maxDistance, disty<maxDistance)
        closeCoordinates = unravel_index(find(indices), distx.shape) # returns i,j, ie y,x
        xWeights = 1-distx[indices]
        yWeights = 1-disty[indices]
        return dot(xWeights, closeCoordinates[1])/sum(xWeights), dot(yWeights, closeCoordinates[0])/sum(yWeights)

    def undistortTrajectoryFromCVMapping(map1, map2, t):
        '''test 'perfect' inversion'''
        from moving import Trajectory
        from numpy import isnan
        undistortedTrajectory = Trajectory()
        for i,p in enumerate(t):
            res = undistortedCoordinates(map1, map2, p.x,p.y)
            if not isnan(res).any():
                undistortedTrajectory.addPositionXY(res[0], res[1])
            else:
                print i,p,res
        return undistortedTrajectory

    def computeInverseMapping(originalImageSize, map1, map2):
        'Computes inverse mapping from maps provided by cv2.initUndistortRectifyMap'
        from numpy import ones, isnan
        invMap1 = -ones(originalImageSize)
        invMap2 = -ones(originalImageSize)
        for x in range(0,originalImageSize[1]):
            for y in range(0,originalImageSize[0]):
                res = undistortedCoordinates(x,y, map1, map2)
                if not isnan(res).any():
                    invMap1[y,x] = res[0]
                    invMap2[y,x] = res[1]
        return invMap1, invMap2

    def cameraIntrinsicCalibration(path, checkerBoardSize=[6,7], secondPassSearch=False, display=False):
        ''' Camera calibration searches through all the images (jpg or png) located
            in _path_ for matches to a checkerboard pattern of size checkboardSize.
            These images should all be of the same camera with the same resolution.

            For best results, use an asymetric board and ensure that the image has
            very high contrast, including the background. Suitable checkerboard:
            http://ftp.isr.ist.utl.pt/pub/roswiki/attachments/camera_calibration(2f)Tutorials(2f)StereoCalibration/check-108.png

            The code below is based off of:
            https://opencv-python-tutroals.readthedocs.org/en/latest/py_tutorials/py_calib3d/py_calibration/py_calibration.html
            Modified by Paul St-Aubin
            '''
        from numpy import zeros, mgrid, float32, savetxt
        import glob, os

        # termination criteria
        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

        # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
        objp = zeros((checkerBoardSize[0]*checkerBoardSize[1],3), float32)
        objp[:,:2] = mgrid[0:checkerBoardSize[1],0:checkerBoardSize[0]].T.reshape(-1,2)

        # Arrays to store object points and image points from all the images.
        objpoints = [] # 3d point in real world space
        imgpoints = [] # 2d points in image plane.

        ## Loop throuhg all images in _path_
        images = glob.glob(os.path.join(path,'*.[jJ][pP][gG]'))+glob.glob(os.path.join(path,'*.[jJ][pP][eE][gG]'))+glob.glob(os.path.join(path,'*.[pP][nN][gG]'))
        for fname in images:
            img = cv2.imread(fname)
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # Find the chess board corners
            ret, corners = cv2.findChessboardCorners(gray, (checkerBoardSize[1],checkerBoardSize[0]), None)

            # If found, add object points, image points (after refining them)
            if ret:
                print 'Found pattern in '+fname
                
                if(secondPassSearch): 
                    corners = cv2.cornerSubPix(gray, corners, (11,11), (-1,-1), criteria)

                objpoints.append(objp)
                imgpoints.append(corners)

                # Draw and display the corners
                if(display):
                    img = cv2.drawChessboardCorners(img, (checkerBoardSize[1],checkerBoardSize[0]), corners, ret)
                    if(img):
                        cv2.imshow('img',img)
                        cv2.waitKey(0)

        ## Close up image loading and calibrate
        cv2.destroyAllWindows()
        if len(objpoints) == 0 or len(imgpoints) == 0: 
            return False
        try:
            ret, camera_matrix, dist_coeffs, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
        except NameError:
            return False
        savetxt('intrinsic-camera.txt', camera_matrix)
        return camera_matrix, dist_coeffs

    def undistortImage(img, intrinsicCameraMatrix = None, distortionCoefficients = None, undistortedImageMultiplication = 1., interpolation=cv2.INTER_LINEAR):
        '''Undistorts the image passed in argument'''
        width = img.shape[1]
        height = img.shape[0]
        [map1, map2] = computeUndistortMaps(width, height, undistortedImageMultiplication, intrinsicCameraMatrix, distortionCoefficients)
        return cv2.remap(img, map1, map2, interpolation=interpolation)


def printCvMat(cvmat, out = stdout):
    '''Prints the cvmat to out'''
    print('Deprecated, use new interface')
    for i in xrange(cvmat.rows):
        for j in xrange(cvmat.cols):
            out.write('{0} '.format(cvmat[i,j]))
        out.write('\n')

def projectArray(homography, points):
    '''Returns the coordinates of the projected points through homography
    (format: array 2xN points)'''
    if points.shape[0] != 2:
        raise Exception('points of dimension {0} {1}'.format(points.shape[0], points.shape[1]))

    if (homography is not None) and homography.size>0:
        #alternatively, on could use cv2.convertpointstohomogeneous and other conversion to/from homogeneous coordinates
        augmentedPoints = append(points,[[1]*points.shape[1]], 0)
        prod = dot(homography, augmentedPoints)
        return prod[0:2]/prod[2]
    else:
        return points

def project(homography, p):
    '''Returns the coordinates of the projection of the point p with coordinates p[0], p[1]
    through homography'''
    return projectArray(homography, array([[p[0]],[p[1]]]))

def projectTrajectory(homography, trajectory):
    '''Projects a series of points in the format
    [[x1, x2, ...],
    [y1, y2, ...]]'''
    return projectArray(homography, array(trajectory))

def invertHomography(homography):
    '''Returns an inverted homography
    Unnecessary for reprojection over camera image'''
    from numpy.linalg import inv
    invH = inv(homography)
    invH /= invH[2,2]
    return invH

def undistortTrajectory(invMap1, invMap2, positions):
    from numpy import floor, ceil
    floorPositions = floor(positions)
    #ceilPositions = ceil(positions)
    undistortedTrajectory = [[],[]]
    for i in xrange(len(positions[0])):
        x,y = None, None
        if positions[0][i]+1 < invMap1.shape[1] and positions[1][i]+1 < invMap1.shape[0]:
            floorX = invMap1[floorPositions[1][i], floorPositions[0][i]]
            floorY = invMap2[floorPositions[1][i], floorPositions[0][i]]
            ceilX = invMap1[floorPositions[1][i]+1, floorPositions[0][i]+1]
            ceilY = invMap2[floorPositions[1][i]+1, floorPositions[0][i]+1]
            #ceilX = invMap1[ceilPositions[1][i], ceilPositions[0][i]]
            #ceilY = invMap2[ceilPositions[1][i], ceilPositions[0][i]]
            if floorX >=0 and floorY >=0 and ceilX >=0 and ceilY >=0:
                x = floorX+(positions[0][i]-floorPositions[0][i])*(ceilX-floorX)
                y = floorY+(positions[1][i]-floorPositions[1][i])*(ceilY-floorY)
        undistortedTrajectory[0].append(x)
        undistortedTrajectory[1].append(y)
    return undistortedTrajectory

def projectGInputPoints(homography, points):
    return projectTrajectory(homography, array(points+[points[0]]).T)

if opencvAvailable:
    def computeTranslation(img1, img2, img1Points, maxTranslation2, minNMatches, windowSize = (5,5), level = 5, criteria = (cv2.TERM_CRITERIA_EPS, 0, 0.01)):
        '''Computes the translation of img2 with respect to img1
        (loaded using OpenCV as numpy arrays)
        img1Points are used to compute the translation

        TODO add diagnostic if data is all over the place, and it most likely is not a translation (eg zoom, other non linear distortion)'''
        from numpy import median, sum

        nextPoints = array([])
        (img2Points, status, track_error) = cv2.calcOpticalFlowPyrLK(img1, img2, img1Points, nextPoints, winSize=windowSize, maxLevel=level, criteria=criteria)
        # calcOpticalFlowPyrLK(prevImg, nextImg, prevPts[, nextPts[, status[, err[, winSize[, maxLevel[, criteria[, derivLambda[, flags]]]]]]]]) -> nextPts, status, err
        delta = []
        for (k, (p1,p2)) in enumerate(zip(img1Points, img2Points)):
            if status[k] == 1:
                dp = p2-p1
                d = sum(dp**2)
                if d < maxTranslation2:
                    delta.append(dp)
        if len(delta) >= minNMatches:
            return median(delta, axis=0)
        else:
            print(dp)
            return None

if skimageAvailable:
    from skimage.feature import hog
    from skimage import color, transform
    
    def HOG(image, rescaleSize = (64, 64), orientations=9, pixelsPerCell=(8, 8), cellsPerBlock=(2, 2), visualize=False, normalize=False):
        bwImg = color.rgb2gray(image)
        inputImg = transform.resize(bwImg, rescaleSize)
        features = hog(inputImg, orientations, pixelsPerCell, cellsPerBlock, visualize, normalize)
        if visualize:
            from matplotlib.pyplot import imshow, figure, subplot
            hogViz = features[1]
            features = features[0]
            figure()
            subplot(1,2,1)
            imshow(inputImg)
            subplot(1,2,2)
            imshow(hogViz)
        return float32(features)

    def createHOGTrainingSet(imageDirectory, classLabel, rescaleSize = (64, 64), orientations=9, pixelsPerCell=(8, 8), cellsPerBlock=(2, 2), visualize=False, normalize=False):
        from os import listdir
        from matplotlib.pyplot import imread

        inputData = []
        for filename in listdir(imageDirectory):
            img = imread(imageDirectory+filename)
            features = HOG(img, rescaleSize, orientations, pixelsPerCell, cellsPerBlock, visualize, normalize)
            inputData.append(features)

        nImages = len(inputData)
        return array(inputData, dtype = float32), array([classLabel]*nImages, dtype = float32)

        
