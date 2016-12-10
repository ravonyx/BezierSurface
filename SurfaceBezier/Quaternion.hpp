#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h> 
#include <iostream>
#include <ostream>
#include <glm\glm.hpp>

class Quaternion
{
private:
	
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion();
	Quaternion(float _x, float _y, float _z, float _w);
	Quaternion QuaternionFromAxis(float axis[], float angle);
	glm::mat4 QuaternionToMatrix();
	Quaternion Conjugate();

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& qt);

	Quaternion& operator=(Quaternion const& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
		return *this;
	}
	Quaternion operator+(Quaternion const& a)
	{
		float newx = x + a.x;
		float newy = y + a.y;
		float newz = z + a.z;
		float neww = w + a.w;
		return Quaternion(newx, newy, newz, neww);
	}
	Quaternion operator* (const Quaternion &rq) const
	{
		return Quaternion
			(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
			w * rq.y + y * rq.w + z * rq.x - x * rq.z,
			w * rq.z + z * rq.w + x * rq.y - y * rq.x,
			w * rq.w - x * rq.x - y * rq.y - z * rq.z);
	}
};

#endif