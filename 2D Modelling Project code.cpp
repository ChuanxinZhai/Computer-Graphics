// File ID: CPT205 Assessment 1
// Title: Celebrating 15th anniversary of XJTLU
// Author: Chuanxin Zhai  1929631

#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>

GLint width = 1200;
GLint height = 800;

const double PI = acos(-1.0);

float t = 0.0;
float r = 0.0;
float s = 1.0;
GLfloat step = 2; // declare step

int time_interval = 16;


void FB();
void Grass();
void Balloon(float x, float y, int R, int G, int B);
void Sun();
void Background();
void Tree();

void OnTimer(int value) {
	step += 1;
	if (step >= 200)
		step -= 200;
	//when_in_mainloop();
	glutTimerFunc(time_interval, OnTimer, 1);
}

void mouse_input(int button, int state, int x, int y) {       // mouse interaction
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && step >= -15)
		step -= 5; // decrement step
	else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON && step <= 15)
		step += 5; // increment step
}

void Display(void) {
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();


	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Background();
	Grass();
	Sun();
	Tree();

	glPushMatrix();
	glTranslatef(0, step, 0);
	Balloon(120.0f, 400.0f, 38, 70, 83);
	Balloon(300.0f, 750.0f, 42, 157, 143);
	Balloon(672.0f, 717.0f, 233, 196, 106);
	Balloon(738.0f, 560.0f, 244, 162, 97);
	Balloon(850.0f, 680.0f, 231, 111, 81);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(t, 0, 0);
	glScalef(s, s, 0);
	glRotatef(r, 531, 374, 1);
	FB();
	glPopMatrix();


	glutSwapBuffers();
}

void when_in_mainloop() {
	glutPostRedisplay(); // force OpenGL to redraw the current window
}

void keyboard_input(unsigned char key, int x, int y) { // keyboard interaction
	if (key == 'q' || key == 'Q') // Exit 
		exit(0);
	else if (key == 'r' || key == 'R')  //right movement
		t += 10.0f;
	else if (key == 'l' || key == 'L') // left movement
		t -= 10.0f;
	else if (key == 'f' || key == 'F')  //Front overturn
		r--;
	else if (key == 'b' || key == 'B') //Back overturn
		r++;
	else if (key == 'e' || key == 'E') //Enlarge
		s += 0.05f;
	else if (key == 's' || key == 'S') //Shrink
		s -= 0.05f;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Celebrating 15th anniversary of XJTLU");

	gluOrtho2D(0, width, 0, height);
	glViewport(0, 0, width, height);

	glutDisplayFunc(Display);
	glutIdleFunc(when_in_mainloop);
	glutKeyboardFunc(keyboard_input);
	glutTimerFunc(time_interval, OnTimer, 1);

	glutMainLoop();
}


void FB()
{
	//draw the Functional Building
	glLineWidth(1.0);
	glColor3ub(152, 193, 217);
	glBegin(GL_POLYGON);
	glVertex2f(600, 200);
	glVertex2f(900, 200);
	glVertex2f(900, 500);
	glVertex2f(850, 500);
	glVertex2f(800, 450);
	glVertex2f(700, 450);
	glVertex2f(700, 555);
	glVertex2f(500, 555);
	glVertex2f(500, 450);
	glVertex2f(400, 450);
	glVertex2f(350, 500);
	glVertex2f(300, 500);
	glVertex2f(300, 200);
	glVertex2f(600, 200);
	glEnd();

	//draw windows
	glColor3ub(224, 251, 252);
	float x1 = 305;
	float y1 = 395;
	for (int i = 0; i <= 9; i++) {
		glRectf(x1, y1, x1 + 50, y1 + 50);
		x1 += 60;
	}
	x1 = 305;
	y1 = 335;
	for (int i = 0; i <= 9; i++) {
		glRectf(x1, y1, x1 + 50, y1 + 50);
		x1 += 60;
	}

	x1 = 305;
	y1 = 275;
	for (int i = 0; i <= 9; i++) {
		glRectf(x1, y1, x1 + 50, y1 + 50);
		x1 += 60;
	}
	x1 = 305;
	y1 = 215;
	for (int i = 0; i <= 9; i++) {
		glRectf(x1, y1, x1 + 50, y1 + 50);
		x1 += 60;
	}
	x1 = 504;
	y1 = 455;
	for (int i = 0; i <= 2; i++) {
		glRectf(x1, y1, x1 + 58, y1 + 45);
		x1 += 67;

	}
	x1 = 504;
	y1 = 505;
	for (int i = 0; i <= 2; i++) {
		glRectf(x1, y1, x1 + 58, y1 + 45);
		x1 += 67;
	}
	//draw the door
	glColor3ub(13, 59, 102);
	glBegin(GL_POLYGON);
	glVertex2f(600, 200);
	glVertex2f(650, 200);
	glVertex2f(650, 270);
	glVertex2f(550, 270);
	glVertex2f(550, 200);
	glVertex2f(600, 200);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3ub(61, 90, 128);
	glVertex2f(600, 270);
	glVertex2f(680, 270);
	glVertex2f(640, 295);
	glVertex2f(560, 295);
	glVertex2f(520, 270);
	glVertex2f(600, 270);
	glEnd();
	


}
void Tree() {
	glBegin(GL_POLYGON);
	glColor3ub(26, 125, 6);
	glVertex2f(150, 190);
	glVertex2f(230, 190);
	glVertex2f(150, 270);
	glVertex2f(70, 190);
	glVertex2f(150, 190);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3ub(119, 73, 54);
	glVertex2f(170, 190);
	glVertex2f(130, 190);
	glVertex2f(130, 100);
	glVertex2f(170, 100);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3ub(26, 125, 6);
	glVertex2f(1050, 190);
	glVertex2f(1130, 190);
	glVertex2f(1050, 270);
	glVertex2f(970, 190);
	glVertex2f(1050, 190);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3ub(119, 73, 54);
	glVertex2f(1070, 190);
	glVertex2f(1030, 190);
	glVertex2f(1030, 100);
	glVertex2f(1070, 100);
	glEnd();



	}

void Grass() {
	for (int i = 0; i < width; i += 50) {
		glBegin(GL_TRIANGLES);
		glColor3ub(0, 75, 35);
		glVertex2f(i, 0);
		glVertex2f(i + 50, 0);
		glColor3ub(158, 240, 26);
		glVertex2f(i + 25, 60);
		glEnd();
	}

}
void Sun() {
	//draw a sun
	float R = 105;
	int n = 200;
	glColor3ub(255, 121, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(R * cos(2 * PI * i / n) + 1095, R * sin(2 * PI * i / n) + 695);
	}
	glEnd();
	glFlush();
	
}

void Balloon(float x, float y, int R, int G, int B) {
	int i;
	float r = 35.0f;
	int n = 200;
	glColor3ub(R, G, B);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++) {
		glVertex2f(x + r * cos(2 * PI * i / n), y + r * sin(2 * PI * i / n));
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(x, y - r);
	glVertex2f(x, y - r - 200);
	glEnd();
}

void Background()
{
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glVertex2f(0, 0);
	glColor3ub(0, 51, 102);
	glVertex2f(0, height);
	glColor3ub(144, 224, 239);
	glVertex2f(width, height);
	glColor3f(153, 204, 255);
	glVertex2f(width, 0);
	glEnd();
}