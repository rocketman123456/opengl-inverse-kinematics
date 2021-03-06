#include "display.h"

#include "limb.h"
#include "arm.h"


#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif


#include <math.h>
#include <iomanip>
#include <iostream>
#include <strstream>
#include <GL/glut.h>
#include "Eigen/Dense"


using namespace Eigen;
using namespace std;


Arm arm; 
Vector2f targetPoint = Vector2f::Zero();  
//todo: target point should be ditermined by the position of the mouse


// window size
int32_t width  = 600;
int32_t height = 400;

// mouse position, normalized to [0,1]
double x_mouse = 0.5;
double y_mouse = 0.5;

// frustum
float nearPlane =  1.0;
float farPlane  = 1000.0;

// viewing angle
double fovy = 40.0;


double alpha = 0; // set by idle function.
double beta = 0;  // set by mouse X.
double mousedist = - (farPlane - nearPlane) / 2;  // set by mouse Y.


Display::Display()
{

}

Display::~Display()
{

}

void glut_mouse(int32_t state, int32_t _x, int32_t _y)
{

}


void glut_update()
{
	glutPostRedisplay(); // mark current window for redisplaying
}


void glut_keyboard(uint8_t _key, int32_t _x, int32_t _y)
{
	switch (_key) {
	case 27:
		exit(0);
	}
}


void glut_motion(int32_t _x, int32_t _y)
{
	// normalizing mouse coordinates.
	x_mouse = double(_x) / double(width);
	y_mouse = 1 - double(_y) / double(height);

	glutPostRedisplay();  // mark current window for redisplaying
}


void glut_timer(int32_t i) // 10ms call
{
	// per pre-defined-delay-interval progressively
	// resolve the end effector target position
	arm.update();
	if (arm.isTargetResolved()){ 
		targetPoint = arm.getPointWithinRange(); arm.moveToPoint(targetPoint); 
	}

	glutTimerFunc(10, glut_timer, i);
	glutPostRedisplay(); // mark current window for redisplaying
}


void glut_resize(int32_t _width, int32_t _height) {
	width = _width;
	height = _height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, GLfloat(width) / GLfloat(height), nearPlane, farPlane);
}


void glut_display()
{
	glEnable(GL_LIGHTING);
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // mouse translation (y)
    glTranslatef(0, 0, -200);

    // idle fucntion rotation
    glRotatef(90, 0, 0, 1);

    // mouse rotation (x)
    beta = 180.0 * x_mouse;
    glRotatef(beta, 0, 1, 0);
    alpha = 180.0 * y_mouse;
    glRotatef(beta, 1, 0, 0);

    // visual update of the IK chain:
	arm.draw();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); 
    glTranslatef(0, -targetPoint(1), targetPoint(0));
	glutWireSphere(3.0f, 8, 8);  // end-effector target sphere
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}


void Display::init(int argc, char** argv, int limbs)
{
	srand(time(NULL));
	
	// specification for construction of the arm 
    for(int i = 1; i <= limbs; i++){
		glColor color = { 0.5f, 0.5f, 0.5f, 0.5f };
        Limb *limb = new Limb(color);
        limb-> angle = 3.14f/4;
        limb-> length = 20;
        ::arm.addLimb(limb);
    }

    // GLUT initialization.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  	glutInitWindowSize(width, height);
	glutInitWindowPosition(64, 64);
	glutCreateWindow(argv[0]);
	glutSetWindowTitle("inverse-kinematics");

	// Register call backs.
    glutDisplayFunc(glut_display);
    glutReshapeFunc(glut_resize);
    glutKeyboardFunc(glut_keyboard);
	glutMotionFunc(glut_motion);
	glutIdleFunc(glut_update);
    glutTimerFunc(60, glut_timer, 0);

	// OpenGL initialization
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_AMBIENT_AND_DIFFUSE, global_ambient);

    // Enter GLUT loop.
    glutMainLoop();
}