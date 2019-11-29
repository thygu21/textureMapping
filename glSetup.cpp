#include "glSetup.h"

#include <string.h>
#include <iostream>
using namespace std;

bool perspectiveView = true;

float screenScale = 0.5;
int screenW = 0, screenH = 0;
int windowW, windowH;
float aspect;
float dpiScaling = 0;

int vsync = 1;

float fovy = 46.4;

float nearDist = 1.0;
float farDist = 20.0;

void errorCallback(int error, const char* description)
{
	cerr << "####" << description << endl;
}

void setupProjectionMatrix()
{
	if (perspectiveView)
		gluPerspective(fovy, aspect, nearDist, farDist);
	else
		glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -nearDist, farDist);
}

void reshape(GLFWwindow* window, int w, int h)
{
	aspect = (float)w / h;

	windowW = w;
	windowH = h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	setupProjectionMatrix();

	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "reshape(" << w << ", " << h << ")";
	cerr << " with screen " << screenW << " x " << screenH << endl;
}

GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bgColor[4], bool modern)
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	if (modern)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_SAMPLES, 4);
	}
	else
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW, monitorH;
	glfwGetMonitorPhysicalSize(monitor, &monitorW, &monitorH);
	cerr << "Status: Monitor " << monitorW << "mm x " << monitorH << "mm" << endl;

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	if (screenW == 0)
		screenW = videoMode->width * screenScale;
	if (screenH == 0)
		screenH = videoMode->height * screenScale;

	GLFWwindow* window = glfwCreateWindow(screenW, screenH, argv[0], NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		cerr << "Failed in glfwCreateWindow()" << endl;
		return NULL;
	}

	glfwMakeContextCurrent(window);

	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glFlush();
	glfwSwapBuffers(window);

	glfwGetWindowSize(window, &windowW, &windowH);
	cerr << "Status: Framebuffer " << windowW << " x " << windowH << endl;

	if (dpiScaling == 0)
		dpiScaling = float(windowW) / screenW;

	glfwSetFramebufferSizeCallback(window, reshape);

	cout << "Status: Renderer " << glGetString(GL_RENDERER) << endl;
	cout << "Status: Ventor " << glGetString(GL_VENDOR) << endl;
	cout << "Status: OpenGL " << glGetString(GL_VERSION) << endl;

	char glslVersion[256];
	strcpy_s(glslVersion, (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	cout << "Status: GLSL " << glslVersion << endl;

	glfwSwapInterval(vsync);
	return window;
}

void drawAxes(float l, float w)
{
	glLineWidth(w);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(5, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 5, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 5);
	glEnd();
}