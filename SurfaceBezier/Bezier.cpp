#include "Bezier.h"
#include "Math.h"
#include <string>

std::vector<Point> deCasteljau(float iter, std::vector<Point> control_points)
{
	std::vector<Point> bezierPoints;
	if (control_points.size() < 4)
		return bezierPoints;
	
	for (double t = 0; t <= 1 + iter; t += 1 / iter)
	{
		if (t > 1)
			t = 1;

		std::vector<Point> temp1 = control_points;

		while (temp1.size()>1)
		{
			std::vector<Point> temp2;

			for (int i = 0; i< temp1.size() - 1; i++)
			{
				Point pt1 = temp1[i];
				Point pt2 = temp1[i + 1];

				temp2.push_back(Approximate(t, pt1, pt2));// center(pt1, pt2));
			}
			temp1 = temp2;
		}
		Point point = temp1[temp1.size() - 1];
		bezierPoints.push_back(point);//finally only one point will be left

		if (t == 1)
			break;
	}
	return bezierPoints;
}

Point Approximate(float t, Point pt1, Point pt2)
{
	double x = pt1.x * (1 - t) + pt2.x * t;
	double y = pt1.y * (1 - t) + pt2.y * t;
	double z = pt1.z * (1 - t) + pt2.z * t;

	return Point(x, y, z);
}

std::vector<Point> BSpline(std::vector<Point> control_points, std::vector<float> node)
{
	std::vector<Point> bezierPoints;

	float p = 3; // degré
	float n = control_points.size() + p; // n - p - 1 = indice où indice = nbPoint - 1
	float pas = ((n - p) - (0 + p)) / n;

	int nbPoints = control_points.size();

	for (double splineNb = 0; splineNb < nbPoints - p; splineNb++)
	{
		std::vector<Point> splineControl;
		splineControl.push_back(control_points[splineNb + 0]);
		splineControl.push_back(control_points[splineNb + 1]);
		splineControl.push_back(control_points[splineNb + 2]);
		splineControl.push_back(control_points[splineNb + 3]);

		for (double t = node[p]; t < node[n - p]; t += pas)
		{
			Point point;
			point.x = 0;
			point.y = 0;
			for (int j = 0; j < 4; j++)
			{
				point.x += int (BPoint(p, j, t, node) * splineControl[j].x);
				point.y += int (BPoint(p, j, t, node) * splineControl[j].y);
			}
			bezierPoints.push_back(point);
		}
	}
	return bezierPoints;
}

float BPoint(int p, int ind, float t, std::vector<float> node)
{
	if ((t < node[ind]) || (t >= node[ind + 1]))
	{
		return 0;
	}
	if (p == 0)
	{
		return 1;
	}

	float resultTemp1, resultTemp2;
	resultTemp1 = (node[ind + p] - node[ind]) != 0 ? ((t - node[ind]) / (node[ind + p] - node[ind])) * BPoint(p - 1, ind, t, node) : 0;
	resultTemp2 = (node[ind + 1 + p] - node[ind + 1]) != 0 ? ((node[ind + 1 + p] - t) / (node[ind + 1 + p] - node[ind + 1])) * BPoint(p - 1, ind + 1, t, node) : 0;

	return resultTemp1 + resultTemp2;
}