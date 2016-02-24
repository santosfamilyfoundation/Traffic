#ifndef TRAJECTORYGENERATOR_H_
#define TRAJECTORYGENERATOR_H_

#include <ctime>

TrajectoryPoint3d *generateFixedTrajectory(unsigned int id, unsigned int size, double valueX, double valueY, double valueZ)
{
	TrajectoryPoint3d *trajectory = new TrajectoryPoint3d();
	trajectory->setId(id);
	for (unsigned int i = 0; i < size; ++i)
	{
		cv::Point3_<double> p(valueX, valueY, valueZ);
		trajectory->add(p);
	}
	return trajectory;
}

TrajectoryPoint *generateTrajectory(unsigned int id, unsigned int size, unsigned int maxDistanceBwPoints)
{
	TrajectoryPoint *trajectory = new TrajectoryPoint();
	trajectory->setId(id);
	for (unsigned int i = 0; i < size; ++i)
	{
		unsigned int px = rand() % maxDistanceBwPoints;
		unsigned int py = rand() % maxDistanceBwPoints;
		cv::Point_<int> p(px, py);
		if (i > 0)
		{
			p += (*trajectory)[i - 1];
		}
		trajectory->add(p);
	}
	return trajectory;
}

std::vector<TrajectoryPoint> *generateTrajectories(unsigned int quantity, unsigned int length, unsigned int maxDistanceBwPoints)
{
  std::vector<TrajectoryPoint> *trajectories = new std::vector<TrajectoryPoint> ();

	for (unsigned int i = 0; i < quantity; ++i)
	{
		TrajectoryPoint *t = generateTrajectory(i + 1, length, maxDistanceBwPoints);
		trajectories->push_back(*t);
	}

	return trajectories;
}

#endif /* TRAJECTORYGENERATOR_H_ */
