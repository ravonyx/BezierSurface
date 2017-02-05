#include <gl\glew.h>

#pragma region my_include
#include "Math.h"
#include "Bezier.h"
#include "Camera.h"
#include "EsgiShader.h"
#include "PatchManager.h"
#include "Tools.h"
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
#include <glm\gtc\type_ptr.hpp>
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

GLuint mainTexture;
Quaternion rotation;
glm::vec3 pos_light_1 = glm::vec3(0.5f, 1.0f, 0.0f);
glm::vec3 pos_light_2 = glm::vec3(-0.5f, 1.0f, 0.0f);
int display_normal = 0, display_texture = 0, display_wireframe = 0;

GLuint lightsBuffer;
GLint basicProgram, gridProgram, patchProgram;
GLuint vaoPoint, vaoCasteljauPoints, vaoPatch;
GLuint vertexBufferPoints, vertexBufferColors, vertexBufferCasteljauPoints, vertexBufferPatch;

Camera *cam;
bool mode_ui, decrease;
int index;
int indexTexture;
std::string indexStr;
std::string infos;

PatchManager patchMng;

#pragma region header_function
static  void __stdcall exitCallbackTw(void* clientData);
static  void __stdcall randomizeCallbackTw(void* clientData);
static  void __stdcall changeTextureCallbackTw(void* clientData);
static  void __stdcall resetRotation(void* clientData);
void TW_CALL SetWireframeCB(const void *value, void *clientData);
void TW_CALL GetWireframeCB(void *value, void *clientData);
void TW_CALL SetNormalCB(const void *value, void *clientData);
void TW_CALL GetNormalCB(void *value, void *clientData);
void TW_CALL SetTextureCB(const void *value, void *clientData);
void TW_CALL GetTextureCB(void *value, void *clientData);

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
		int     model_matrix;
		int     projview_matrix;
		int     view_matrix;
		int     display_normal;
		int     display_texture;
		int		pos_lights;
		int		cam_pos;
	} patch;
	struct
	{
		int     projview_matrix;
	} grid;
} uniforms;

glm::vec3     posLights[2];

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
	TwDefine(" BezierSurface size='300 500' color='86 101 115' valueswidth=fit ");
	float refresh = 0.1f;
	TwSetParam(bar, NULL, "refresh", TW_PARAM_FLOAT, 1, &refresh);
	TwAddVarRO(bar, "Output", TW_TYPE_STDSTRING, &infos," label='Infos' ");
	TwAddVarRO(bar, "Index", TW_TYPE_STDSTRING, &indexStr, " label='Index' help='Change index of the point' ");

	TwAddVarCB(bar, "Wireframe", TW_TYPE_BOOL32, SetWireframeCB, GetWireframeCB, NULL, " label='Wireframe' help='Display in wireframe' ");
	TwAddVarCB(bar, "Normal", TW_TYPE_BOOL32, SetNormalCB, GetNormalCB, NULL, " label='Normal' help='Display normals' ");
	TwAddVarCB(bar, "Texture", TW_TYPE_BOOL32, SetTextureCB, GetTextureCB, NULL, " label='Texture' help='Display texture' ");

	TwAddSeparator(bar, "settings object", "");
	TwAddVarRW(bar, "LightPos1", TW_TYPE_DIR3F, &pos_light_1, " label='Light pos 1' opened=false help='Change the light pos number 1.' ");
	TwAddVarRW(bar, "LightPos2", TW_TYPE_DIR3F, &pos_light_2, " label='Light pos 1' opened=false help='Change the light pos number 2.' ");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation, " label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddButton(bar, "Reset rotation", &resetRotation, nullptr, "");
	TwAddButton(bar, "Change texture", &changeTextureCallbackTw, nullptr, "");
	TwAddButton(bar, "Randomize points", &randomizeCallbackTw, nullptr, "");

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
	glViewport(0, 0, width, height);
	glPointSize(10);

	//Parameters
	glm::vec3 cam_pos = glm::vec3(cam->posx, cam->posy, cam->posz);
	glm::mat4 proj = glm::perspective(45.0f, (float)width / height, 0.01f, 1000.0f);
	glm::mat4 view = cam->GetOrientation() * glm::translate(cam_pos);
	glm::mat4 proj_view = proj * view;
	glm::mat4 model_mat;

	std::vector<Point> controlPoints = patchMng.getControlPoints();

	if (display_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Draw patch
	glUseProgram(patchProgram);
	model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * rotation.QuaternionToMatrix();
	glUniformMatrix4fv(uniforms.patch.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);
	glUniformMatrix4fv(uniforms.patch.projview_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glUniformMatrix4fv(uniforms.patch.view_matrix, 1, GL_FALSE, (GLfloat*)&view[0][0]);

	if(display_normal)
		glUniform1i(uniforms.patch.display_normal, 1);
	else
		glUniform1i(uniforms.patch.display_normal, 0);
	if (display_texture)
		glUniform1i(uniforms.patch.display_texture, 1);
	else
		glUniform1i(uniforms.patch.display_texture, 0);

	posLights[0] = glm::vec3(pos_light_1.x, pos_light_1.y, pos_light_1.z);
	posLights[1] = glm::vec3(pos_light_2.x, pos_light_2.y, pos_light_2.z);
	glUniform3fv(uniforms.patch.pos_lights, 2, glm::value_ptr(posLights[0]));
	glUniform3fv(uniforms.patch.cam_pos, 1, glm::value_ptr(cam_pos));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTexture);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPatch);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * controlPoints.size(), controlPoints.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(vaoPatch);
	glPatchParameteri(GL_PATCH_VERTICES, controlPoints.size());
	glDrawArrays(GL_PATCHES, 0, controlPoints.size());
	glBindVertexArray(0);

	//Draw Point and curves
	glUseProgram(basicProgram);
	glUniformMatrix4fv(uniforms.basic.projview_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
	glUniformMatrix4fv(uniforms.basic.model_matrix, 1, GL_FALSE, (GLfloat*)&model_mat[0][0]);

	glBindVertexArray(vaoPoint);
	glDrawArrays(GL_POINTS, 0, patchMng.getControlPoints().size());
	glBindVertexArray(0);

	glBindVertexArray(vaoCasteljauPoints);
	glDrawArrays(GL_POINTS, 0, patchMng.getCasteljauPoints().size());
	glBindVertexArray(0);

	//Draw grid
	glUseProgram(gridProgram);
	glUniformMatrix4fv(uniforms.grid.projview_matrix, 1, GL_FALSE, (GLfloat*)&proj_view[0][0]);
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
	patchMng = PatchManager(4.0, 4.0);
	indexTexture = 0;
	mainTexture = CreateTexture("img/minecraft.jpg");

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

	/*VAO Patch*/
	glGenVertexArrays(1, &vaoPatch);
	glBindVertexArray(vaoPatch);

	glGenBuffers(1, &vertexBufferPatch);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPatch);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * controlPoints.size(), controlPoints.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	
}

void loadShaders()
{
	EsgiShader basicShader, gridShader, patchShader;

	printf("Load basic fs\n");
	basicShader.LoadFragmentShader("shaders/basic.frag");
	printf("Load basic vs\n");
	basicShader.LoadVertexShader("shaders/basic.vert");
	basicShader.Create();

	printf("Load grid fs\n");
	gridShader.LoadFragmentShader("shaders/grid.frag");
	printf("Load grid vs\n");
	gridShader.LoadVertexShader("shaders/grid.vert");
	printf("Load grid geom\n");
	gridShader.LoadGeometryShader("shaders/grid.geom");
	gridShader.Create();

	printf("Load patch fs\n");
	patchShader.LoadFragmentShader("shaders/patch.fs");
	printf("Load patch tcs\n");
	patchShader.LoadTessControlShader("shaders/patch.tcs");
	printf("Load patch tes\n");
	patchShader.LoadTessEvaluationShader("shaders/patch.tes");
	printf("Load patch vs\n");
	patchShader.LoadVertexShader("shaders/patch.vs");
	patchShader.Create();

	basicProgram = basicShader.GetProgram();
	gridProgram = gridShader.GetProgram();
	patchProgram = patchShader.GetProgram();

	uniforms.grid.projview_matrix = glGetUniformLocation(gridProgram, "projview_matrix");

	uniforms.basic.projview_matrix = glGetUniformLocation(basicProgram, "projview_matrix");
	uniforms.basic.model_matrix = glGetUniformLocation(basicProgram, "model_matrix");
	uniforms.basic.position = glGetAttribLocation(basicProgram, "a_position");
	uniforms.basic.color = glGetAttribLocation(basicProgram, "a_color");

	uniforms.patch.model_matrix = glGetUniformLocation(patchProgram, "model_matrix");
	uniforms.patch.view_matrix = glGetUniformLocation(patchProgram, "view_matrix");
	uniforms.patch.projview_matrix = glGetUniformLocation(patchProgram, "projview_matrix");
	uniforms.patch.display_normal = glGetUniformLocation(patchProgram, "display_normal");
	uniforms.patch.display_texture = glGetUniformLocation(patchProgram, "display_texture");
	uniforms.patch.pos_lights = glGetUniformLocation(patchProgram, "pos_lights");
	uniforms.patch.cam_pos = glGetUniformLocation(patchProgram, "cam_pos");
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

static  void __stdcall changeTextureCallbackTw(void* clientData)
{
	indexTexture++;
	if (indexTexture == 3)
		indexTexture = 0;
	if(indexTexture == 0)
		mainTexture = CreateTexture("img/minecraft.jpg");
	else if (indexTexture == 1)
		mainTexture = CreateTexture("img/checker.jpg");
	else if (indexTexture == 2)
		mainTexture = CreateTexture("img/pacman.png");
	glutPostRedisplay();

}

static  void __stdcall resetRotation(void* clientData)
{
	rotation = Quaternion();
	glutPostRedisplay();
}

void TW_CALL SetWireframeCB(const void *value, void *clientData)
{
	(void)clientData;
	display_wireframe = *(const int *)value;
}
void TW_CALL GetWireframeCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = display_wireframe;
}

void TW_CALL SetNormalCB(const void *value, void *clientData)
{
	(void)clientData;
	display_normal = *(const int *)value;
}
void TW_CALL GetNormalCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = display_normal;
}

void TW_CALL SetTextureCB(const void *value, void *clientData)
{
	(void)clientData;
	display_texture = *(const int *)value;
}
void TW_CALL GetTextureCB(void *value, void *clientData)
{
	(void)clientData;
	*(int *)value = display_texture;
}


#pragma endregion

