#ifndef CAMERA_H
#define CAMERA_H

#define MOVE_SPEED 0.01f
#define SENSIBILITY 0.001f

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Quaternion.hpp"

class Camera
{
public:
	Camera();

	char locked;

	float posx;
	float posy;
	float posz;

	float dirx;
	float diry;
	float dirz;

	char deltaForward;
	char deltaStrafe;

	glm::mat4 _matrix;

	void updatePos(void);
	void releaseCam(void);
	void grabCam(int x, int y);
	void orienterCam(int x, int y);
	glm::mat4 GetOrientation();
	void reset();

protected:
private:
	float angleh;
	float anglev;

	int xOrigin;
	int yOrigin;
	float deltaAnglex;
	float deltaAngley;

};

#endif // CAMERA_H
