#pragma once
#include "Point.h"
#include <vector>
#include <glm\glm.hpp>


double factorial(double n);
//cf wiki for names (k parmi n)
double combination(double k, double n);

Point center(Point p1, Point p2);

void select_close(float x, float y, float z, int& selectI, int& selectJ,const std::vector<std::vector<Point>> &all_control_point);
Point rotate_point(Point &pivot, float angle, Point &p);

float dot(glm::vec3 u, glm::vec3 v);
float norm(glm::vec3 v); // norm = length of  vector
float distance(glm::vec3 u, glm::vec3 v); // distance = norm of difference