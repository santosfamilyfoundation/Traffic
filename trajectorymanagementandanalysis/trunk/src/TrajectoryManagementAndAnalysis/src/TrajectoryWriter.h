#ifndef TRAJECTORYWRITER_H_
#define TRAJECTORYWRITER_H_

#include <fstream>

#include "Trajectory.h"

/**
 * TrajectoryWriter class.
 *
 * The TrajectoryWriter class allows to save trajectories in different formats like image files, Matlab, Scilab or FreeMat files.
 */
class TrajectoryWriter
{
public:
	/**
	 * Save the trajectory in the image.
	 *
	 * @param[in] trajectory trajectory
	 * @param[in] color color of the trajectory
	 * @param[in] img image on which the trajectory should be saved
	 * @return image with the saved trajectory
	 */
	template<typename T>
	  void write(const Trajectory<T> &trajectory, CvScalar color, cv::Mat& img = cv::Mat::Mat())
	{
	  if (img.empty())
	    {
	      img = createImg(trajectory);
	    }
	  
	  writeOnNotNullImg(trajectory, color, img);
	}

	/**
	 * Save trajectories in the image.
	 *
	 * @param[in] img image on which the trajectory should be saved
	 * @param[in] trajectories trajectories
	 * @param[in] colors colors of trajectories
	 * @return image with saved trajectories
	 */
	template<typename T>
	  void saveInIplImage(cv::Mat& img, std::vector<Trajectory<T> > *trajectories, std::vector<CvScalar> *colors)
	{
	  assert( !img.empty());
	  assert( trajectories != NULL );
	  assert( trajectories->size() > 0 );
	  assert( colors != NULL );
	  assert( colors->size() > 0 );

	  unsigned nbOfColors = colors->size();
	  
	  for (unsigned i = 0; i < trajectories->size(); ++i)
	    {
	      Trajectory<T> trajectory = (*trajectories)[i];
	      CvScalar color = (*colors)[i % nbOfColors];
	      writeOnNotNullImg(trajectory, color, img);
	    }
	}

	/**
	 * Save trajectories in the Matlab format file.
	 *
	 * @param[in] file name of the file
	 * @param[in] trajectories trajectories
	 * @param[in] writeWithVisualization information if functions to visualize data should be written into the file
	 * @param[in] plotCommand2D command to plot in 2D
	 * @param[in] plotCommand3D command to plot in 3D
	 */
	template<typename T>
	void saveInMatlabFormat(const char *file, std::vector<Trajectory<T> > *trajectories, bool writeWithVisualization = true, const char *plotCommand2D = "plot", const char *plotCommand3D = "plot3d")
	{
		saveInMatlabScilabFormat(file, trajectories, writeWithVisualization, plotCommand2D, plotCommand3D, true);
	}

	/**
	 * Save trajectories in the Scilab format file.
	 *
	 * @param[in] file name of the file
	 * @param[in] trajectories trajectories
	 * @param[in] writeWithVisualization information if functions to visualize data should be written into the file
	 * @param[in] plotCommand2D command to plot in 2D
	 * @param[in] plotCommand3D command to plot in 3D
	 */
	template<typename T>
	void saveInScilabFormat(const char *file, std::vector<Trajectory<T> > *trajectories, bool writeWithVisualization = true, const char *plotCommand2D = "plot", const char *plotCommand3D = "plot3d")
	{
		saveInMatlabScilabFormat(file, trajectories, writeWithVisualization, plotCommand2D, plotCommand3D, false);
	}

private:
	/**
	 * Create image on which the trajectory can be saved.
	 *
	 * @param[in] trajectory trajectory
	 * @return image
	 */
	template<typename T>
	  cv::Mat createImg(const Trajectory<cv::Point_<T> > &trajectory)
	{
		cv::Point_<T> _min = min(trajectory);
		cv::Point_<T> _max = max(trajectory);
		CvSize imgSize = cvSize(_max.x + 1, _max.y + 1);
		cv::Mat img = cv::Mat::zeros(imgSize, CV_8UC3);
		return img;
	}

	/**
	 * Save the trajectory in the image.
	 *
	 * @param[in] trajectory trajectory
	 * @param[in] color color of the trajectory
	 * @param[in] img image on which the trajectory should be saved
	 * @return image with the saved trajectory
	 */
	template<typename T>
	  void writeOnNotNullImg(const Trajectory<T> &trajectory, CvScalar color, cv::Mat& img)
	{
	  assert( !img.empty() );
		for (unsigned int i = 1; i < trajectory.size(); ++i)
		{
			const T p1 = trajectory.getPoint(i - 1);
			const T p2 = trajectory.getPoint(i);
			cv::line(img, p1, p2, color);
		}
	}

	/**
	 * Save trajectories in the Matlab or Scilab format file.
	 *
	 * @param[in] file name of the file
	 * @param[in] trajectories trajectories
	 * @param[in] writeWithVisualization information if functions to visualize data should be written into the file
	 * @param[in] plotCommand2D command to plot in 2D
	 * @param[in] plotCommand3D command to plot in 3D
	 * @param[in] matlab information whether trajectories should be saved in Matlab or Scilab format
	 */
	template<typename T>
	void saveInMatlabScilabFormat(const char *file, std::vector<Trajectory<T> > *trajectories, bool writeWithVisualization, const char *plotCommand2D, const char *plotCommand3D, bool matlab)
	{
		if (trajectories == NULL)
		{
			assert( false );
		}

		std::stringstream matlabFileSS;

		if (matlab)
		{
			matlabFileSS << "hold on;\n";
		}

		for (unsigned i = 0; i < trajectories->size(); ++i)
		{
			matlabFileSS << "T";
			matlabFileSS << i << "=[";

			for (unsigned j = 0; j < (*trajectories)[i].size(); ++j)
			{
				matlabFileSS << (*trajectories)[i].getPoint(j);

				if (j < (*trajectories)[i].size() - 1)
				{
					matlabFileSS << ";";
				}
			}

			matlabFileSS << "];\n";

			if (writeWithVisualization)
			{
				matlabFileSS << "if size(T" << i << ",2) == 2\n";
				matlabFileSS << "\t\t" << plotCommand2D << "( T" << i << "(:,1), T" << i << "(:,2) );\n";
				matlabFileSS << "end;\n";
				matlabFileSS << "if size(T" << i << ",2) == 3\n";
				matlabFileSS << "\t\t" << plotCommand3D << "( T" << i << "(:,1), T" << i << "(:,2), T" << i << "(:,3) );\n";
				matlabFileSS << "end;\n";
			}
		}

		std::ofstream matlabFile;
		matlabFile.open(file, std::ios::out);
		matlabFile << matlabFileSS.str();
		matlabFile.flush();
		matlabFile.close();
	}
};

#endif /* TRAJECTORYWRITER_H_ */
