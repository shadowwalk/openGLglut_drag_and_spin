#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//sensitivity for rotation or drag
const float ROTSENSITIVITY = 3.0f;
const float DRAGSENSITIVITY = 50.0f;
const float BOX_SIZE = 3.0f;

//use to remeber the previous mouse position
int xOrigin = -1,yOrigin = -1;
//translate coordinate
float transX = 0.0f,transY = 0.0f,transZ = 0.0f;
bool shiftPress = false;

//4x4 rotation matrix
float rotMat[16];

void init_rotMat();
void update_rotMat( float angle, float x, float y, float z );

void changeSize(int w, int h) {
	
	//called when window size changes
	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	//reset matrix
	glLoadIdentity();
	//viewport entire screen and set the correct ratio
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);//restore to modelview
}

void drawBox() {

	glBegin(GL_QUADS);
	//Top face
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//Bottom face
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	
	//Left face
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//Right face
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	
	//Back face
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);

	glEnd();
}


void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);	
	//set camera to default position which camera never change here
	glLoadIdentity();
	gluLookAt(0.0f,0.0f,10.0f,
		  0.0f,0.0f,0.0f,
		  0.0f,1.0f,0.0f);
	
	//translate then rotate to make it rotate in local axis
	glTranslatef(transX,transY,transZ);
	glMultMatrixf(rotMat);

	drawBox();

        glutSwapBuffers();
} 

void init_rotMat()
{	
	//initialize the roatation matrix to identity matrix
	memset( rotMat, 0, sizeof(rotMat) );
	rotMat[0]=rotMat[5]=rotMat[10]=rotMat[15]=1;
	glutPostRedisplay();
}

void update_rotMat( float angle, float x, float y, float z )
{
	//update the rotation matrix accord the how mauch angle and 
	//which local axis(x/y/z) we want to spin avoid gimbal lock
	//this done by save current matrix then save the result of 
	//rotation matrix in rotMat[16] then restore the current matrix
	//that rotMat will multipy current matrix in later call of renderScene
	glPushMatrix();
	glLoadIdentity();
	glRotatef( angle, x,y,z );
	glMultMatrixf(rotMat);
	glGetFloatv( GL_MODELVIEW_MATRIX, rotMat );
	glPopMatrix();

	glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int xx, int yy) { 	

	switch( key )
	{
		case 27://Esc to exit
			exit(0);			
			break;
		case 'q'://reset the cube
			transX = 0.0f;
			transY = 0.0f;
			transZ = 0.0f;
			init_rotMat();
			break;
	}
} 

void processSpecialKeys(int key, int xx, int yy) { 	
	switch( key )
		{	
			//rotate in Z axis using left/right arrow key
			case GLUT_KEY_RIGHT:
				update_rotMat( -5/ROTSENSITIVITY, 0,0,1 );
				break;
			case GLUT_KEY_LEFT:
				update_rotMat( 5/ROTSENSITIVITY, 0,0,1 );
				break;
			//pull or push the cube using up/down arrow key
			case GLUT_KEY_UP:
				transZ += 1/DRAGSENSITIVITY;
				break;
			case GLUT_KEY_DOWN:
				transZ -= 1/DRAGSENSITIVITY;
				break;
	}
}

void mouseMove(int x, int y) { 	
	//do that only if previous mouse state is valid(left button have been clicked)
	 if (xOrigin >= 0 && yOrigin >= 0) {
		 //check whether shift is pressed
		 //need to check that use global variable 
		 //since can't use glutGetModifiers() here
		if(shiftPress){
			//if pressed, update the tranlate parameter
			transX += (float)(x-xOrigin)/DRAGSENSITIVITY;
			transY += (float)(yOrigin-y)/DRAGSENSITIVITY;
		}
		else{
			//if not update the rotation matrix
			//and only if mouse move in that direction
			if (y!=yOrigin){
				update_rotMat( (float)(y-yOrigin)/ROTSENSITIVITY, 1,0,0 );
			}
			if (x!=xOrigin){
				update_rotMat( (float)(x-xOrigin)/ROTSENSITIVITY, 0,1,0 );
			}
		}
		//store current mouse position for next move
		xOrigin = x;
		yOrigin = y;
	}
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
	//the purpose of this function purpose is just 
	//record the mouse position and manage shift key state
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			//restore to invalid state when mouse up
			xOrigin = -1;
			yOrigin = -1;
			shiftPress = false;
		}
		else  {
			//record current mouse/shift state
			xOrigin = x;
			yOrigin = y;
			if(GLUT_ACTIVE_SHIFT == glutGetModifiers())
				shiftPress = true;
		}
	}
}

int main(int argc, char **argv) {
	//initial glut window and enable some feature
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow(" GLUT & GL---Free Spin Cube");
		
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	//initialize rotation matrix
	init_rotMat();

	glutMainLoop();

	return 1;
}