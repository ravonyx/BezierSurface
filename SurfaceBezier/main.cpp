#include <gl\glew.h>

#pragma region my_include
#include "Math.h"
#include "Bezier.h"
#include "Camera.h"
#include "EsgiShader.h"
#include "PatchManager.h"
#pragma endregion

#pragma region general_include
#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#include <Windows.h>
#endif
#include <GL\freeglut.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\string_cast.hpp>
#include <glm\gtx\transform.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <AntTweakBar.h>
#pragma endregion

std::vector<Point> lines;

int current_index = 0;
int width = 1500;
int height = 800;

float first_mousex, first_mousey;
float last_mousex, last_mousey;
float last_movex, last_movey;

int _selectMovePointI = -1, _selectMovePointJ = -1;

Quaternion rotation;

glm::mat4 proj;
glm::mat4 view;

EsgiShader basicShader, gridShader;
GLuint vaoPoint, vaoLine, vertexBufferPoints, vertexBufferColors, vertexBufferLine;
GLuint mvp_location, position_location, color_location;

/*var shaders*/
int programBasicID;
GLuint attr_color;

Camera *cam;
bool mode_ui, decrease;
int index;
std::string indexStr;

std::string infos;
std::vector <Point>lineVector;

PatchManager patchMng;

#pragma region header_function
static  void __stdcall exitCallbackTw(void* clientData);
void idle();
void display(void);
void computePos(int unused);
void terminate();

void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardSpecDown(int key, int x, int y);
void keyboardSpecUp(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void reshape(int w, int h);
#pragma endregion

template<typename T>
void majBuffer(int vertexBuffer, std::vector<T> &vecteur);

int main(int argc, char** argv)
{
	TwBar *bar;
	cam = new Camera();
	mode_ui = false;
	decrease = false;
	infos = "In mode Camera";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("OpenGL");
	TwWindowSize(width, height);
	TwInit(TW_OPENGL, NULL);
	glewInit();

	//Callback functions
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	//glutMouseWheelFunc(mouseZoom);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, computePos, 0);

	/*Load shader and get attributes*/
	printf("Load Fragment basic shader\n");
	basicShader.LoadFragmentShader("basicShader.frag");
	printf("Load Vertex basic shader\n");
	basicShader.LoadVertexShader("basicShader.vert");
	basicShader.Create();
	printf("Load Fragment grid shader\n");
	gridShader.LoadFragmentShader("gridShader.frag");
	printf("Load Vertex grid shader\n");
	gridShader.LoadVertexShader("gridShader.vert");
	printf("Load Geom grid shader\n");
	gridShader.LoadGeometryShader("gridShader.geom");
	gridShader.Create();

	position_location = glGetAttribLocation(basicShader.GetProgram(), "a_position");
	color_location = glGetAttribLocation(basicShader.GetProgram(), "a_color");

	GLuint uniVP = glGetUniformLocation(gridShader.GetProgram(), "VP");
	gridShader.SetVP(uniVP);
	uniVP = glGetUniformLocation(basicShader.GetProgram(), "VP");
	basicShader.SetVP(uniVP);
	GLuint uniM = glGetUniformLocation(basicShader.GetProgram(), "M");
	basicShader.SetM(uniM);

	rotation = Quaternion();
	patchMng = PatchManager(5.0, 3.0);

	/*VAO Points*/
	glGenVertexArrays(1, &vaoPoint);
	glBindVertexArray(vaoPoint);
	
	patchMng.generateControlPoints();
	std::vector<Point> controlPoints = patchMng.getControlPoints();
	std::vector<Color> colors = patchMng.getColors();

	glGenBuffers(1, &vertexBufferPoints);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPoints);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * controlPoints.size(), controlPoints.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &vertexBufferColors);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * colors.size(), colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

	colors[0].r = 1.0f; colors[0].g = 1.0f; colors[0].b = 1.0f;
	majBuffer(vertexBufferColors, colors);
	index = 0;
	indexStr = std::to_string(index);

	/*VAO Line*/
	glGenVertexArrays(1, &vaoLine);
	glBindVertexArray(vaoLine);

	glGenBuffers(1, &vertexBufferLine);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * lineVector.size(), lineVector.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

	/** GESTION SOURIS **/
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(NULL);
	glutSpecialFunc(keyboardSpecDown);
	glutSpecialUpFunc(keyboardSpecUp);

	// Create a tweak bar
	bar = TwNewBar("BezierSurface");

	TwDefine(" BezierSurface size='200 300' color='86 101 115' valueswidth=fit ");
	float refresh = 0.1f;
	TwSetParam(bar, NULL, "refresh", TW_PARAM_FLOAT, 1, &refresh);
	TwAddVarRO(bar, "Output", TW_TYPE_STDSTRING, &infos," label='Infos' ");
	//TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &light_direction, " label='Light direction' opened=true help='Change the light direction.' ");
	TwAddVarRO(bar, "Index", TW_TYPE_STDSTRING, &indexStr, " label='Index' help='Change index of the point' ");


	TwAddSeparator(bar, "settings object", "");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation, " label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddSeparator(bar, "program", "");
	TwAddButton(bar, "Exit", &exitCallbackTw, nullptr, "");

	_selectMovePointI = -1;
	_selectMovePointJ = -1;

	glutMainLoop();
	terminate();
	return 1;
}

void display(void)
{
	//Clear screen
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Set view and zoom
	glm::vec3 camPos = glm::vec3(cam->posx, cam->posy, cam->posz);
	proj = glm::perspective(45.0f, (float)width / height, 0.01f, 2000.0f);
	view = cam->GetOrientation() * glm::translate(camPos);
	glm::mat4 proj_view = proj * view;
	glViewport(0, 0, width, height);

	glUseProgram(basicShader.GetProgram());
	glUniformMatrix4fv(basicShader.GetVP(), 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glUseProgram(gridShader.GetProgram());
	glUniformMatrix4fv(gridShader.GetVP(), 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);

	glUseProgram(basicShader.GetProgram());
	Quaternion rotationObj = Quaternion();

	glm::mat4 model_mat;

	glPointSize(5);
	glBindVertexArray(vaoPoint);
		
	model_mat = rotationObj.QuaternionToMatrix();
	glUniformMatrix4fv(basicShader.GetM(), 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	std::vector<Point> controlPoints = patchMng.getControlPoints();
	glDrawArrays(GL_POINTS, 0, controlPoints.size());

	glBindVertexArray(0);

	//glBindVertexArray(vaoLine);
	//if (control_points.size() >= 2)
	//{
	//	lineVector.push_back(Point(control_points[0].x, control_points[0].y, control_points[0].z));
	//	lineVector.push_back(Point(control_points[1].x, control_points[1].y, control_points[1].z));
	//	model_mat = rotationObj.QuaternionToMatrix();
	//	majBuffer(vertexBufferLine, lineVector);
	//	glUniformMatrix4fv(basicShader.GetM(), 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	//	glDrawArrays(GL_LINES, 0, lineVector.size());
	//}
	//glBindVertexArray(0);

	glUseProgram(gridShader.GetProgram());
	glDrawArrays(GL_POINTS, 0, 1);

	TwDraw();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);

	proj = glm::perspective(45.0f, (float)width / height, 0.01f, 2000.0f);
}

void idle()
{
	glutPostRedisplay();
}

/** GESTION DEPLACEMENT CAMERA **/
void computePos(int unused)
{
	cam->updatePos();
	glutTimerFunc(10, computePos, 0);
}

#pragma region event_callback
void keyboardDown(unsigned char key, int x, int y)
{
	
	if (key == '\t' && decrease)
	{
		index -= 1;

		std::vector<Color> colors = patchMng.getColors();
		if (index < 0)
			index = colors.size() - 1;

		if (index == colors.size() - 1)
		{
			colors[0].r = 0.0f; colors[0].g = 0.0f; colors[0].b = 1.0f;
		}
		else if (index < colors.size())
		{
			colors[index + 1].r = 0.0f; colors[index + 1].g = 0.0f; colors[index + 1].b = 1.0f;
		}
		colors[index].r = 1.0f; colors[index].g = 1.0f; colors[index].b = 1.0f;
		majBuffer(vertexBufferColors, colors);
		indexStr = std::to_string(index);
	}
	else if (key == '\t')
	{
		index += 1;

		std::vector<Color> colors = patchMng.getColors();
		if (index == colors.size())
			index = 0;

		if (index > 0)
		{
			colors[index - 1].r = 0.0f; colors[index - 1].g = 0.0f; colors[index - 1].b = 1.0f;
		}
		colors[index].r = 1.0f; colors[index].g = 1.0f; colors[index].b = 1.0f;
		majBuffer(vertexBufferColors, colors);
		indexStr = std::to_string(index);
	}

	if (key == '+')
	{
		patchMng.getControlPoints()[index].y += 0.1;
		majBuffer(vertexBufferPoints, patchMng.getControlPoints());
	}
	if (key == '-')
	{
		patchMng.getControlPoints()[index].y -= 0.1;
		majBuffer(vertexBufferPoints, patchMng.getControlPoints());
	}

	if (key == ' ')
	{
		mode_ui = !mode_ui;
		if (mode_ui)
		{
			infos = "In mode UI";
			glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
			glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
			glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
			glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
		}
		else
		{
			infos = "In mode camera";
			glutMouseFunc(mouseButton);
			glutMotionFunc(mouseMove);
			glutPassiveMotionFunc(NULL);
			glutSpecialFunc(NULL);
		}
	}
	if (!mode_ui)
	{
		switch (key)
		{
			case 'z':
				cam->deltaForward = -1;
				break;
			case 's':
				cam->deltaForward = 1;
				break;
			case 'q':
				cam->deltaStrafe = -1;
				break;
			case 'd':
				cam->deltaStrafe = 1;
				break;
		}
	}
	TwDraw();
	glutPostRedisplay();
}

void keyboardSpecDown(int key, int x, int y)
{
	if (key == 112)
	{
		std::cout << "shift" << std::endl;
		decrease = true;
	}
}


void keyboardSpecUp(int key, int x, int y)
{
	if (key == 112)
	{
		std::cout << "shift up" << std::endl;
		decrease = false;
	}
}



void keyboardUp(unsigned char key, int xx, int yy)
{
	if (!mode_ui)
	{
		switch (key)
		{
		case 'z':
		case 's':
			cam->deltaForward = 0;
			break;
		case 'q':
		case 'd':
			cam->deltaStrafe = 0;
			break;
		}
	}
}

float pointToLineDistance(glm::vec3 point, glm::vec3 start, glm::vec3 end)
{
	glm::vec3 dir = glm::normalize(end - start);
	glm::vec3 startToPoint = point - start;

	float dotProduct = dot(glm::normalize(startToPoint), dir);
	float normcalc = norm(dir);
	std::cout << normcalc << std::endl;

	float projection = dotProduct / 5;
	std::cout << projection << std::endl;
	glm::vec3 nearPoint;
	if (projection < 0)
		nearPoint = start;
	else if (projection > 1)
		nearPoint = end;
	else
		nearPoint = end * projection + start * (1 - projection);// VectorAdd(VectorMultiply(vEnd, Projection), VectorMultiply(vStart, 1 - Projection));
	
	float distance = norm(nearPoint - point); // VectorLength(VectorSubtract(NearPoint, vPoint));
	return distance;
}

void mouseButton(int button, int state, int x, int y)
{
	if (!mode_ui)
	{
		// Gestion camera en fonction du clic souris
		if (button == GLUT_RIGHT_BUTTON)
		{
			// Relacher la camera
			if (state == GLUT_UP)
			{
				cam->releaseCam();
			}
			// Mise à jour origine du clic
			else
			{
				cam->grabCam(x, y);
			}
		}
	}
	glutPostRedisplay();
}

void terminate()
{
	TwTerminate();
}

void __stdcall exitCallbackTw(void* clientData)
{
	TwTerminate();
	glutLeaveMainLoop();
}

template<typename T>
void majBuffer(int vertexBuffer, std::vector<T> &vecteur)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vecteur.size(), vecteur.data(), GL_STATIC_DRAW);
}

void mouseMove(int x, int y)
{
	if (!mode_ui)
		cam->orienterCam(x, y);

	glutPostRedisplay();
}
