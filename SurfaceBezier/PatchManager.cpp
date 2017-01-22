
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