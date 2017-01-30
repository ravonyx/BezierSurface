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
#include <time.h>
#pragma endregion

std::vector<Point> lines;

int current_index = 0;
int width = 1500;
int height = 800;

Quaternion rotation;




GLint basicProgram, gridProgram, tessProgram;

GLuint vaoPoint, vaoCasteljauPoints, vertexBufferPoints, vertexBufferColors, vertexBufferCasteljauPoints;

/*var shaders*/
int programBasicID;
GLuint attr_color;

Camera *cam;
bool mode_ui, decrease;
int index;
std::string indexStr;
std::string infos;

PatchManager patchMng;

#pragma region header_function
static  void __stdcall exitCallbackTw(void* clientData);
static  void __stdcall randomizeCallbackTw(void* clientData);
void display(void);
void computePos(int unused);

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

void initialize();
void loadShaders();

struct
{
	struct
	{
		int     model_matrix;
		int     projview_matrix;
		int     position;
		int     color;
	} basic;
	struct
	{
		int     projview_matrix;
	} grid;
} uniforms;

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

	srand(time(0));

	//Callback functions
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, computePos, 0);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(NULL);
	glutSpecialFunc(keyboardSpecDown);
	glutSpecialUpFunc(keyboardSpecUp);

	loadShaders();
	initialize();

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
	TwAddButton(bar, "Randomize", &randomizeCallbackTw, nullptr, "");

	TwAddSeparator(bar, "program", "");
	TwAddButton(bar, "Exit", &exitCallbackTw, nullptr, "");

	glutMainLoop();
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
	glm::mat4 proj = glm::perspective(45.0f, (float)width / height, 0.01f, 1000.0f);
	glm::mat4 view = cam->GetOrientation() * glm::translate(camPos);
	glm::mat4 proj_view = proj * view;
	glViewport(0, 0, width, height);

	glUseProgram(basicProgram);
	glUniformMatrix4fv(uniforms.basic.projview_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glUseProgram(gridProgram);
	glUniformMatrix4fv(uniforms.grid.projview_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);

	glUseProgram(basicProgram);
	Quaternion rotationObj = Quaternion();

	glm::mat4 model_mat;

	glPointSize(5);
	glBindVertexArray(vaoPoint);
		
	glUniformMatrix4fv(uniforms.basic.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	glDrawArrays(GL_POINTS, 0, patchMng.getControlPoints().size());

	glBindVertexArray(0);

	glBindVertexArray(vaoCasteljauPoints);
	glUniformMatrix4fv(uniforms.basic.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	glDrawArrays(GL_POINTS, 0, patchMng.getCasteljauPoints().size());
	glBindVertexArray(0);

	glUseProgram(gridProgram);
	glDrawArrays(GL_POINTS, 0, 1);

	TwDraw();
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

void computePos(int unused)
{
	cam->updatePos();
	glutTimerFunc(10, computePos, 0);
}

void initialize()
{
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
	glEnableVertexAttribArray(uniforms.basic.position);
	glVertexAttribPointer(uniforms.basic.position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &vertexBufferColors);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * colors.size(), colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uniforms.basic.color);
	glVertexAttribPointer(uniforms.basic.color, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

	colors[0].r = 1.0f; colors[0].g = 1.0f; colors[0].b = 1.0f;
	majBuffer(vertexBufferColors, colors);
	index = 0;
	indexStr = std::to_string(index);

	/*VAO Line*/
	glGenVertexArrays(1, &vaoCasteljauPoints);
	glBindVertexArray(vaoCasteljauPoints);

	std::vector<Point> casteljauPoints = patchMng.getCasteljauPoints();
	glGenBuffers(1, &vertexBufferCasteljauPoints);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCasteljauPoints);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * casteljauPoints.size(), casteljauPoints.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uniforms.basic.position);
	glVertexAttribPointer(uniforms.basic.position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

void loadShaders()
{
	EsgiShader basicShader, gridShader, tessShader;

	printf("Load Fragment basic shader\n");
	basicShader.LoadFragmentShader("shaders/basic.frag");
	printf("Load Vertex basic shader\n");
	basicShader.LoadVertexShader("shaders/basic.vert");
	basicShader.Create();
	printf("Load Fragment grid shader\n");
	gridShader.LoadFragmentShader("shaders/grid.frag");
	printf("Load Vertex grid shader\n");
	gridShader.LoadVertexShader("shaders/grid.vert");
	printf("Load Geom grid shader\n");
	gridShader.LoadGeometryShader("shaders/grid.geom");
	gridShader.Create();

	basicProgram = basicShader.GetProgram();
	gridProgram = gridShader.GetProgram();
	tessProgram = tessShader.GetProgram();

	uniforms.grid.projview_matrix = glGetUniformLocation(gridProgram, "VP");

	uniforms.basic.projview_matrix = glGetUniformLocation(basicProgram, "VP");
	uniforms.basic.model_matrix = glGetUniformLocation(basicProgram, "M");
	uniforms.basic.position = glGetAttribLocation(basicProgram, "a_position");
	uniforms.basic.color = glGetAttribLocation(basicProgram, "a_color");
}


template<typename T>
void majBuffer(int vertexBuffer, std::vector<T> &vecteur)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vecteur.size(), vecteur.data(), GL_STATIC_DRAW);
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

	if (key == 'e')
	{
		patchMng.getBezierCurveOnRow(patchMng.getControlPoints(), 20, std::vector<int>());
		majBuffer(vertexBufferCasteljauPoints, patchMng.getCasteljauPoints());
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

void mouseMove(int x, int y)
{
	if (!mode_ui)
		cam->orienterCam(x, y);

	glutPostRedisplay();
}

void __stdcall exitCallbackTw(void* clientData)
{
	TwTerminate();
	glutLeaveMainLoop();
}

void __stdcall randomizeCallbackTw(void* clientData)
{
	patchMng.randomizeControlPoints();
	majBuffer(vertexBufferPoints, patchMng.getControlPoints());
	glutPostRedisplay();
}
#pragma endregion

