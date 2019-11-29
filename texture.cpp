#define _CRT_SECURE_NO_WARNINGS
#include "glSetup.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
using namespace std;

// Camera configuation
vec3 eye(3.5, 4, 3.5);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);

//Light
vec4 light(3.5, 4, 3.5, 0);

// Global coordinate frame
float AXIS_LENGTH = 3;
float AXIS_LINE_WIDTH = 2;

//Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Cureent frame
int frame = 0;

GLuint texID;

int selection = 1;

// position p
vec4 p[36][18];

//normal vector & mid position vector
vec3 nor[36][18];

// rotate matrix
glm::mat4 axisZ = glm::mat4(1.0);
glm::mat4 axisY = glm::mat4(1.0);

// translate matrix
glm::mat4 transOrigin = glm::mat4(1.0);
glm::mat4 transPivot = glm::mat4(1.0);

// sweep value
int ySweep = 36;
int zSweep = 18;

void LoadTexture()
{
	unsigned char* data;
	FILE* file;
	char filename[100];

	if (selection == 1)
		strcpy(filename, "marble.raw");
	else if (selection == 2)
		strcpy(filename, "wood.raw");
	else if (selection == 3)
		strcpy(filename, "check.raw");
	
	file = fopen(filename, "rb");

	if (file == NULL) return;
	data = (unsigned char*)malloc(512 * 512 * 3);
	fread(data, 512 * 512 * 3, 1, file);
	fclose(file);

	// target, mipmap lavel, texture format, hor, ver, border, dataformat, type, real data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	free(data);
}

void setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

void setFrame()
{
	int ys, zs; //temp ySweep, zSweep for last sweep
	vec3 v; // midPoint to eye;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// distinguish full sweep or not
	if (ySweep == 36)
		ys = ySweep - 1;
	else
		ys = ySweep;
	if (zSweep == 18)
		zs = zSweep - 1;
	else
		zs = zSweep;

	glBindTexture(GL_TEXTURE_2D, texID);

	for (int i = 0; i < ys; i++)
	{
		for (int j = 0; j < zs; j++)
		{
			glBegin(GL_QUADS); //basic draw
			{
				glNormal3fv(value_ptr(nor[i][j]));
				glTexCoord2fv(value_ptr(vec2(i / 36.0, j / 18.0)));
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
				glTexCoord2fv(value_ptr(vec2(i / 36.0, (j + 1) / 18.0)));
				glVertex3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
				glTexCoord2fv(value_ptr(vec2((i + 1) / 36.0, (j + 1) / 18.0)));
				glVertex3f(p[i + 1][j + 1].x, p[i + 1][j + 1].y, p[i + 1][j + 1].z);
				glTexCoord2fv(value_ptr(vec2((i + 1) / 36.0, j / 18.0)));
				glVertex3f(p[i + 1][j].x, p[i + 1][j].y, p[i + 1][j].z);
			}
			glEnd();
			if (ySweep == 36)
			{
				glBegin(GL_QUADS); //i 0 to 35 connect in j 0 to 16
				{
					glNormal3fv(value_ptr(nor[35][j]));
					glTexCoord2fv(value_ptr(vec2(35.0 / 36.0, j / 18.0)));
					glVertex3f(p[35][j].x, p[35][j].y, p[35][j].z);
					glTexCoord2fv(value_ptr(vec2(35.0 / 36.0, (j + 1) / 18.0)));
					glVertex3f(p[35][j + 1].x, p[35][j + 1].y, p[35][j + 1].z);
					glTexCoord2fv(value_ptr(vec2(1, (j + 1) / 18.0)));
					glVertex3f(p[0][j + 1].x, p[0][j + 1].y, p[0][j + 1].z);
					glTexCoord2fv(value_ptr(vec2(1, j / 18.0)));
					glVertex3f(p[0][j].x, p[0][j].y, p[0][j].z);
				}
				glEnd();
			}
		}
		if (zSweep == 18)
		{
			glBegin(GL_QUADS); //j 0 to 17 connect in i 0 to 34
			{
				glNormal3fv(value_ptr(nor[i][17]));
				glTexCoord2fv(value_ptr(vec2(i / 36.0, 1)));
				glVertex3f(p[i][0].x, p[i][0].y, p[i][0].z);
				glTexCoord2fv(value_ptr(vec2(i / 36.0, 17.0 / 18.0)));
				glVertex3f(p[i][17].x, p[i][17].y, p[i][17].z);
				glTexCoord2fv(value_ptr(vec2((i + 1) / 36.0, 17.0 / 18.0)));
				glVertex3f(p[i + 1][17].x, p[i + 1][17].y, p[i + 1][17].z);
				glTexCoord2fv(value_ptr(vec2((i + 1) / 36.0, 1)));
				glVertex3f(p[i + 1][0].x, p[i + 1][0].y, p[i + 1][0].z);
			}
			glEnd();
		}
	}

	if (ySweep == 36 && zSweep == 18)
	{
		glBegin(GL_QUADS); //j 0 to 17 and i 0 to 35 draw
		{
			glNormal3fv(value_ptr(nor[35][17]));
			glTexCoord2fv(value_ptr(vec2(1, 1)));
			glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);
			glTexCoord2fv(value_ptr(vec2(1, 17.0 / 18.0)));
			glVertex3f(p[0][17].x, p[0][17].y, p[0][17].z);
			glTexCoord2fv(value_ptr(vec2(35.0 / 36.0, 17.0 / 18.0)));
			glVertex3f(p[35][17].x, p[35][17].y, p[35][17].z);
			glTexCoord2fv(value_ptr(vec2(35.0 / 36.0, 1)));
			glVertex3f(p[35][0].x, p[35][0].y, p[35][0].z);
		}
		glEnd();
	}
}

void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//view eye to zero
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	// Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Lighting
	setupLight();

	setFrame();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			// Example selection
		case GLFW_KEY_1: selection = 1; LoadTexture(); break;
		case GLFW_KEY_2: selection = 2; LoadTexture(); break;
		case GLFW_KEY_3: selection = 3; LoadTexture(); break;

		}
	}
}

void setDot()
{
	p[0][0] = vec4(1.4, 1, 0, 1.0f); //start dot
	transOrigin = glm::translate(transOrigin, vec3(-1.0, -1.0, 0));
	transPivot = glm::translate(transPivot, vec3(1.0, 1.0, 0));
	for (int j = 0; j < 17; j++)
	{
		axisZ = glm::rotate(axisZ, glm::radians(20.0f), vec3(0, 0, 1.0));
		p[0][j + 1] = transPivot * axisZ * transOrigin * p[0][0];
	}
	for (int i = 0; i < 35; i++)
	{
		axisY = glm::rotate(axisY, glm::radians(10.0f), vec3(0, 1, 0));
		for (int j = 0; j < 18; j++)
		{
			p[i + 1][j] = axisY * p[0][j];
		}
	}
}

void setNormalVector()
{
	int ys, zs; //temp ySweep, zSweep for last sweep
	vec3 v1, v2, c;
	double normalize;

	// distinguish full sweep or not
	if (ySweep == 36)
		ys = ySweep - 1;
	else
		ys = ySweep;
	if (zSweep == 18)
		zs = zSweep - 1;
	else
		zs = zSweep;

	for (int i = 0; i < ys; i++)
	{
		for (int j = 0; j < zs; j++)
		{
			//cross
			v1 = p[i + 1][j] - p[i][j];
			v2 = p[i][j + 1] - p[i][j];
			c = glm::cross(v1, v2);

			//normalize
			normalize = sqrt(c.x * c.x + c.y * c.y + c.z * c.z) * 8.0; // for visible
			c = vec3(c.x / normalize, c.y / normalize, c.z / normalize);
			nor[i][j] = c;

			if (ySweep == 36) //i 0 to 35 connect in j 0 to 16
			{
				//cross
				v1 = p[0][j] - p[35][j];
				v2 = p[35][j + 1] - p[35][j];
				c = glm::cross(v1, v2);

				//normalize
				normalize = sqrt(c.x * c.x + c.y * c.y + c.z * c.z) * 8.0; // for visible
				c = vec3(c.x / normalize, c.y / normalize, c.z / normalize);
				nor[35][j] = c;

			}
		}
		if (zSweep == 18) //j 0 to 17 connect in i 0 to 34
		{
			//cross
			v1 = p[i + 1][17] - p[i][17];
			v2 = p[i][0] - p[i][17];
			c = glm::cross(v1, v2);

			//normalize
			normalize = sqrt(c.x * c.x + c.y * c.y + c.z * c.z) * 8.0; // for visible
			c = vec3(c.x / normalize, c.y / normalize, c.z / normalize);
			nor[i][17] = c;

		}
	}

	if (ySweep == 36 && zSweep == 18) //j 0 to 17 and i 0 to 35 draw
	{
		//cross
		v1 = p[0][17] - p[35][17];
		v2 = p[35][0] - p[35][17];
		c = glm::cross(v1, v2);

		//normalize
		normalize = sqrt(c.x * c.x + c.y * c.y + c.z * c.z) * 8.0; // for visible
		c = vec3(c.x / normalize, c.y / normalize, c.z / normalize);
		nor[35][17] = c;

	}
}

void init()
{
	// Texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(2, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	LoadTexture();

	// dot set
	setDot();

	// vector set
	setNormalVector();
}

int main(int argc, char* argv[])
{
	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Initialization - Main loop - Finalization
	init();

	// ---Main loop---
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();			// Events
		render(window);				// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
	}

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
