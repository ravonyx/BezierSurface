#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include "Camera.h"
#include <GL/glew.h>

Camera::Camera()
{
    deltaForward = 0;
    deltaStrafe = 0;

    posx = 0.0f;
    posy = 0.0f;
    posz = -1.0f;

    dirx = 0.0f;
    diry = 0.0f;
    dirz = -1.0f;

	angleh = M_PI / 8.0f;// 0.0f;
	anglev = 0.0f;

    xOrigin = -1;
    yOrigin = -1;

    deltaAnglex = 0.0f;
    deltaAngley = 0.0f;

    locked = 1;
}

void Camera::reset()
{
	deltaForward = 0;
	deltaStrafe = 0;

	posx = 0.0f;
	posy = 0.0f;
	posz = -1.0f;

	dirx = 0.0f;
	diry = 0.0f;
	dirz = -1.0f;

	angleh = M_PI / 8.0f;// 0.0f;
	anglev = 0.0f;

	xOrigin = -1;
	yOrigin = -1;

	deltaAnglex = 0.0f;
	deltaAngley = 0.0f;

	locked = 1;

	_matrix = glm::mat4(1.0);
}

void Camera::updatePos()
{
    if (deltaForward || deltaStrafe)
    {
        posx += deltaForward * dirx * MOVE_SPEED;
        posy += deltaForward * diry * MOVE_SPEED;
        posz += deltaForward * dirz * MOVE_SPEED;
        posx += deltaStrafe * dirz * MOVE_SPEED;
        posz -= deltaStrafe * dirx * MOVE_SPEED;
    }
}
void Camera::orienterCam(int x, int y)
{
	if (xOrigin >= 0)
	{
		// Calcul de l'angle horizontal
		deltaAnglex = (x - xOrigin) * SENSIBILITY;
		// Correction de l'angle ]-Pi; Pi[
		while (deltaAnglex + angleh > M_PI)
			deltaAnglex -= M_PI * 2;
		while (deltaAnglex + angleh < -M_PI)
			deltaAnglex += M_PI * 2;

		// Calcul de l'angle vertical
		deltaAngley = (y - yOrigin) * SENSIBILITY;
		// Limitation de l'angle (limite haute)
		if (deltaAngley + anglev > M_PI_2)
			deltaAngley = M_PI_2 - anglev - 0.01f;
		// Limitation de l'angle (limite basse)
		if (deltaAngley + anglev < -M_PI_2)
			deltaAngley = -M_PI_2 - anglev + 0.01f;

		dirx = sin(angleh + deltaAnglex)*cos(anglev + deltaAngley);
		diry = -sin(anglev + deltaAngley);
		dirz = -cos(angleh + deltaAnglex)*cos(anglev + deltaAngley);

		Quaternion q1 = Quaternion();
		Quaternion q2 = Quaternion();

		float axisX[3] = { 1.0f, 0.0f, 0.0f };
		float axisY[3] = { 0.0f, 1.0f, 0.0f };

		q1 = q1.QuaternionFromAxis(axisX, anglev + deltaAngley);
		q2 = q2.QuaternionFromAxis(axisY, angleh + deltaAnglex);

		Quaternion qResult;
		qResult = q1 *q2;

		_matrix = qResult.QuaternionToMatrix();
	}
}

glm::mat4 Camera::GetOrientation()
{
	return _matrix;
}

void Camera::releaseCam()
{
    angleh += deltaAnglex;
    anglev += deltaAngley;

    yOrigin = -1;
    xOrigin = -1;
}
void Camera::grabCam(int x, int y)
{
    xOrigin = x;
    yOrigin = y;
}