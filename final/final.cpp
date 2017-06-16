// final.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <glm.h>
#include <windows.h>
#include <math.h>
#include <imageloader.h>
#include <lodepng.h>

using namespace std;

double camera_x = 0.0, camera_y = 15.0, camera_z = 30.0, item_x = 0.0, item_y = 0.0, item_z = -1.0, head_x = 0.0, head_y = 1.0, head_z = 0.0;
double move_x = 0.0, move_y = 0.0, move_z = 0.0;
bool choosed;

float  table_y = 0; //控制箭頭
#define table_max -0.1 // 箭頭最頂端限制
#define table_min -5 //箭頭最底端限制



#define init_texture 0
#define skill 1 //技能建
#define bagpack 2 //背包
#define diagram 3 //圖鑑
int texture_control = init_texture; // 偵測"案"確定(e)之後，要貼哪一張texture
int direct_which = 0; //指向目前箭頭指向哪一點


GLMmodel *glm_model;
GLuint list_id;

GLfloat light_position[] = { 0.0, 5.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat local_view[] = { 0.0 };
GLuint set_textureId[5];// 用來儲存textureID		0:地板 1:列表 2:周遭 3:天空
BOOLEAN texture_change = TRUE;

int monster_Max_HP[3], monster_Now_HP[3], hero_Max_HP[3], hero_Now_HP[3], hero_Max_MP[3], hero_Now_MP[3];

GLuint load_texture(char* texture_name);
void draw_Monster_heal(int Now_HP, int Max_HP);
void draw_Hero_state();
void draw_floor();
void draw_Table();
void load_monster();
void output(int x, int y, char *string);

GLuint load_texture(char* texture_name){
	unsigned error; //偵測錯誤

	static unsigned char* image; //存圖點的陣列
	unsigned width, height; //圖的寬，高
	error = lodepng_decode32_file(&image, &width, &height, texture_name);
	if (error){
		printf("error %u: %s\n", error, lodepng_error_text(error));
	}
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	return textureId;
}

void draw_Monster_heal(int Now_HP, int Max_HP){
	double percent = Now_HP / Max_HP;
	double new_x = -3 + 6 * percent;

	char HP[100];
	char tmp[100];

	_itoa(Now_HP, HP, 10);
	_itoa(Max_HP, tmp, 10);
	strcat(HP, " / ");
	strcat(HP, tmp);
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glScaled(0.9, 0.9, 0.9);
	output(-5, 7, HP);
	glPopMatrix();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
		glVertex3f(-3.0, 6.0, 0.0);
		glVertex3f(new_x, 6.0, 0.0);
		glVertex3f(new_x, 5.0, 0.0);
		glVertex3f(-3.0, 5.0, 0.0);
	glEnd();
	glColor3f(0.5, 0.5, 0.5);
}

void draw_Hero_state(){
	glDisable(GL_DEPTH_TEST);

	double HP_x[3], MP_x[3];

	for (int i = 0; i < 3; i++)
	{
		HP_x[i] = -55 + 20 * hero_Now_HP[i] / hero_Max_HP[i];
		MP_x[i] = -55 + 20 * hero_Now_MP[i] / hero_Max_MP[i];

		glColor3f(1.0, 0.0, 0.0);
		glTranslated(0.0, -3.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex3f(-55.0, 35.0, 0.0);
		glVertex3f(HP_x[i], 35.0, 0.0);
		glVertex3f(HP_x[i], 33.5, 0.0);
		glVertex3f(-55.0, 33.5, 0.0);
		glEnd();
		glColor3f(0.0, 0.0, 1.0);
		glTranslated(0.0, -2.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex3f(-55.0, 35.0, 0.0);
		glVertex3f(MP_x[i], 35.0, 0.0);
		glVertex3f(MP_x[i], 33.5, 0.0);
		glVertex3f(-55.0, 33.5, 0.0);
		glEnd();
	}

	glEnable(GL_DEPTH_TEST);
}

void draw_background(){

	glEnable(GL_TEXTURE_2D);
	
	/*****Sky****/
	glBindTexture(GL_TEXTURE_2D, set_textureId[3]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 30.0, 50.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 30.0, 50.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(50.0, 30.0, -50.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, 30.0, -50.0);
	glEnd();

	/*****Around****/
	glBindTexture(GL_TEXTURE_2D, set_textureId[2]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, -2.5, 50.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, 30, 50.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, 30, -50.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, -2.5, -50.0);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, -2.5, -50.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, 30, -50.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(50.0, 30, -50.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(50.0, -2.5, -50.0);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0); glVertex3f(50.0, -2.5, -50.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 30, -50.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(50.0, 30, 50.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(50.0, -2.5, 50.0);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0); glVertex3f(50.0, -2.5, -50.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 30, -50.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, 30, -50.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, -2.5, -50.0);
	glEnd();

	/*****Ground****/
	glBindTexture(GL_TEXTURE_2D, set_textureId[0]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, -2.5, -50.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -2.5, 50.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(50.0, -2.5, 50.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(50.0, -2.5, -50.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void draw_table(){
	glDisable(GL_DEPTH_TEST);

	glTranslatef(camera_x, 0, camera_z - 30);
	glTranslatef(0, 0, 21);
	glRotatef(-25, 1, 0, 0);
	glTranslatef(0, 0, -21);
	if (texture_change){// 存取貼圖
		//loadTexture(texture_control);
	}

	glEnable(GL_TEXTURE_2D);

	/******Table*****/
	glBindTexture(GL_TEXTURE_2D, set_textureId[1]);

	glBegin(GL_POLYGON);// 畫選單表+貼貼圖
	glTexCoord2f(0.0, 1 - 0); glVertex3f(-26.5, -5, 21);
	glTexCoord2f(0.0, 1 - 1); glVertex3f(-26.5, 5, 21);
	glTexCoord2f(1.0, 1 - 1); glVertex3f(0, 5, 21);
	glTexCoord2f(1.0, 1 - 0); glVertex3f(0, -5, 21);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	/******Mouse*****/
	if (choosed)
	{
		glColor3f(1.0, 0.0, 0.0);
	}
	else
	{
		glColor3f(0.5, 0.5, 0.5);
	}
	glTranslatef(0, table_y, 0);
	glBegin(GL_TRIANGLES); //箭頭
		glVertex3f(-22, 4, 21);
		glVertex3f(-19, 2.5, 21);
		glVertex3f(-22, 1, 21);
	glEnd();

	glEnable(GL_DEPTH_TEST);
}

void load_monster(){
	glm_model = glmReadOBJ("monster/Gargoyle_1.obj");
	glmUnitize(glm_model);
	glmScale(glm_model, .5);
	glmFacetNormals(glm_model);
	glmVertexNormals(glm_model, 90);

	list_id = glmList(glm_model, GLM_MATERIAL | GLM_SMOOTH);

	glmDelete(glm_model);
}

void Init(){

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	
	///// load ground texture
	set_textureId[0] = load_texture("pic/ground.png");
	set_textureId[1] = load_texture("pic/item2.png");
	set_textureId[2] = load_texture("pic/Mountain.png");
	set_textureId[3] = load_texture("pic/sky.png");
}

void game_init(){
	
	for (int i = 0; i < 3; i++)
	{
		monster_Max_HP[i] = 100;
		monster_Now_HP[i] = monster_Max_HP[i];
		hero_Max_HP[i] = 100;
		hero_Now_HP[i] = hero_Max_HP[i];
		hero_Max_MP[i] = 30;
		hero_Now_MP[i] = hero_Max_MP[i];
	}
}

void loadTexture(int value) {

	//Load the floor texture

	switch (value)
	{
	case 0:
		set_textureId[1] = load_texture("pic/item2.png");
		break;
	case 1:
		set_textureId[1] = load_texture("pic/item.png");
		break;
	case 2:
		set_textureId[1] = load_texture("pic/ground.png");
		break;
	case 3:
		set_textureId[1] = load_texture("pic/item.png");
		break;
	}

	texture_change = false;
}

void output(int x, int y, char *string)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void myDisplay(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.5, 0.5, 0.5);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, camera_z, item_x, item_y, item_z, head_x, head_y, head_z);

	/****Floor*****/
	glPushMatrix();
			draw_background();
	glTranslatef(-10.0, 0.0, 20.0);
	glPopMatrix();

	/****Hero state***/


	glPushMatrix();

	glTranslated(move_x, move_y, move_z);
	glRotated(-25.0, 1.0, 0.0, 0.0);
		draw_Hero_state();
	glPopMatrix();

	/****Battlefield*****/
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
		
		/****Monster*****/
		glPushMatrix();
		glTranslatef(10.0, 0.0, -15.0);
			draw_Monster_heal(monster_Now_HP[0], monster_Max_HP[0]);
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(-10.0, 0.0, 15.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 0.0, -15.0);
			draw_Monster_heal(monster_Now_HP[1], monster_Max_HP[1]);
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(0.0, 0.0, 15.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10.0, 0.0, -15.0);
			draw_Monster_heal(monster_Now_HP[2], monster_Max_HP[2]);
			glutSolidCube(5.0);
			//glCallList(list_id);
		glTranslatef(10.0, 0.0, 15.0);
		glPopMatrix();


		/****Hero*****/
		glPushMatrix();
		glTranslatef(10.0, 0.0, 20.0);
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(-10.0, 0.0, -20.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(0.0, 0.0, -20.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10.0, 0.0, 20.0);
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(10.0, 0.0, -20.0);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		draw_table();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, camera_z, item_x, item_y, item_z, head_x, head_y, head_z);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (GLfloat)w / (GLfloat)h, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, camera_z, item_x, item_y, item_z, head_x, head_y, head_z);

}

void keyPress(unsigned char key, int x, int y){

	switch (key)
	{
	case 'w':
	case 'W':
		if (camera_z > -30)
		{
			move_z -= 0.2;
			camera_z -= 0.2;
			item_z -= 0.2;
		}
		break;
	case 'a':
	case 'A':
		if (camera_x > -30)
		{
			move_x -= 0.2;
			camera_x -= 0.2;
			item_x -= 0.2;
		}
		break;
	case 's':
	case 'S':

		if (camera_z < 30)
		{
			move_z += 0.2;
			camera_z += 0.2;
			item_z += 0.2;
		}
		break;
	case 'd':
	case 'D':
		if (camera_x < 30)
		{
			move_x += 0.2;
			camera_x += 0.2;
			item_x += 0.2;
		}
		break;
	case 'm':
	case 'M':
		
			move_y -= 0.2;
			camera_y -= 0.2;
			item_y -= 0.2;
		break;
	case 'j':
	case 'J':

		move_y += 0.2;
		camera_y += 0.2;
		item_y += 0.2;
		break;
	case '1':
		hero_Now_HP[0]--;
		break;
	case '2':
		hero_Now_HP[1]--;
		break;
	case '3':
		hero_Now_HP[2]--;
		break;
	}
	glutPostRedisplay();
}

int _tmain(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glShadeModel(GL_FLAT);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1600, 800);
	glutCreateWindow("");
	glewInit();
	Init();

	load_monster();
	game_init();

	//glutIdleFunc(spinDisplay);
	glutDisplayFunc(&myDisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
	//glutTimerFunc(100, Timer, 1);
	glutMainLoop();
	return 0;
}

