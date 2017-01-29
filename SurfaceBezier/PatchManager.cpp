
#include "PatchManager.h"


PatchManager::PatchManager()
{
	nbRows = 3;
	nbPointsPerRow = 3;
	allControlPoints = std::vector<std::vector<Point>>(3);
	allCasteljauPoints = std::vector < std::vector<Point>>(3);
}

PatchManager::PatchManager(float inbRows, float inbPointsPerRow)
{
	nbRows = inbRows;
	nbPointsPerRow = inbPointsPerRow;
	allControlPoints = std::vector<std::vector<Point>>(nbRows);
	allCasteljauPoints = std::vector < std::vector<Point>>(nbRows);
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

void PatchManager::majCasteljauPoints()
{
	casteljauPoints.clear();
	for (int i = 0; i < allCasteljauPoints.size(); i++)
	{
		for (int j = 0; j < allCasteljauPoints[i].size(); j++)
		{
			casteljauPoints.push_back(allCasteljauPoints[i][j]);
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

void PatchManager::getBezierCurveOnRow(std::vector<Point> controlPoints, int steps, std::vector<int> parameterSpace)
{
	std::vector<glm::vec3> gPoints = std::vector<glm::vec3>();
	parameterSpace = std::vector<int>();
	parameterSpace.push_back(0);
	parameterSpace.push_back(10);
	
	for (int indexRow = 0; indexRow < nbRows; indexRow++)
	{
		int max = nbPointsPerRow * indexRow + nbPointsPerRow;

		gPoints.clear();
		for (int i = nbPointsPerRow * indexRow; i < max; i++)
		{
			gPoints.push_back(glm::vec3(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z));
		}

		std::vector<glm::vec3> points = bezierManager.CasteljauBezier(gPoints, 20, parameterSpace);
		for (int i = 0; i < points.size(); i++)
		{
			allCasteljauPoints[indexRow].push_back(Point(points[i].x, points[i].y, controlPoints[nbPointsPerRow * indexRow].z));
		}
	}
	
	majCasteljauPoints();
}

std::vector<Point>& PatchManager::getControlPoints()
{
	return controlPoints;
}

std::vector<Point>& PatchManager::getCasteljauPoints()
{
	return casteljauPoints;
}

std::vector<std::vector<Point>>& PatchManager::getAllControlPoints()
{
	return allControlPoints;
}

std::vector<Color>& PatchManager::getColors()
{
	return colors;
}
