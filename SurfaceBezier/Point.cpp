#include "Point.h"

Point::Point(float x, float y, float z) : x(x), y(y), z(z){}
Point::Point() : x(0), y(0), z(0) {}
void Point::print()
{
	std::cout << "X : " << x << " Y : " << y << " Z : " << z << std::endl;
}