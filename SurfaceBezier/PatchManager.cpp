
#include "PatchManager.h"


PatchManager::PatchManager()
{
	nbRows = 3;
	nbPointsPerRow = 3;
	allControlPoints = std::vector<std::vector<Point>>(3);
}

PatchManager::PatchManager(float inbRows, float inbPointsPerRow)
{
	nbRows = inbRows;
	nbPointsPerRow = inbPointsPerRow;
	allControlPoints = std::vector<std::vector<Point>>(nbRows);
}

void PatchManager::generateControlPoints()
{
	if (nbRows >= 3 || nbPointsPerRow >= 3)
	{
		float step = 3.0 / nbRows;
		float stepPoints = 3.0 / nbPointsPerRow;

		float z = -1;
		float x = -1;

		for (int i = 0; i < nbRows; i++)
		{
			for (int j = 0; j < nbPointsPerRow; j++)
			{
				allControlPoints[i].push_back(Point(x, 0.5, z));
				x += stepPoints;
			}
			x = -1;
			z += step;
		}
	}
	majControlPoints();
	colors.resize(controlPoints.size());
}

void PatchManager::majControlPoints()
{
	controlPoints.clear();
	for (int i = 0; i < allControlPoints.size(); i++)
	{
		for (int j = 0; j < allControlPoints[i].size(); j++)
		{
			controlPoints.push_back(allControlPoints[i][j]);
		}
	}
}

void PatchManager::randomizeControlPoints()
{
	for (int i = 0; i < allControlPoints.size(); i++)
	{
		for (int j = 0; j < allControlPoints[i].size(); j++)
		{
			//allControlPoints[i][j].x = getRandomNumber(-10, 10) * 0.1f;
			allControlPoints[i][j].y = getRandomNumber(0, 1);
		}
	}
	majControlPoints();
}


float PatchManager::getRandomNumber(int a, int b)
{
	return a + ((float)rand() / (float)RAND_MAX) * (b - a);
}

std::vector<Point>& PatchManager::getControlPoints()
{
	return controlPoints;
}

std::vector<std::vector<Point>>& PatchManager::getAllControlPoints()
{
	return allControlPoints;
}

std::vector<Color>& PatchManager::getColors()
{
	return colors;
}

std::vector<Point> PatchManager::getBezierCurveOnRow(std::vector<Point> points, int steps, std::vector<int> parameterSpace)
{
	std::vector<glm::vec3> gPoints = std::vector<glm::vec3>();
	parameterSpace = std::vector<int>();
	parameterSpace.push_back(0);
	parameterSpace.push_back(10);
	for (int i = 0; i < points.size(); i++)
	{
		gPoints.push_back(glm::vec3(points[i].x, points[i].y, points[i].z));
	}
	bezierManager.CasteljauBezier(gPoints, 20, parameterSpace);
	return std::vector<Point>();
}
