#include "cvutils.hpp"
#include "utils.hpp"

#include "klt.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define FEAT_VAL(x) ((x) > 0) ? 1 : x // 1 means the features was replaced

void cvGetCharArray(IplImage *image, unsigned char* img, int width, int height);

int main(int argc, char *argv[]) {
  //cout << "Hello World" << endl;

  KLT_TrackingContext tc;
  KLT_FeatureList fl;
  KLT_FeatureTable ft;
  KLT_Feature feature;

  int i,j;
  string fnamein, fnameout, tmp;
  string sequenceDir="../test-images";

  IplImage *image;
  int width, height;
  unsigned char *img1, *img2;

//   CvCapture *inputVideo = 0;
//   if (argc == 1)
//     inputVideo = cvCreateCameraCapture(-1);
//   else
//     inputVideo = cvCaptureFromFile(argv[1]);

  int frameNum = 0;

  if (argc < 7){
    printf("\ntrack sequenceFile startFrame numFrames numFeatures min_dist window_size\n");
    return (-1);
  }

  // parameters for tracking
  char* sequenceFile = argv[1];
  int startFrame = (int) strtol(argv[2],NULL,10);
  int nFrames = (int) strtol(argv[3],NULL,10);
  int nFeatures = (int) strtol(argv[4],NULL,10);
  int mindist = (int) strtol(argv[5],NULL,10);
  int window_size = (int) strtol(argv[6],NULL,10);

  fnamein = sequenceFile;//sequenceDir+"/"++".avi"
  CvCapture* sequence = cvCaptureFromFile(fnamein.c_str());
  if (sequence == NULL) {
    cout << "Pb reading " << fnamein << " file.";
    exit(0);
  }
    

  // from http://ai.stanford.edu/~dstavens/cs223b/optical_flow_demo.cpp
  /* This is a hack. If we don't call this first then getting capture
   * properties (below) won't work right. This is an OpenCV bug. We
   * ignore the return value here. But it's actually a video frame.
   */

  cvQueryFrame(sequence);
  /* Read the video's frame size out of the AVI. */
  CvSize frame_size;
  frame_size.height = cvGetCaptureProperty(sequence, CV_CAP_PROP_FRAME_HEIGHT );
  frame_size.width = cvGetCaptureProperty(sequence, CV_CAP_PROP_FRAME_WIDTH );
  /* Determine the number of frames in the AVI. */
  int number_of_frames;
  /* Go to the end of the AVI (ie: the fraction is "1") */
  cvSetCaptureProperty(sequence, CV_CAP_PROP_POS_AVI_RATIO, 1.);
  /* Now that we're at the end, read the AVI position in frames */
  number_of_frames = cvGetCaptureProperty( sequence, CV_CAP_PROP_POS_FRAMES );
  /* Return to the beginning */
  cvSetCaptureProperty(sequence, CV_CAP_PROP_POS_FRAMES, 0. );
  printf("%d %d %d\n", frame_size.height, frame_size.width, number_of_frames);

  if (nFrames < 0)
    nFrames = number_of_frames;

  // go forward to the startFrame
  //cvNamedWindow("Image",1);

  for (i=1; i<startFrame; i++) { // pb if more than the number of frames
      image = cvQueryFrame(sequence);
      //if (i%1000==0) {
      //printf("%d\n", i);
      //cvShowImage("Image",image);
      //WaitKey(0);
      //}
    }

  img1 = (unsigned char*)malloc(frame_size.height*frame_size.width*sizeof(unsigned char));
  img2 = (unsigned char*)malloc(frame_size.height*frame_size.width*sizeof(unsigned char));

  // KLT initialization
  tc = KLTCreateTrackingContext();
  fl = KLTCreateFeatureList(nFeatures);
  ft = KLTCreateFeatureTable(nFrames, nFeatures);
  tc->sequentialMode = TRUE;
  tc->mindist = mindist;
  tc->window_width = window_size;
  tc->window_height = window_size;
  KLTSetVerbosity(0);
  tc->affineConsistencyCheck = 2;

  // initialization of the first frame
  image = cvQueryFrame(sequence);
  cvGetCharArray(image, img1, frame_size.width, frame_size.height);

  KLTSelectGoodFeatures(tc, img1, frame_size.width, frame_size.height, fl);
  KLTStoreFeatureList(fl, ft, 0);
  //KLTWriteFeatureListToPPM(fl, img1, frame_size.width, frame_size.height, "feat0000.ppm");

  i=1;
  while (((image = cvQueryFrame(sequence)) != NULL) && (i<nFrames)) {
    cvGetCharArray(image, img2, frame_size.width, frame_size.height);

    // feature tracking
    KLTTrackFeatures(tc, img1, img2, frame_size.width, frame_size.height, fl);
    KLTReplaceLostFeatures(tc, img2, frame_size.width, frame_size.height, fl);
    KLTStoreFeatureList(fl, ft, i);

    img1 = img2;
    // unquote if you want to write the frames with the tracked features.
    //sprintf(fnameout, "feat%04d.ppm", startFrame+i);
    //KLTWriteFeatureListToPPM(fl, img2, frame_size.width, frame_size.height, fnameout);
    //printf("%04d\n", i);
    i++;
  }

  printf("%d\n", i);

  // writing feature file
  fnameout = "features.txt";//"features-"%s"-"%d"-"%d"-"%d".txt", sequenceFile, nFeatures, tc->mindist, tc->window_width);
  //KLTWriteFeatureTable(ft, fnameout, "%5.1f");

  int precision = 2;
  ofstream out;
  ::openWriteScientificPrecision(out, fnameout, precision);

  //fprintf(out,"%%StartFrame: 0\n");
  //fprintf(out,"%%NumFrames: %d\n",nFrames);
    
  // 1 feature / line
  // x1 y1 val1 x2 y2 val2 ... (1 2... frame numbers)
  for (j = 0 ; j < ft->nFeatures ; j++)  {
    int firstFrameNum = startFrame;
    stringstream lx, ly;
    for (i = 0 ; i < ft->nFrames ; i++){
      feature = ft->feature[j][i];
      //fprintf(out,"%.2f %.2f %d ",(float)feature->x,(float)feature->y,FEAT_VAL(feature->val));
      lx << feature->x << " ";
      ly << feature->y << " ";
      if (feature->val <= 0) {
	// print the feature
	out << firstFrameNum <<  " " << startFrame+i << endl;
	out << lx << endl;
	out << ly << endl;
	// velocity
	out << "%" << endl;
	firstFrameNum = i+1;
	lx.clear();
	ly.clear();
      }
    }
  }
    
  cvReleaseCapture(&sequence);

  return 1;
}

// converts a frame from IplImage format (OpenCV) to an array of unsigned char for KLT
void cvGetCharArray(IplImage *image, unsigned char* img, int width, int height) {
  int x, y;
  
  for(y=0 ; y < height ; y++) {
    for(x=0; x < width ; x++) {
      img[(height-y-1)*width+x] = (image->imageData+image->widthStep*y)[x*3];
    }
  }
}
