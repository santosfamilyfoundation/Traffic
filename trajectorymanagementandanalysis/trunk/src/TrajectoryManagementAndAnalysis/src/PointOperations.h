#ifndef POINTOPERATIONS_H_
#define POINTOPERATIONS_H_

#include "opencv2/core/core.hpp"

template<typename Ti, typename Tr>
  std::basic_istream<Ti>& operator>>(std::basic_istream<Ti>& in, cv::Point_<Tr>& point)
{
	Tr x, y;
	if (in >> x >> y)
	{
		point.x = x;
		point.y = y;
	}
	return in;
}

template<typename Ti, typename Tr>
std::basic_istream<Ti>& operator>>(std::basic_istream<Ti>& in, cv::Point3_<Tr>& point)
{
	Tr x, y, z;
	if (in >> x >> y >> z)
	{
		point.x = x;
		point.y = y;
		point.z = z;
	}
	return in;
}


template<typename Tr>
inline std::ostringstream& operator<<(std::ostringstream& out, const cv::Point_<Tr>& point)
{
	out << point.x << " " << point.y;
	return out;
}

template<typename Tr>
inline std::ostringstream& operator<<(std::ostringstream& out, const cv::Point3_<Tr>& point)
{
	out << point.x << " " << point.y << " " << point.z;
	return out;
}

#endif /* POINTOPERATIONS_H_ */
