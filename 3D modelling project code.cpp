// File ID: CPT205 Assessment 2
// Title: 3-Dimensional scene
// Author: Chuanxin Zhai  1929631

/*In this CW2, I wrote a 3-Dimensional scene（a small beautiful park）. I use camera to represent the people's eyes. What I mean is it is like a first-person perspective tour.
 You can visit it in most places (some places I add several restrictions so that it looks like more real. Using keyboard, you can surround the park. Using mouse, you even could fly to the sky.
 More details will be demostrated in the written report. Hope you have fun in this 3-Dimensional scene!
*/


#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4

using namespace std;

//define math value and define color
#define white 1.0, 1.0, 1.0
#define black 0.0, 0.0, 0.0
#define root_3 1.732
#define root_2 1.414
#define sin_45 0.71
const double PI = acos(-1.0);

//windows size
GLint Width = 1200;
GLint Height = 800;

struct point {
	GLfloat x, y, z;
};

//脑袋位置
GLfloat camera_x = 0.0;
GLfloat camera_y = 155.0;
GLfloat camera_z = 1700.0;
//人眼朝向
GLfloat lookat_x = 0.0;
GLfloat lookat_y = 175.0;
GLfloat lookat_z = 1700.0;
//脑袋朝向
GLfloat view_x = 0.0;
GLfloat view_y = 1.0;
GLfloat view_z = 0.0;

// glFrustum
GLfloat GLdouble_left = -120.0;
GLfloat GLdouble_right = 120.0;
GLfloat GLdouble_bottom = -100.0;
GLfloat GLdouble_top = 100.0;
GLfloat GLdouble_near = 99;
GLfloat GLdouble_far = 7000.0;

// start direction  最一开始面朝的方向
GLfloat n =570.0;
//sunrize  起始时间设成日出（凌晨刚出太阳）
GLint day_time = 0;

GLint sea_wave = 0;

GLint step = 25;

GLfloat cloud_pos[100][10];
//is used to write the trees' rotation
GLfloat tree_degree = 0.0;
bool tree_dir = 1;

bool winter = false;


//textures data
GLuint textures[20];
GLubyte* wood_1_data;
GLubyte* wood_2_data;
GLubyte* stone_1_data;
GLubyte* stone_2_data;
GLubyte* sea_data;
GLubyte* sand_data;
GLubyte* grass_data;
GLubyte* word_data;
GLubyte* sky_data[5];  //skybox: daytime
GLubyte* night_data[5]; //skybox: night


//viewport
void myReshapeFunc(GLint new_width, GLint new_height)
{
	glViewport(0, 0, new_width, new_height);
}


// animation动画
void myIdleFunc(void)
{
	// branches sway 树枝摇晃 (动画 1)
	if (tree_degree >= 4)
		tree_dir = 0;
	else if (tree_degree <= -4)
		tree_dir = 1;
	if (tree_dir == 1)
		tree_degree += 0.3;
	else
		tree_degree -= 0.5;
	// clouds float 飘着的云 (动画 2)
	for (int i = 0; i < 10; i++)
	{
		cloud_pos[i][0] += cloud_pos[i][4];
		if (cloud_pos[i][0] >= 2800)
			cloud_pos[i][0] = -2800;
	}
	// sea wave海浪 (动画 3)
	sea_wave = sea_wave + 60;
	glutPostRedisplay();	// redisplay 一直重放，达成像是海浪一直在冲刷的视觉效果
}



// restrict areas can be moved to, return true or false  限定一下camera可移动的范围（不能无限制位移，不然会出skybox或穿模）
bool move_restriction(GLfloat pos_x, GLfloat pos_z)
{
	if (pos_z > 3150 || pos_z < -480)  //Left and right boundaries  左右边界
		return false;
	if (pos_x > 1200 || pos_x < -1600) //Front and Back boundaries 前后边界
		return false;
	//Limit the camera displacement on the bridge, in case something unrealistic happens (people on the sea)  
	//限定一下桥上camera的位移，以防发生不实际的事情（人在海面上）
	if (abs(pos_x) > 80 && pos_z < 1500 && pos_z > 500)
		return false;
	if (abs(pos_x) > 350 && pos_z < 1500)
		return (abs(pos_x) - 350) * root_3 < 600 - abs(pos_z);
	return true;
}

//special keys上下左右键控制行走 control walk
void processSpecialKeys(int key, int xx, int yy) {
	// record oringinal position
	GLfloat oringin_x = lookat_x, oringin_z = lookat_z;
	GLfloat oringinal_x = camera_x, oringinal_z = camera_z;
	switch (key) {
		// n是旋转角度
	case GLUT_KEY_RIGHT:
		camera_x -= step * cos(n / 200);
		lookat_x -= step * cos(n / 200);
		camera_z += step * sin(n / 200);
		lookat_z += step * sin(n / 200);
		break;
	case GLUT_KEY_LEFT:
		camera_x += step * cos(n / 200);
		lookat_x += step * cos(n / 200);
		camera_z -= step * sin(n / 200);
		lookat_z -= step * sin(n / 200);
		break;
	case GLUT_KEY_DOWN:
		camera_z -= step * cos(n / 200);
		lookat_z -= step * cos(n / 200);
		camera_x -= step * sin(n / 200);
		lookat_x -= step * sin(n / 200);
		break;
	case GLUT_KEY_UP:
		camera_z += step * cos(n / 200);
		lookat_z += step * cos(n / 200);
		camera_x += step * sin(n / 200);
		lookat_x += step * sin(n / 200);
		break;
	}
	//restrictions: if people cannot walk on, the camera would still stay at the oringinal place.
	if (!move_restriction(camera_x, camera_z)) {
		lookat_x = oringin_x, lookat_z = oringin_z;
		camera_x = oringinal_x, camera_z = oringinal_z;
	}
}
// keyboard interactions 
	//WASD旋转视角 rotate view angle
void myKeyboardFunc(unsigned char key, int x, int y)
{
	// move the position of the camera
	if (key == 'w' || key == 'W')	 // 视角往上camera_up
		lookat_y += 10;
	if (key == 's' || key == 'S')    // 视角往下camera_down
		lookat_y -= 10;
	// n是旋转角度
	if (key == 'a' || key == 'A')	// 视角往左camera_left
		n += 25;
	if (key == 'd' || key == 'D')   // 视角往右camera_right
		n -= 25;
	lookat_x = 200 * sin(n / 100) + camera_x;
	lookat_z = 200 * cos(n / 100) + camera_z;

	if (key == 't' || key == 'T')  // change day_time
		day_time = (day_time + 1) % 3;
	// change the speed of moving
	if (key == 's')
		step += 2;  // step = step + 2
	if (key == 'S' && step > 2)
		step -= 2;   // step = step - 2
}

// mouse interaction 鼠标控制
void mouseInput(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_WHEEL_UP) {	// up 滑轮向上
		if (camera_y < 2000)
			camera_y += step, lookat_y += step;
	}
	else if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {	// down 滑轮向下
		if (camera_y > 120)
			camera_y -= step, lookat_y -= step;
	}
}


// texture images
GLubyte* loadImg(string filename) {
	GLint i_Width;
	GLint i_Height;
	GLint pixel_length;
	GLubyte* data;
	FILE* pfile = NULL;
	fopen_s(&pfile, filename.c_str(), "rb"); 
	if (pfile == 0) exit(0);
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&i_Width, sizeof(i_Width), 1, pfile);
	fread(&i_Height, sizeof(i_Height), 1, pfile);
	pixel_length = i_Width * 3;
	while (pixel_length % 4 != 0)
		pixel_length++;
	pixel_length *= i_Height;		
	data = (GLubyte*)malloc(pixel_length);	
	if (data == 0)
		exit(0);
	fseek(pfile, 54, SEEK_SET);
	fread(data, pixel_length, 1, pfile);
	fclose(pfile);
	// BGR→RGB
	for (int i = 0; i < pixel_length / 3; i++) {		
		GLubyte x = data[i * 3];
		GLubyte z = data[i * 3 + 2];
		data[i * 3 + 2] = x;
		data[i * 3] = z;
	}
	return data;	// save the texture information in data and return
}


// point
point toPoint(GLfloat x, GLfloat y, GLfloat z) {
	point p;
	p.x = x, p.y = y, p.z = z;
	return p;
}



// 开始画objects
// 立方体（先画好，封装）Draw a cuboid and package it
void drawCuboid(GLfloat cen_x, GLfloat cen_y, GLfloat cen_z, GLfloat len_x, GLfloat len_y, GLfloat len_z) {
	GLfloat x_start = cen_x - len_x, x_end = cen_x + len_x;
	GLfloat y_start = cen_y - len_y, y_end = cen_y + len_y;
	GLfloat z_start = cen_z - len_z, z_end = cen_z + len_z;
	//repeat
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_start, y_start, z_end);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_start, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_start, z_start);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_end, y_start, z_end);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_start, y_end, z_end);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_end, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_start);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_end, y_end, z_end);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_start, y_start, z_end);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_start, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_start, y_end, z_start);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_start, y_end, z_end);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_end, y_start, z_end);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_end, y_start, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_start);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_end, y_end, z_end);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_start, y_start, z_end);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_end, z_end);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_end);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_end, y_start, z_end);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_end, y_start, z_end);
	glEnd();
}

// 柱（先画好，封装）Draw a cylinder and package it
void drawCylinder(GLfloat cen_x, GLfloat cen_y, GLfloat cen_z, GLfloat d, GLfloat len, GLfloat degree, bool need) {
	GLUquadricObj* objCylinder = gluNewQuadric();
	gluQuadricTexture(objCylinder, GL_TRUE);
	glPushMatrix();
	glTranslatef(cen_x, cen_y, cen_z);
	glPushMatrix();
	glRotatef(degree, 1, 0, 0);
	gluCylinder(objCylinder, d, d, len, 32, 5);
	if (need) {
		glutSolidSphere(d, 19, 19);
		glPushMatrix();
		glTranslatef(0, 0, len);
		glutSolidSphere(d, 19, 19);
		glPopMatrix();
	}
	glPopMatrix();
	glPopMatrix();
}

void drawHexagon(GLfloat cen_x, GLfloat cen_y, GLfloat cen_z, GLfloat len, GLfloat thickness, GLfloat tex_size) {
	// computing the six vertexs of the hexagon
	point points[6] = { toPoint(cen_x - len / 2, cen_y + thickness, cen_z + len / 2 * root_3),
						toPoint(cen_x + len / 2, cen_y + thickness, cen_z + len / 2 * root_3),
						toPoint(cen_x + len, cen_y + thickness, cen_z),
						toPoint(cen_x + len / 2, cen_y + thickness, cen_z - len / 2 * root_3),
						toPoint(cen_x - len / 2, cen_y + thickness, cen_z - len / 2 * root_3),
						toPoint(cen_x - len, cen_y + thickness, cen_z) };
	//表面贴图
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0), glVertex3f(points[0].x, points[0].y, points[0].z);
	glTexCoord2f(0.0, tex_size), glVertex3f(points[1].x, points[1].y, points[1].z);
	glTexCoord2f(tex_size / 2, tex_size), glVertex3f(points[2].x, points[2].y, points[2].z);
	glTexCoord2f(tex_size, tex_size), glVertex3f(points[3].x, points[3].y, points[3].z);
	glTexCoord2f(tex_size, 0.0), glVertex3f(points[4].x, points[4].y, points[4].z);
	glTexCoord2f(tex_size / 2, 0.0), glVertex3f(points[5].x, points[5].y, points[5].z);
	glEnd();

}

// lighting 光照
	// spot light 点光源
void pointLight(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z) {
	GLfloat light_position[] = { pos_x, pos_y, pos_z, 5 };
	GLfloat light_ambient[] = {0.9, 0.9, 0.5, 0.9 }, light_diffuse[] = { 0.9, 0.9, 0.78, 0.9 };
	GLfloat light_specular[] = { 0.9, 0.9, 0.9, 0.9 };

	GLfloat light_ambient2[] = { 0.9, 0.9, 0.9, 0.9 }, light_diffuse2[] = { 0.9, 0.9, 0.9,0.9 };
	GLfloat light_specular2[] = { 0.9, 0.9, 0.9, 0.9 };

	GLfloat light_ambient3[] = { 165.0 / 255.0, 66.6 / 255.0, 222.3 / 255.0 };
	GLfloat light_diffuse3[] = { 221.0 / 255.0, 66.6 / 255.0, 164.0 / 255.0 };

	glEnable(GL_LIGHT1);
	if (day_time == 0) {       // sunrize日出（凌晨）
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 8.8);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	}
	else if (day_time == 1) {  // noon白天（中午）
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 11.2);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	}
	else (day_time == 2); {    //nightfall 日落（晚上）
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 3.4);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient3);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse3);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
	}
}

//parallel light平行光
void parallelLight() {
	GLfloat light_ambient[] = { 0.8, 0.8, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 1.0, 0.5, 1.0 };
	GLfloat light_specular[] = { 0.0, 0.5, 0.5, 1.0 };
	GLfloat lightDirection[] = { 0.0, 0.9, 0.9, 0.0 };
	if (day_time == 0)	//to make sunrize distinguish from nightfall   可以看出日出（凌晨）和日落（晚上）光是不一样的
		glEnable(GL_LIGHT2);
	else
		glDisable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, lightDirection);
}


//开始画buildings
// Road路
void drawRoad(GLfloat x, GLfloat degree) {
	glPushMatrix();
	glRotated(degree, 0, 1, 0);
	// line_2
	drawCuboid(x - 30, 20, -47, 13, 1, 18);
	drawCuboid(x - 30, 20, -16, 13, 1, 10);
	drawCuboid(x - 30, 20, 31, 13, 1, 34);
	// line_2
	drawCuboid(x, 20, -51, 13, 1, 14);
	drawCuboid(x, 20, 15.5, 13, 1, 49.5);
	// line_3
	drawCuboid(x + 30, 20, -35, 13, 1, 30);
	drawCuboid(x + 30, 20, 18, 13, 1, 20);
	drawCuboid(x + 30, 20, 53, 13, 1, 12);
	glPopMatrix();
}

// chair 椅子
void drawChair(GLfloat x_loc, GLfloat z_loc) {
	glPushMatrix();
	glTranslatef(x_loc, 0, z_loc);
	glBindTexture(GL_TEXTURE_2D, textures[0]); // wood_1 texture
	for (int i = 0; i < 4; i++)
		drawCuboid(x_loc, 66, -15 + i * 10, 75, 1, 3.5);
	glPushMatrix();
	glRotatef(15, 1, 0, 0);
	for (int i = 0; i < 4; i++)
		drawCuboid(x_loc, 81 + i * 10, 0, 75, 4, 1);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[2]); // stone_1 texture
	drawCuboid(x_loc, 20, 0, 100, 1, 25);
	glBindTexture(GL_TEXTURE_2D, textures[3]);  // stone_2 texture
	// 椅子腿
	for (int i = -1; i < 2; i += 2) {
		drawCuboid(x_loc + 52 * i, 43, -16 * i, 5, 22, 1);
		drawCuboid(x_loc + 52 * i, 64, 0, 5, 1, 17);
		drawCuboid(x_loc - 52, 43, 16 * i, 5, 22, 1);
		drawCuboid(x_loc + 52 * i, 43, 18, 3.5, 22, 1);
	}

	glPushMatrix();
	glRotatef(15, 1, 0, 0);
	drawCuboid(x_loc + 52, 90, 2, 3.5, 27, 1);
	drawCuboid(x_loc - 52, 90, 2, 3.5, 27, 1);
	glPopMatrix();
	glPopMatrix();
}

// 树
void drawTree(GLfloat x_loc, GLfloat z_loc) {
	// using grass texture to draw the leaf
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glPushMatrix();
	glTranslatef(x_loc, 0, z_loc);
	for (int i = 0; i < 6; i++) {
		// the lowest level leaf with the darkest color
		glColor3f(43 / 255.0, 80 / 255.0, 9 / 255.0);
		glPushMatrix();
		glRotatef(tree_degree + 90 + i * 60, 0, 1, 0);
		for (int j = -8; j < 6; j++)
			drawCuboid(47 + j * 6, 240 - j * j * 1.7, 0, 3, 75, 1);
		glPopMatrix();
		// middle level leaf with middle color
		glColor3f(106 / 255.0, 150 / 255.0, 36 / 255.0);
		glPushMatrix();
		glRotatef(tree_degree + 110 + i * 60, 0, 1, 0);
		for (int j = -5; j < 8; j++)
			drawCuboid(37 + j * 6, 274 - j * j * 1.5, 0, 3, 75, 0.5);
		glPopMatrix();
		// the highest level leaf with the shallowest color
		glColor3f(171 / 255.0, 211 / 255.0, 60 / 255.0);
		glPushMatrix();
		glRotatef(tree_degree + 130 + i * 60, 0, 1, 0);
		for (int j = -2; j < 8; j++)
			drawCuboid(20 + j * 6, 315 - j * j * 1.5, 0, 3, 75, 0.5);
		glPopMatrix();
	}
	glPopMatrix();

	// using wood texture to draw the truck
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glPushMatrix();
	glTranslatef(x_loc, 0, z_loc);
	glColor3ub(112, 84, 56);

	// draw branch when winter
	if (winter) {
		for (int i = 0; i < 6; i++) {
			// the lowest level branch
			glPushMatrix();
			glRotatef(i * 60, 0, 1, 0);

			glPopMatrix();
			// middle level branch
			glPushMatrix();
			glRotatef(20 + i * 60, 0, 1, 0);

			glPopMatrix();
			// the highest level branck
			glPushMatrix();
			glRotatef(40 + i * 60, 0, 1, 0);

			glPopMatrix();
		}
	}
	glColor3f(white);
	glPopMatrix();
}

// draw street light (short) in the lawn
void drawStreetLight(GLfloat x_loc, GLfloat z_loc) {
	glDisable(GL_TEXTURE_2D);
	glColor3f(black);
	glPushMatrix();
	glTranslatef(x_loc, 0, z_loc);	// set position

	// body of the light
	drawCuboid(0, 42, 0, 8, 22, 8);
	drawCuboid(0, 67, 0, 6, 3, 6);

	// x轴
	drawCuboid(0, 86, 7, 8, 1, 1);
	drawCuboid(0, 86, -7, 8, 1, 1);
	drawCuboid(0, 70, 7, 8, 1, 1);
	drawCuboid(0, 70, -7, 8, 1, 1);
	// y轴
	drawCuboid(7, 78, -7, 1, 8, 1);
	drawCuboid(-7, 78, -7, 1, 8, 1);
	drawCuboid(7, 78, 7, 1, 8, 1);
	drawCuboid(-7, 78, 7, 1, 8, 1);
	// z轴
	drawCuboid(7, 86, 0, 1, 1, 8);
	drawCuboid(-7, 86, 0, 1, 1, 8);
	drawCuboid(7, 70, 0, 1, 1, 8);
	drawCuboid(-7, 70, 0, 1, 1, 8);
	// the light
	GLfloat emmission[] = { 0.7, 0.7, 0.7 };	// light up
	GLfloat no_emmission[] = { 0, 0, 0 };	// light off
	if (day_time == 2)	// only light up in the night (time state is 2)
		glMaterialfv(GL_FRONT, GL_EMISSION, emmission);
	else
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);
	glColor3f(1, 1, 1);
	drawCuboid(0, 78, 0, 6, 6, 6);

	glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

// draw the lawn
void drawLawn() {
	GLfloat x_start = -2000.0, x_len = 4000;	// set the size of the lawn
	GLfloat z_start = 1740, z_len = 2000;
	GLfloat tex_len = 200;	// size of piece of the grass
	GLfloat lawn_z = 1825;

	// using grass texture
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	for (int i = 0; i < x_len / tex_len; i++) {
		for (int j = 0; j < z_len / tex_len; j++) {
			glBegin(GL_QUADS);	// bind texture
			glTexCoord2f(0.0, 0.0), glVertex3f(x_start + tex_len * i, 19, z_start + 200 * (j + 1));
			glTexCoord2f(0.0, 1.0), glVertex3f(x_start + tex_len * i, 19, z_start + 200 * j);
			glTexCoord2f(1.0, 1.0), glVertex3f(x_start + tex_len * (i + 1), 19, z_start + 200 * j);
			glTexCoord2f(1.0, 0.0), glVertex3f(x_start + tex_len * (i + 1), 19, z_start + 200 * (j + 1));
			glEnd();
		}
	}

	// draw roads
	glBindTexture(GL_TEXTURE_2D, textures[2]);	// using stone texture
	glPushMatrix();
	glTranslatef(0, 0, lawn_z);
	for (int i = 8; i < 23; i++) {
		drawRoad(90 * i, 0);
		drawRoad(-90 * i, 0);
	}
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, lawn_z + 133);
	for (int i = -7; i <= 7; i++)
		drawRoad(90 * i, 0);
	glPopMatrix();
	for (int i = -1; i <= 1; i += 2)
	{
		glPushMatrix();
		glTranslatef(i * 741.5, 0, lawn_z + 22);
		for (int i = 1; i < 20; i++)
			drawRoad(i * 90, 270);
		glPopMatrix();
	}

	// draw chairs
	drawChair(250, lawn_z);
	drawChair(-250, lawn_z);

	// draw trees (forests)
	for (int i = -1; i < 2; i += 2) {
		drawTree(635 * i, lawn_z);
		drawTree(351 * i, lawn_z);
		drawTree(117 * i, lawn_z);
	}
	for (int i = 0; i < 5; i++) {
		drawTree(840 + 280 * i, lawn_z + 110);
		drawTree(-840 - 280 * i, lawn_z + 110);
	}
	for (int i = -3; i < 4; i++)
		drawTree(211 * i, lawn_z + 240);
	for (int i = -6; i < 7; i++) {
		drawTree(280 * i, lawn_z + 640);
		drawTree(280 * i, lawn_z + 1040);
		drawTree(280 * i, lawn_z + 1440);
	}

	// draw street lights
	drawStreetLight(0, lawn_z);
	drawStreetLight(234, lawn_z);
	drawStreetLight(-234, lawn_z);
	for (int i = 0; i < 4; i++) {
		drawStreetLight(980 + 280 * i, lawn_z + 110);
		drawStreetLight(-980 - 280 * i, lawn_z + 110);
	}
	for (int i = 0; i < 6; i++)
		drawStreetLight(-527.5 + 211 * i, lawn_z + 240);
}
void drawGuard() {
	glBindTexture(GL_TEXTURE_2D, textures[0]); //贴图
	glColor3ub(237,221,158);
	for (int i = 0; i < 10; i++)
		drawCuboid(0, 10, 659 + 90 * i, 70, 10, 42);
}


// 顶ceiling
void drawCeiling() {
	GLfloat stick_num = 32;
	//顶1
	GLfloat height_1 = 1120.9, len = 599.9, len2 = 280;	
	//顶2
	GLfloat height_2 =880.9, len_2 = 699.9, len2_2 = 300;	

	glBindTexture(GL_TEXTURE_2D, textures[0]); //贴图
	for (int j = 0; j < 6; j++) {
		glPushMatrix();
		glRotatef(60 * j, 0, 1, 0);
		// 顶1_的顶
		for (int i = 0; i < 13; i++) {
			drawCylinder(19 * i, height_1 - stick_num * i, stick_num * i, 10, len - stick_num / sin_45 * i, 45, false);
			drawCylinder(-19 * i, height_1 - stick_num * i, stick_num * i, 10, len - stick_num / sin_45 * i, 45, false);
		}
		// 顶2_的顶
		for (int i = 0; i < 15; i++) {
			drawCylinder(19 * i, height_2 - stick_num * i, stick_num * i, 10, len_2 - stick_num / sin_45 * i, 45, false);
			drawCylinder(-19 * i, height_2 - stick_num * i, stick_num * i, 10, len_2 - stick_num / sin_45 * i, 45, false);
		}
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, textures[1]); //换贴图
	for (int j = 0; j < 6; j++) {
		glPushMatrix();
		glRotatef(60 * j, 0, 1, 0);
		// 框架1
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		drawCylinder(-len * sin_45, height_1 - len * sin_45, -len2, 15, len2 * 2, 0, false);	
		drawCylinder(-len_2 * sin_45, height_2 - len_2 * sin_45, -len2_2, 15, len2_2 * 2, 0, false);	
		drawCylinder(175 * root_3, 605, -190, 10, 380, 0, false);	// center
		glPopMatrix();
		// 框架2
		glPushMatrix();
		glRotatef(30, 0, 1, 0);
		drawCylinder(0, height_1, 0, 15, len * 1.12, 40, true);	
		drawCylinder(0, height_2, 0, 15, len_2 * 1.12, 40, true);	
		glPopMatrix();
		glPopMatrix();
	}
	// 顶尖
	drawCylinder(0, height_1 + 166, 0, 41, 130, 100, true);
}

// pavilion亭子 
void drawPavilion() {
	drawCeiling();

	glBindTexture(GL_TEXTURE_2D, textures[0]);	// using wood texture
	// base of the pavilion
	drawHexagon(0, 9, 0, 700, 9, 5);
	drawHexagon(0, -130, 0, 648, 130, 5);
	// surface of seats
	for (int i = 1; i < 6; i++) {
		glPushMatrix();
		glRotatef(60 * i, 0, 1, 0);
		for (int j = 0; j < 4; j++)
			drawCuboid(0, 57.5, 450 / 2 * root_3 - 15 + j * 10, 140, 2.5, 3.5);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, textures[1]);		// using red wood texture
	// 亭子的柱子 columns
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glColor3ub(116, 125, 140);
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glRotatef(60 * i, 0, 1, 0);
		drawCylinder(450, 500, 0, 20, 494, 90, false);
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, textures[2]);	// using stone_1 texture
	// 碑林
	drawCuboid(0, 30, 0, 60, 10, 25);
	drawCuboid(0, 170, 0, 40, 130, 10);


	glBindTexture(GL_TEXTURE_2D, textures[3]);	// stone_2 texture
	// base of seats
	for (int i = 1; i < 6; i++) {
		glPushMatrix();
		glRotatef(60 * i, 0, 1, 0);
		drawCuboid(0, 53, 450 / 2 * root_3, 154, 2, 20);
		for (int j = 1; j < 4; j++)
			drawCuboid(-300 + j * 600 / 4, 32, 450 / 2 * root_3, 10, 32, 20);
		glPopMatrix();
	}
	// board
	drawCuboid(0, 675, 175 * root_3, 150, 55, 5);

	// word on the board
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.6), glVertex3f(-138, 715, 175 * root_3 + 6);
	glTexCoord2f(0.0, 0.3), glVertex3f(-138, 635, 175 * root_3 + 6);
	glTexCoord2f(1.0, 0.3), glVertex3f(138, 635, 175 * root_3 + 6);
	glTexCoord2f(1.0, 0.6), glVertex3f(138, 715, 175 * root_3 + 6);
	glEnd();
}



// draw bank and things on it
void drawBank() {
	/// draw the base of the bank
	/// choose stone texture and tile the texture according to length
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, -252, 1500);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 10, 1500);
	glTexCoord2f(10.0, 1.0), glVertex3f(-1030, 10, 1500);
	glTexCoord2f(10.0, 0.0), glVertex3f(-1030, -252, 1500);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, -252, 1700);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 18, 1700);
	glTexCoord2f(5.0, 1.0), glVertex3f(2000, 18, 1700);
	glTexCoord2f(5.0, 0.0), glVertex3f(2000, -252, 1700);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(-2000, -252, 1700);
	glTexCoord2f(0.0, 1.0), glVertex3f(-2000, 18, 1700);
	glTexCoord2f(5.0, 1.0), glVertex3f(-1030, 18, 1700);
	glTexCoord2f(5.0, 0.0), glVertex3f(-1030, -252, 1700);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 10, 1700);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 10, 1500);
	glTexCoord2f(10.0, 1.0), glVertex3f(-1030, 10, 1500);
	glTexCoord2f(01.0, 0.0), glVertex3f(-1030, 10, 1700);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(2000, 10, 1800);
	glTexCoord2f(0.0, 1.0), glVertex3f(2000, 10, 1700);
	glTexCoord2f(20.0, 1.0), glVertex3f(-2000, 10, 1700);
	glTexCoord2f(20.0, 0.0), glVertex3f(-2000, 10, 1800);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 10, 1500);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 10, 1700);
	glTexCoord2f(1.0, 1.0), glVertex3f(1010, -252, 1700);
	glTexCoord2f(1.0, 0.0), glVertex3f(1010, -252, 1500);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(-1030, 10, 1500);
	glTexCoord2f(0.0, 1.0), glVertex3f(-1030, 10, 1700);
	glTexCoord2f(1.0, 1.0), glVertex3f(-1030, -252, 1700);
	glTexCoord2f(1.0, 0.0), glVertex3f(-1030, -252, 1500);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(2000, 18, 3500);
	glTexCoord2f(0.0, 1.0), glVertex3f(2000, 18, 1700);
	glTexCoord2f(9.0, 1.0), glVertex3f(2000, -252, 1700);
	glTexCoord2f(9.0, 0.0), glVertex3f(2000, -252, 3500);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(-2000, 18, 3500);
	glTexCoord2f(0.0, 1.0), glVertex3f(-2000, 18, 1700);
	glTexCoord2f(9.0, 1.0), glVertex3f(-2000, -252, 1700);
	glTexCoord2f(9.0, 0.0), glVertex3f(-2000, -252, 3500);
	glEnd();
	// draw stairs on the side of the bank
	GLfloat stair_len = 20, stair_height = 110 / 8;
	for (int i = 1; i <= 8; i++)
		drawCuboid(1010 + stair_len * (2 * i - 1), -90 - stair_height * i, 1700, stair_len, 110 - stair_height * i, 160);
	glBegin(GL_QUADS);	// bind textures on stairs
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 20, 1500);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 20, 1540);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -200, 1540);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -200, 1500);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 20, 1540);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, -200, 1540);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -200, 1540);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 20, 1500);
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, -200, 1500);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -200, 1500);
	glEnd();

	//围栏的柱子
	for (int i = 0; i <= 10; i++) {
		drawCuboid(70 + 2 * 47 * i, 60, 1520, 7, 40, 7);
		drawCuboid(-70 - 2 * 47 * i, 60, 1520, 7, 40, 7);
		drawCuboid(-1010 - 2 * 47 * i, 60, 1720, 7, 40, 7);
		drawCuboid(1010 + 2 * 47 * i, 60, 1720, 7, 40, 7);
	}
	
	for (int i = 1; i < 5; i++)
		drawCuboid(1010 + 330 / 4 * i, 60 - i * 220 / 4, 1520, 7, 40, 7);
	
	for (int i = 1; i <= 2; i++)
		drawCuboid(-1010, 60, 1520 + i * 100, 7, 40, 7);

	glBindTexture(GL_TEXTURE_2D, textures[0]);   //  wood_1 texture
	// draw planks on the base
	for (int i = 0; i < 112; i++)
		drawCuboid(-1010 + i * 18, 14, 1610, 7, 4, 90);
	// draw the plank of the Guard (all together)
	drawCuboid(540, 80, 1520, 470, 5, 5);	// top plank
	drawCuboid(540, 45, 1520, 470, 25, 5);	// bottom plank
	drawCuboid(-540, 80, 1520, 470, 5, 5);
	drawCuboid(-540, 45, 1520, 470, 25, 5);
	drawCuboid(-1510, 80, 1720, 500, 5, 5);
	drawCuboid(-1510, 45, 1720, 500, 25, 5);
	drawCuboid(1510, 80, 1720, 500, 5, 5);
	drawCuboid(1510, 45, 1720, 500, 25, 5);
	// plank of te Guard of the stair
	glBegin(GL_QUADS);	// top plank
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 70, 1515);	// top side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 70, 1525);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -150, 1525);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -150, 1515);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 70, 1525);	// back side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 20, 1525);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -200, 1525);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -150, 1525);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 70, 1515);	// front side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 20, 1515);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -200, 1515);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -150, 1515);
	glEnd();		// do not draw sides that can't be seen
	glBegin(GL_QUADS);	// bottom plank
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 75, 1515);	// top side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 75, 1525);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -145, 1525);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -145, 1515);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 85, 1525);	// back side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 75, 1525);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -145, 1525);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -135, 1525);
	glTexCoord2f(0.0, 0.0), glVertex3f(1010, 85, 1515);	// front side
	glTexCoord2f(0.0, 1.0), glVertex3f(1010, 75, 1515);
	glTexCoord2f(1.0, 1.0), glVertex3f(1344, -145, 1515);
	glTexCoord2f(1.0, 0.0), glVertex3f(1344, -135, 1515);
	glEnd();	// do not draw sides that can't be seen
	// side plank
	drawCuboid(-1010, 80, 1620, 5, 5, 100);
	drawCuboid(-1010, 45, 1620, 5, 25, 100);
}

// draw a cloud
void drawCloud(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat size, GLfloat color) {
	glDisable(GL_TEXTURE_2D);
	if (day_time < 2)	// set color of cloud by time state
		glColor3f(color, color, color);
	else
		glColor3f(color - 0.3, color - 0.3, color - 0.3);
	glPushMatrix();	// draw cloud (made up of spheres)
	glTranslatef(pos_x, pos_y, pos_z);
	glutSolidSphere(70 * size, 200, 200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(pos_x + 50 * size, pos_y, pos_z);
	glutSolidSphere(60 * size, 200, 200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(pos_x + 90 * size, pos_y - 30 * size, pos_z - 30 * size);
	glutSolidSphere(35 * size, 200, 200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(pos_x - 60 * size, pos_y - 15 * size, pos_z + 20 * size);
	glutSolidSphere(50 * size, 200, 200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(pos_x - 55 * size, pos_y - 25 * size, pos_z - 45 * size);
	glutSolidSphere(30 * size, 200, 200);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
}

// draw skybox
void drawSky() {
	// set position
	GLfloat x_start = -2500, x_end = 2500;
	GLfloat y_start = -800, y_mid = 0.0, y_end = 3300;
	GLfloat z_start = -1500, z_end = 3500;
	GLfloat lookat_y, y_rate;
	int daytime;	// choose sky picture by time state
	if (time == 0 || day_time == 1) {
		daytime = 8;
		lookat_y = 0.5, y_rate = 0.13;
	}
	else {
		daytime = 13;
		lookat_y = 0.5, y_rate = 0.4;
	}
	// bind textures
	//front side
	glBindTexture(GL_TEXTURE_2D, textures[daytime]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_end, y_end, z_end);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_end, y_mid, z_end);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_start, y_mid, z_end);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_start, y_end, z_end);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_end, y_mid, z_end);
	glTexCoord2f(0.0, y_rate), glVertex3f(x_end, y_start, z_end);
	glTexCoord2f(1.0, y_rate), glVertex3f(x_start, y_start, z_end);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_start, y_mid, z_end);
	glEnd();
	// right side
	glBindTexture(GL_TEXTURE_2D, textures[daytime + 1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_end, y_end, z_start);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_end, y_mid, z_start);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_end, y_mid, z_end);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_end);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_end, y_mid, z_start);
	glTexCoord2f(0.0, y_rate), glVertex3f(x_end, y_start, z_start);
	glTexCoord2f(1.0, y_rate), glVertex3f(x_end, y_start, z_end);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_end, y_mid, z_end);
	glEnd();
	// back side
	glBindTexture(GL_TEXTURE_2D, textures[daytime + 2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_end, z_start);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_start, y_mid, z_start);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_end, y_mid, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_start);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_start, y_mid, z_start);
	glTexCoord2f(0.0, y_rate), glVertex3f(x_start, y_start, z_start);
	glTexCoord2f(1.0, y_rate), glVertex3f(x_end, y_start, z_start);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_end, y_mid, z_start);
	glEnd();
	// left side
	glBindTexture(GL_TEXTURE_2D, textures[daytime + 3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_start, y_end, z_end);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_start, y_mid, z_end);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_start, y_mid, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_start, y_end, z_start);
	glTexCoord2f(0.0, lookat_y), glVertex3f(x_start, y_mid, z_end);
	glTexCoord2f(0.0, y_rate), glVertex3f(x_start, y_start, z_end);
	glTexCoord2f(1.0, y_rate), glVertex3f(x_start, y_start, z_start);
	glTexCoord2f(1.0, lookat_y), glVertex3f(x_start, y_mid, z_start);
	glEnd();
	// top side
	glBindTexture(GL_TEXTURE_2D, textures[daytime + 4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(x_start, y_end, z_end);
	glTexCoord2f(1.0, 0.0), glVertex3f(x_start, y_end, z_start);
	glTexCoord2f(1.0, 1.0), glVertex3f(x_end, y_end, z_start);
	glTexCoord2f(0.0, 1.0), glVertex3f(x_end, y_end, z_end);
	glEnd();
	// draw clouds on sky
	for (int i = 0; i < 10; i++)
		drawCloud(cloud_pos[i][0], cloud_pos[i][1], cloud_pos[i][2], cloud_pos[i][3], cloud_pos[i][5]);
}

// draw sand
void drawSand() {
	glBindTexture(GL_TEXTURE_2D, textures[5]);		// choose sand texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(-1500.0, -201.0, 2000.0);
	glTexCoord2f(0.0, 1.0), glVertex3f(-1500.0, -201.0, 1300.0);
	glTexCoord2f(1.0, 1.0), glVertex3f(1500.0, -201.0, 1300.0);
	glTexCoord2f(1.0, 0.0), glVertex3f(1500.0, -201.0, 2000.0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0), glVertex3f(-1500.0, -251.0, 1200.0);
	glTexCoord2f(0.0, 1.0), glVertex3f(-1500.0, -201.0, 1300.0);
	glTexCoord2f(1.0, 1.0), glVertex3f(1500.0, -201.0, 1300.0);
	glTexCoord2f(1.0, 0.0), glVertex3f(1500.0, -251.0, 1200.0);
	glEnd();
	for (int i = -1; i < 2; i += 2) {	// symmetric part
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0), glVertex3f(i * 1600.0, -251.0, 1300.0);
		glTexCoord2f(0.0, 1.0), glVertex3f(i * 1500.0, -201.0, 1300.0);
		glTexCoord2f(1.0, 1.0), glVertex3f(i * 1500.0, -201.0, 2000.0);
		glTexCoord2f(1.0, 0.0), glVertex3f(i * 1600.0, -251.0, 2000.0);
		glEnd();
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0), glVertex3f(i * 1500.0, -251.0, 1200.0);
		glTexCoord2f(1.0, 1.0), glVertex3f(i * 1500.0, -201.0, 1300.0);
		glTexCoord2f(1.0, 0.0), glVertex3f(i * 1600.0, -251.0, 1300.0);
		glEnd();
	}
}

// draw sea
void drawSea() {
	GLfloat specular[] = { 0.5, 0.5, 0.5, 1.0 };	// change the specular of material
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glBindTexture(GL_TEXTURE_2D, textures[4]);		// change texture
	GLfloat sea_z_start = 2250, sea_length = 5000;
	GLfloat sea_tex_size = 50;
	for (int j = -95; j < 5; j++) {
		for (int i = -50; i < 50; i++) {
			GLfloat loc = max(0.0, sqrt(i * i + j * j) - 8);	// color of sea become darker from center to side
			if (day_time <= 1)
				glColor3f(max(80 / 255.0, (163 - loc * 83 / 50.0) / 255.0),
					max(100 / 255.0, (213 - loc * 113 / 50.0) / 255.0),
					max(131 / 255.0, (231 - loc * 100 / 50.0) / 255.0));
			else
				glColor3f(max(0 / 255.0, (163 - loc * 163 / 50.0) / 255.0),
					max(0 / 255.0, (213 - loc * 213 / 50.0) / 255.0),
					max(10 / 255.0, (231 - loc * 231 / 50.0) / 255.0));
			glBegin(GL_QUADS);	// bind texture piece by piece
			glTexCoord2f(0.0, 0.0), glVertex3f(sea_tex_size * i, -252.0, sea_z_start + sea_tex_size * (j + 1));
			glTexCoord2f(0.0, 1.0), glVertex3f(sea_tex_size * i, -252.0, sea_z_start + sea_tex_size * j);
			glTexCoord2f(1.0, 1.0), glVertex3f(sea_tex_size * (i + 1), -252.0, sea_z_start + sea_tex_size * j);
			glTexCoord2f(1.0, 0.0), glVertex3f(sea_tex_size * (i + 1), -252.0, sea_z_start + sea_tex_size * (j + 1));
			glEnd();
		}
	}
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

// draw spindrift using bezier curve
void drawSea_wave() {
	glDisable(GL_TEXTURE_2D);
	GLfloat ctrlPoints[3][3][3] = {
		{{ 1500.0f, -3.0f, -40.0f }, { 1500.0f, 13.0f, 0.0f }, { 1500.0f, 10.0f, 16.0f }},
		{{ 0.0f, -3.0f, -40.0f }, { 0.0f, 13.0f, 0.0f }, { 0.0f, 10.0f, 16.0f }},
		{{ -1500.0f, -3.0f, -40.0f }, { -1500.0f, 13.0f, 0.0f }, { -1500.0f, 10.0f, 16.0f }}
	};	// control points
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 10; i++) {
		glPushMatrix();
		glTranslatef(1000, -245, (i * 500 + sea_wave) % 5000 - 1500);	// set position
		glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
		glMap2f(GL_MAP2_VERTEX_3,	// set map
			0.0f, 10.0f, 3, 3,
			0.0f, 10.0f, 9, 3,
			&ctrlPoints[0][0][0]);
		glEnable(GL_MAP2_VERTEX_3);
		glMapGrid2f(30, 0.0f, 20.0f, 10, 0.0f, 20.0f);	// set grid
		glEvalMesh2(GL_LINE, 0, 10, 0, 10);	// calculate grid
		glPopMatrix();
	}
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

// control lights
void set_lights() {
	// global ambient light
	GLfloat Light_day[] = { 0.7 , 0.7 , 0.6 , 1.0 };
	GLfloat Light_night[] = { 0.3 , 0.3, 0.4 , 1.0 };
	if (time == 0 || day_time == 1)	// day light
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_day);
	else	// night light
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_night);
	// draw a spot light

	pointLight(1500, 1800, -1450);
	// draw a parallel light
	parallelLight();
}

// display the scene
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	lookat_x = 200 * sin(n / 200) + camera_x;
	lookat_z = 200 * cos(n / 200) + camera_z;
	gluLookAt(camera_x, camera_y, camera_z, lookat_x, lookat_y, lookat_z, view_x, view_y, view_z);	// set camera position, look at point and direction of coordinate system 
	// set lights
	set_lights();
	// initialize materials
	GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0 }, diffuse[4] = { 1.0, 1.0, 1.0, 1.0 }, specular[] = { 1, 1, 1, 1.0 };
	GLfloat shininess = 10.0, emission[] = { 0.0, 0.0, 0.0, 1.0 };
	// set initial matrials
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	drawSky();
	drawSand();
	drawSea();
	drawSea_wave();
	drawGuard();
	drawPavilion();
	drawBank();
	drawLawn();

	glutSwapBuffers();
}

// initiating
void initialization(void)
{
	glClearColor(0, 0, 0, 0.0);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrustum(GLdouble_left, GLdouble_right, GLdouble_bottom, GLdouble_top, GLdouble_near, GLdouble_far);		// set the frustum

	glEnable(GL_LIGHTING);	// enable lighting
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// load textures
	string path = "textures/";
	wood_1_data = loadImg(path + "wood_1.bmp");
	wood_2_data = loadImg(path + "wood_2.bmp");
	stone_1_data = loadImg(path + "stone_1.bmp");
	stone_2_data = loadImg(path + "stone_2.bmp");
	sea_data = loadImg(path + "sea.bmp");
	sand_data = loadImg(path + "sand.bmp");
	grass_data = loadImg(path + "grass.bmp");
	word_data = loadImg(path + "word.bmp");
	string sky = "textures/sky_";
	for (int i = 0; i <= 4; i++)
		sky_data[i] = loadImg(sky + to_string(i) + ".bmp");
	string night = "textures/night_";
	for (int i = 0; i <= 4; i++)
		night_data[i] = loadImg(night + to_string(i) + ".bmp");

	// bind and save textures
	glShadeModel(GL_FLAT);		// select flat or smooth shading
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// set texture environment parameters
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Choose the most correct or highest quality option
	glGenTextures(20, textures);	// get texture's id and bind textures

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 200, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, wood_1_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, wood_2_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, stone_1_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 660, 487, 0, GL_RGB, GL_UNSIGNED_BYTE, stone_2_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sea_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 660, 440, 0, GL_RGB, GL_UNSIGNED_BYTE, sand_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 482, 0, GL_RGB, GL_UNSIGNED_BYTE, word_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	for (int i = 0; i < 5; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[8 + i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky_data[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	for (int i = 0; i < 5; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[13 + i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, night_data[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	// set clouds randomly
	for (int i = 0; i < 10; i++) {
		cloud_pos[i][0] = rand() % 5000 - 2500;
		cloud_pos[i][1] = rand() % 500 + 2800;
		cloud_pos[i][2] = rand() % 5000 - 1500;
		cloud_pos[i][3] = rand() % 5 + 4;
		cloud_pos[i][4] = rand() % 5 + 10;
		cloud_pos[i][5] = rand() % 4 / 10.0 + 0.5;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// create the window
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Chuanxin Zhai 3D");

	initialization();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdleFunc);	// 动画
	glutReshapeFunc(myReshapeFunc);
	glutKeyboardFunc(myKeyboardFunc);	
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouseInput);	
	glutMainLoop();
}