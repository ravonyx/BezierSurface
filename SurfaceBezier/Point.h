#pragma once

#include <iostream>
#include <ostream>

struct Color
{
	float r, g, b, a;
	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
	Color() : r(0.0f), g(0.0f), b(1.0f), a(1.0f) {}
};

class Point
{
	private:
	
	
	public:
		float x;
		float y;
		float z;
		Point(float x, float y, float z);
		Point();

		void print();

		friend std::ostream& operator<< (std::ostream& stream, const Point& p) 
		{
			stream << "X: " << p.x << " Y: " << p.y << " Z : " << p.z << std::endl;
			return stream;
		}

		bool Point::operator ==(const Point& p)
		{
			if (x == p.x && y == p.y && z == p.z)
				return true;
			else
				return false;
		}

		bool Point::operator !=(const Point& p)
		{
			if (*this == p)
				return false;
			else
				return true;
		}

		Point Point::operator +(Point p)
		{
			return Point(x + p.x, y + p.y, z + p.z);
		}
		Point Point::operator -(Point p)
		{
			return Point(x - p.x, y - p.y, z - p.z);
		}

		Point Point::operator *(float s)
		{
			return Point(s * x, s * y, s * z);
		}
};

