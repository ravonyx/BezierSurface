#pragma once
#include <vector>
#include "Point.h"
std::vector<Point> deCasteljau(float iter, std::vector<Point> control_points);
//std::vector<Point> deCasteljau(float iter, std::vector<std::vector<Point>> &all_control_points, int degre = 3, int offset = 4);
std::vector<Point> BSpline(std::vector<Point> control_points, std::vector<float> node);
float BPoint(int p, int ind, float t, std::vector<float> node);
Point Approximate(float t, Point pt1, Point pt2);