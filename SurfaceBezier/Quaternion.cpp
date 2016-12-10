#include "Quaternion.hpp"

Quaternion::Quaternion()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Quaternion Quaternion::QuaternionFromAxis(float axis[], float angle)
{
	x = axis[0] * sin(angle / 2.0f);
	y = axis[1] * sin(angle / 2.0f);
	z = axis[2] * sin(angle / 2.0f);
	w = cos(angle / 2.0f);

	return *this;
}

glm::mat4 Quaternion::QuaternionToMatrix()
{
	glm::mat4 rot_matrix;

	rot_matrix[0].x = 1 - 2 * (pow(y, 2) + pow(z, 2));
	rot_matrix[0].y = 2 * (x*y + w*z);
	rot_matrix[0].z = 2 * (x*z - w*y);
	rot_matrix[0].w = 0;

	rot_matrix[1].x = 2 * (x*y - w*z);
	rot_matrix[1].y = 1 - 2 * (pow(x, 2) + pow(z, 2));
	rot_matrix[1].z = 2 * (y*z + w*x);
	rot_matrix[1].w = 0;

	rot_matrix[2].x = 2 * (x*z + w*y);
	rot_matrix[2].y = 2 * (y*z - w*x);
	rot_matrix[2].z = 1 - 2 * (pow(x,2) + pow(y,2));
	rot_matrix[2].w = 0;

	rot_matrix[3].x = 0;
	rot_matrix[3].y = 0;
	rot_matrix[3].z = 0;
	rot_matrix[3].w = 1;

	return rot_matrix;
}

Quaternion Quaternion::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& qt)
{
	os << "x:" << qt.x << " " << "y:" << qt.y << " " << "z:" << qt.z << " " << "w:" << qt.w << std::endl;
	return os;
}

