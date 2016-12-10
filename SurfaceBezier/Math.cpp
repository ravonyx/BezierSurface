#include "Math.h"
#include <iostream>
#include <cstdlib>

double factorial(double n)
{
	if (n == 0)
		return 1;
	else
		return n * factorial(n - 1);
}

double combination(double k, double n)
{
	if (n < 1)
	{
		std::cout << "Error : Bad size of n elems - Abort" << std::endl;
		return -1;
	}
	if (k > n)
	{
		std::cout << "Error : K > N - Abort" << std::endl;
		return -1;
	}	
	double result = factorial(n) / (factorial(k) * factorial(n - k));
	return result;
}

// --------------------------------------------------
// Function select_close: Select Point le plus proche
// --------------------------------------------------
void select_close(float x, float y, int& selectI, int& selectJ,const std::vector<std::vector<Point>> &all_control_point)
{
	double prevNorm = sqrt(pow(all_control_point[0][0].x - x, 2) + pow(all_control_point[0][0].y - y, 2));
	selectI = 0;
	selectJ = 0;

	for (unsigned int i = 0; i < all_control_point.size(); i++)
	{
		for (unsigned int j = 0; j < all_control_point[i].size(); j++)
		{
			double norm = sqrt(pow(all_control_point[i][j].x - x, 2) + pow(all_control_point[i][j].y - y, 2));
			if (norm < prevNorm)
			{
				prevNorm = norm;
				selectI = i;
				selectJ = j;
			}
		}
	}
	if (prevNorm > sqrt(25))
	{
		selectI = -1;
		selectJ = -1;
	}
}
Point center(Point p1, Point p2)
{
	Point p = Point((p1.x + p2.x)/2, (p1.y + p2.y)/2, (p1.z + p2.z) / 2);
	return p;
}