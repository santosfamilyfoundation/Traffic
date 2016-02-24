#include "../src/TrajectoryWriter.h"

#include "opencv2/highgui/highgui.hpp"

#include <iostream>

using namespace std;

const unsigned nbOfPoints = 10;
const char* matlabFileName = "matlabsample.m";
const char* scilabFileName = "scilabsample.sce";
const char* iplImageFileName = "iplimagesample.jpg";

int main()
{
	TrajectoryPoint trajectoriesTable[3];
	for (unsigned i = 0; i < nbOfPoints; ++i)
	{
		unsigned y = i % 2 ? i : nbOfPoints - i;
		trajectoriesTable[0].add(cvPoint(i * 20, y * 20));
		trajectoriesTable[1].add(cvPoint(i * 20, (y + 2) * 20));
		trajectoriesTable[2].add(cvPoint(i * 20, (y + 4) * 20));
	}

	vector<TrajectoryPoint> *trajectoriesVector = new vector<TrajectoryPoint> (
			trajectoriesTable, trajectoriesTable + 3);

	TrajectoryWriter trajectoryWriter;

	cout << "Saving trajectories in " << matlabFileName
			<< " file (Matlab & FreeMat format).." << endl;
	trajectoryWriter.saveInMatlabFormat<cv::Point_<int> > (matlabFileName,
			trajectoriesVector);

	cout << "Saving trajectories in " << scilabFileName
			<< " file (Scilab format).." << endl;
	trajectoryWriter.saveInScilabFormat<cv::Point_<int> > (scilabFileName,
			trajectoriesVector);

	const CvScalar colorsTable[3] =
	{ CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(0,0,255) };

	vector<CvScalar> *colorsVector = new vector<CvScalar> (colorsTable,
			colorsTable + 3);

	cout << "Saving trajectories in " << iplImageFileName << " file.." << endl;
	cv::Mat img = cv::Mat::zeros(200, 300, CV_8UC3);
	trajectoryWriter.saveInIplImage<cv::Point_<int> > (img, trajectoriesVector, colorsVector);
	imwrite(iplImageFileName, img);

	delete colorsVector;
	delete trajectoriesVector;

	cout << "Done." << endl;

	return 0;
}
