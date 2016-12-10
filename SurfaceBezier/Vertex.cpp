#include "Vertex.h"

Vertex::Vertex()
{
	p = Point();
	x = 0;
	y = 0;
}

Vertex::Vertex(Point _p, int _x, int _y)
{
	p = _p;
	x = _x;
	y = _y;
}

