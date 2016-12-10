#pragma once
#include "Point.h"

class Vertex
{
	private:
		Point p;
		int x;
		int y;
	public:
		Vertex();
		Vertex(Point _p, int _x, int _y);
};

