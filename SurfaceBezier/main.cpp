#include <gl\glew.h>

#pragma region my_include
#include "Math.h"
#include "Bezier.h"
#include "Camera.h"
#include "EsgiShader.h"
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

std::vector< std::vector<Point> > all_control_point;
std::vector< std::vector<Point> > all_result;
std::vector<Point> control_points;
std::vector<std::vector<int>> index_curve;
std::vector<std::vector<int>> index_extrusion;

int current_index = 0;
int width = 1500;
int height = 800;

float first_mousex, first_mousey;
float last_movex, last_movey;

int _selectMovePointI = 0, _selectMovePointJ = 0;

Quaternion rotation;

glm::mat4 proj;
glm::mat4 view;

EsgiShader basicShader, gridShader;

GLuint vaoCube, vertexBufferCube;
GLuint mvp_location, position_location, color_location;

/*var shaders*/
int programBasicID;
GLuint attr_color;

Camera *cam;
bool mode_ui;

std::string infos;
int nbCubes;

#pragma region header_function
static  void __stdcall exitCallbackTw(void* clientData);
void idle();
void display(void);
void computePos(int unused);
void terminate();

void mouseZoom(int button, int dir, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void reshape(int w, int h);
#pragma endregion

int main(int argc, char** argv)
{
	TwBar *bar;
	cam = new Camera();
	mode_ui = false;
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
	color_location = glGetUniformLocation(basicShader.GetProgram(), "fragmentColor");

	GLuint uniVP = glGetUniformLocation(gridShader.GetProgram(), "VP");
	gridShader.SetVP(uniVP);
	uniVP = glGetUniformLocation(basicShader.GetProgram(), "VP");
	basicShader.SetVP(uniVP);
	GLuint uniM = glGetUniformLocation(basicShader.GetProgram(), "M");
	basicShader.SetM(uniM);

	rotation = Quaternion();
	nbCubes = 3;

	float fragmentColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glProgramUniform4fv(basicShader.GetProgram(), color_location, 1, fragmentColor);

	static const GLfloat g_cube_buffer_data[] = {
		0.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  1.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  1.0f
	};
	
	static unsigned int g_cube_buffer_indices[] = {
		1, 7, 5,
		1, 3, 7,
		1, 4, 3,
		1, 2, 4,
		3, 8, 7,
		3, 4, 8,
		5, 7, 8,
		5, 8, 6,
		1, 5, 6,
		1, 6, 2,
		2, 6, 8,
		2, 8, 4
	};

	for (int i = 0; i < 12 * 3; i++)
	{
		g_cube_buffer_indices[i] -= 1;
	}
	
	/*VAO Points*/
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	glGenBuffers(1, &vertexBufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_buffer_data), g_cube_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 12 * sizeof(unsigned int), g_cube_buffer_indices, GL_STATIC_DRAW);
	
	glBindVertexArray(0);

	/** GESTION SOURIS **/
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(NULL);
	glutSpecialFunc(NULL);

	// Create a tweak bar
	bar = TwNewBar("BezierSurface");

	TwDefine(" BezierSurface size='200 300' color='86 101 115' valueswidth=fit ");
	TwAddVarRO(bar, "Output", TW_TYPE_STDSTRING, &infos," label='Infos' ");
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
	glBindVertexArray(vaoCube);
	for (int i = 0; i < control_points.size(); i++)
	{
		glm::vec3 position = glm::vec3(control_points[i].x, control_points[i].y, control_points[i].z);
		model_mat = glm::translate(position) * rotationObj.QuaternionToMatrix() *  glm::scale(glm::vec3(0.05, 0.05, 0.05));
		glUniformMatrix4fv(basicShader.GetM(), 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
		glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
	}
	glBindVertexArray(0);


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
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			GLdouble Bx, By, Bz;
			glm::vec4 viewport = glm::vec4(0, 0, width, height);

			
			glm::vec3 pointPosStart = glm::unProject(glm::vec3(x, viewport[3] - y, 0.0f), view, proj, viewport);
			glm::vec3 pointPosEnd = glm::unProject(glm::vec3(x, viewport[3] - y, 1.0f), view, proj, viewport);

			glm::vec3 dir = glm::normalize(pointPosEnd - pointPosStart);
			glm::vec3 pos = pointPosStart + dir * 2.0f;

			Bx = pos.x;
			By = pos.y;
			Bz = pos.z;

			std::cout << Bx << " " << By << " " << Bz << std::endl;
			control_points.push_back(Point(Bx, By, Bz));
		}

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

/*void mouseZoom(int button, int dir, int x, int y)
{
	g_scale = 0;
	if (dir > 0)
		g_scale += 0.05f;
	else
		g_scale -= 0.05f;
	if (g_scale < -1)
		g_scale = -1;
	else if (g_scale > 1)
		g_scale = 1;

	if ((g_display_manager.scale_control_points && _selectMovePointI != -1 && _selectMovePointJ != -1) && mode_ui)
	{
		int index_curve_current = -1;
		for (unsigned int i = 0; i < index_curve.size(); i++)
		{
			for (unsigned int j = 0; j < index_curve[i].size(); j++)
			{
				if (index_curve[i][j] == _selectMovePointI)
				{
					index_curve_current = i;
				}
			}
		}
		for (unsigned int i = 0; i < index_curve[index_curve_current].size(); i++)
		{
			for (size_t j = 0; j < all_control_point[index_curve[index_curve_current][i]].size(); ++j)
			{
				if (j != _selectMovePointJ || _selectMovePointI != index_curve[index_curve_current][i])
					all_control_point[index_curve[index_curve_current][i]][j] = all_control_point[index_curve[index_curve_current][i]][j] +
					(all_control_point[_selectMovePointI][_selectMovePointJ] - all_control_point[index_curve[index_curve_current][i]][j]) * g_scale;
			}
		}
	}

	
	glutPostRedisplay();
}*/

void mouseMove(int x, int y)
{
	if (!mode_ui)
		cam->orienterCam(x, y);

	glutPostRedisplay();
}

/*void validate_junction(Fl_Widget *w, void *data)
{
	int choice = choice_junction->value();
	int nb_curves = all_control_point.size();
	int nb_points = all_control_point[nb_curves - 1].size();
	float t = 2.0f;

	Point point, point_temp;
	Point last_point = all_control_point[nb_curves - 1][nb_points - 1];
	Point second_to_last = all_control_point[nb_curves - 1][nb_points - 2];
	Point third_to_last = all_control_point[nb_curves - 1][nb_points - 3];

	switch (choice) 
	{
		case 1:
			control_points.push_back(last_point);
			break;
		case 2:
			point = second_to_last * (1 - t) + last_point * t;
			control_points.push_back(last_point);
			control_points.push_back(point);
			break;
		case 3:
			point = second_to_last * (1 - t) + last_point * t;
			control_points.push_back(last_point);
			control_points.push_back(point);

			point_temp = third_to_last * (1 - t) + last_point * t;
			point = point_temp * (1 - t) + point * t;
			control_points.push_back(point);
			break;
	}
}*/

