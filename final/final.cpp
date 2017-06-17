	// final.cpp : Defines the entry point for the console application.
	//

#include "stdafx.h"
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <glm.h>
#include <math.h>
#include <imageloader.h>
#include <lodepng.h>
#include <Windows.h>
#include <MMSystem.h>

using namespace std;

/*環境設置*/
double camera_x = 0.0, camera_y = 15.0, camera_z = 30.0, item_x = 0.0, item_y = 0.0, item_z = -1.0, head_x = 0.0, head_y = 1.0, head_z = 0.0;
double move_x = 0.0, move_y = 0.0, move_z = 0.0;
GLMmodel *glm_model;
GLuint list_id;
GLfloat light_position[] = { 0.0, 5.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat local_view[] = { 0.0 };
GLuint set_textureId[20];// 用來儲存textureID		0:地板 1:列表 2:周遭 3:天空 4:

/*貼圖*/
float  table_y = 0; //控制箭頭
bool attack_screen = false;
bool hero_moving = false;
int hero_move_dis = 0;


/*遊戲設置*/
#define init_texture 0
#define skill 1 //技能建
#define bagpack 2 //背包
#define diagram 3 //圖鑑
int table_texture_control = init_texture; // 偵測"案"確定(e)之後，要貼哪一張texture
int direct_which = 0; //指向目前箭頭指向哪一點

#define nothing 0;
#define attack 1	//怪物 做 普攻
#define skill_1 2	//怪物 做 技能1
#define skill_2 3	//怪物 做 技能2
#define skill_3 4	//怪物 做 技能3
int monster_do = nothing;	// 怪物目前的動作
int hero_do = nothing;		// 英雄目前的動作


#define right_hero 2
#define middle_hero 1
#define left_hero 0
int direct_hero = right_hero; // 目前你指向哪一個英雄

#define right_monster 2
#define middle_monster 1
#define left_monster 0
int direct_monster = right_monster; // 目前你指向哪一個怪獸

int monster_Max_HP[3], monster_Now_HP[3], hero_Max_HP[3], hero_Now_HP[3], hero_Max_MP[3], hero_Now_MP[3];
int game_round = 1;

/*Function*/
GLuint load_texture(char* texture_name);
void draw_Monster_heal(int Now_HP, int Max_HP);
void draw_Hero_state();
void draw_floor();
void draw_Table();
void draw_Hero_direct();
void draw_Monster_direct();
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
	glEnable(GL_TEXTURE_2D);


	switch (table_texture_control)// 看目前箭頭只到哪一個地方以及要貼哪一個貼圖
	{
	case 0:
		glBindTexture(GL_TEXTURE_2D, set_textureId[1]);
		break;
	case 1:
		glBindTexture(GL_TEXTURE_2D, set_textureId[4]);
		break;
	case 2:
		glBindTexture(GL_TEXTURE_2D, set_textureId[5]);
		break;
	case 3:
		glBindTexture(GL_TEXTURE_2D, set_textureId[6]);
		break;
	}


	/******Table*****/


	glBegin(GL_POLYGON);// 畫選單表+貼貼圖
	glTexCoord2f(0.0, 1 - 0); glVertex3f(-26.5, -6.5, 21);
	glTexCoord2f(0.0, 1 - 1); glVertex3f(-26.5, 6.5, 21);
	glTexCoord2f(1.0, 1 - 1); glVertex3f(-8.1, 6.5, 21);
	glTexCoord2f(1.0, 1 - 0); glVertex3f(-8.1, -6.5, 21);
	glEnd();


	glDisable(GL_TEXTURE_2D);

	/******Mouse*****/
	table_y = direct_which * -2.3;
	glTranslatef(0, table_y, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, set_textureId[7]);
	glBegin(GL_POLYGON); //箭頭
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-22.5, 2.3, 21);
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-22.5, 3.3, 21);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(-19.5, 3.3, 21);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(-19.5, 2.3, 21);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

void draw_Hero_direct(){
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, set_textureId[8]);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.45, 1 - 0.9); glVertex3f(-0.5, 12, 0);
			glTexCoord2f(0.55, 1 - 0.9); glVertex3f(0.5, 12, 0);
			glTexCoord2f(0.8, 1 - 0.8); glVertex3f(1, 10, 0);
			glTexCoord2f(0.5, 1 - 0.2); glVertex3f(0, 3, 0);
			glTexCoord2f(0.2, 1 - 0.8); glVertex3f(-1, 10, 0);
		glEnd();

		glBegin(GL_POLYGON);
			glTexCoord2f(0.45, 1 - 0.9); glVertex3f(0, 12, 0.5);
			glTexCoord2f(0.55, 1 - 0.9); glVertex3f(0, 12, -0.5);
			glTexCoord2f(0.8, 1 - 0.8); glVertex3f(0, 10, -1);
			glTexCoord2f(0.5, 1 - 0.2); glVertex3f(0, 3, 0);
			glTexCoord2f(0.2, 1 - 0.8); glVertex3f(0, 10, 1);
		glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void draw_Monster_direct(){

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, set_textureId[9]);
	glTranslated(0.0, 5.0, 0.0);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.45, 1 - 0.9); glVertex3f(-0.5, 12, 0);
			glTexCoord2f(0.55, 1 - 0.9); glVertex3f(0.5, 12, 0);
			glTexCoord2f(0.8, 1 - 0.8); glVertex3f(1, 10, 0);
			glTexCoord2f(0.5, 1 - 0.2); glVertex3f(0, 3, 0);
			glTexCoord2f(0.2, 1 - 0.8); glVertex3f(-1, 10, 0);
		glEnd();

		glBegin(GL_POLYGON);
			glTexCoord2f(0.45, 1 - 0.9); glVertex3f(0, 12, 0.5);
			glTexCoord2f(0.55, 1 - 0.9); glVertex3f(0, 12, -0.5);
			glTexCoord2f(0.8, 1 - 0.8); glVertex3f(0, 10, -1);
			glTexCoord2f(0.5, 1 - 0.2); glVertex3f(0, 3, 0);
			glTexCoord2f(0.2, 1 - 0.8); glVertex3f(0, 10, 1);
		glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
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
	set_textureId[1] = load_texture("pic/first.png");	// table 畫面
	set_textureId[2] = load_texture("pic/Mountain.png");
	set_textureId[3] = load_texture("pic/sky.png");
	set_textureId[4] = load_texture("pic/skill.png");	// 技能 畫面
	set_textureId[5] = load_texture("pic/bag.png");		// 背包 畫面
	set_textureId[6] = load_texture("pic/image.png");	// 圖鑑 畫面
	set_textureId[7] = load_texture("pic/arrow.png");	// 箭頭 
	set_textureId[8] = load_texture("pic/Hero_direct.png");
	set_textureId[9] = load_texture("pic/Monster_direct.png");

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

void round_over(){
	hero_do = nothing;
	game_round++;
	table_texture_control = init_texture;
	
	if (direct_hero != left_hero)
	{
		direct_hero--;
	}

	if (game_round % 3 == 0)
	{
		
	}
}

void game_over(){
	game_round = 0;
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

void hero_move(int id)
{
	static bool clock = true;
	bool over = false;

	if (clock){
		if (hero_move_dis > -25)
		{
			hero_move_dis -= 1;
		}
		else if (hero_move_dis == -25)
		{
			clock = false;
		}
	}
	else{
		if (hero_move_dis < 0)
		{
			hero_move_dis += 1;
		}
		else if (hero_move_dis == 0)
		{
			clock = true;
			over = true;
		}
	}

	if (!over)
	{
		glutTimerFunc(33, hero_move, 1);
	}
	else
	{
		hero_moving = false;
		attack_screen = false;
		round_over();
	}

	glutPostRedisplay();
}

void myDisplay(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
			if (attack_screen && direct_monster == right_monster)
			{
				draw_Monster_direct();
			}
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(-10.0, 0.0, 15.0);
		glPopMatrix();

		glPushMatrix();

		glTranslatef(0.0, 0.0, -15.0);
			draw_Monster_heal(monster_Now_HP[1], monster_Max_HP[1]);
			if (attack_screen && direct_monster == middle_monster)
			{
				draw_Monster_direct();
			}
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(0.0, 0.0, 15.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10.0, 0.0, -15.0);
			draw_Monster_heal(monster_Now_HP[2], monster_Max_HP[2]);
			if (attack_screen && direct_monster == left_monster)
			{
				draw_Monster_direct();
			}
			glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(10.0, 0.0, 15.0);
		glPopMatrix();


		/****Hero*****/
		glPushMatrix();

		glTranslatef(10.0, 0.0, 20.0);
		if (direct_hero == right_hero){
			draw_Hero_direct();
			glTranslatef(0, 0, hero_move_dis); //英雄移動
		}
			glutSolidCube(5.0);
			//glCallList(list_id);
		glTranslatef(-10.0, 0.0, -20.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);
		if (direct_hero == middle_hero){
			glTranslatef(0, 0, hero_move_dis);
			draw_Hero_direct();
		}
			glutSolidCube(5.0);
			//glCallList(list_id);
		glTranslatef(0.0, 0.0, -20.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10.0, 0.0, 20.0);
		if (direct_hero == left_hero){
			draw_Hero_direct();
			glTranslatef(0, 0, hero_move_dis);
		}
			glutSolidCube(5.0);
			//glCallList(list_id);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		draw_table();
	glPopMatrix();
	
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

void detect_key_Enter(){
	if (attack_screen)
	{
		hero_moving = true;
		//monster_do = attack;
		//hero_do = attack;
		glutTimerFunc(33, hero_move, 1);
	}else if (!hero_moving)
	{
		if (table_texture_control == init_texture){ // 在首頁欄
			if (direct_which == 0){
				attack_screen = true;
			}
			else if (direct_which == 1)
			{
				direct_which = 0;
				table_texture_control = skill; // 切換到技能texture
			}
			else if (direct_which == 2){
				direct_which = 0;
				table_texture_control = bagpack;// 切換到背包texture
			}
			else if (direct_which == 3){
				direct_which = 0;
				table_texture_control = diagram;// 切換到圖鑑texture
			}
		}
		else if (table_texture_control == skill){ // 在技能欄
			if (direct_which == 0){
				//第一招技能
				monster_do = skill_1;
				hero_do = skill_1;
			}
			else if (direct_which == 1)
			{
				//第二招技能
				monster_do = skill_2;
				hero_do = skill_2;
			}
			else if (direct_which == 2){
				//第三招技能
				monster_do = skill_3;
				hero_do = skill_3;
			}
			else if (direct_which == 3){

				table_texture_control = 0;
				monster_do = nothing;
				hero_do = nothing;
				direct_which = 1;
			}

		}
		else if (table_texture_control == bagpack){ // 在背包欄
			if (direct_which == 0){

			}
			else if (direct_which == 1)
			{

			}
			else if (direct_which == 2){

			}
			else if (direct_which == 3){
				//返回鍵 回到首頁texture
				direct_which = 2;
				table_texture_control = init_texture;
			}

		}
		else if (table_texture_control == diagram){ // 在圖鑑欄
			if (direct_which == 0){

			}
			else if (direct_which == 1)
			{

			}
			else if (direct_which == 2){

			}
			else if (direct_which == 3){
				//返回鍵 回到首頁texture
				direct_which = 3;
				table_texture_control = init_texture;
			}

		}
	}
}

void keyPress(unsigned char key, int x, int y){

	switch (key)
	{
	case 13: //Enter key
		detect_key_Enter();
		break;
	}
	glutPostRedisplay();
}

void select_table(GLint key, GLint x, GLint y){
	
	switch (key)
	{
	case GLUT_KEY_UP:
		if (!attack_screen && !hero_moving)
		{
			direct_which = (direct_which + 3) % 4;
		}
		break;
	case GLUT_KEY_DOWN:
		if (!attack_screen && !hero_moving)
		{
			direct_which = (direct_which + 1) % 4;
		}
		break;
	case GLUT_KEY_LEFT:
		if (attack_screen && !hero_moving)
		{
			direct_monster = (direct_monster + 2) % 3;
			PlaySound(TEXT("sound/choose.wav"), NULL, SND_ASYNC);
		}
		break;
	case GLUT_KEY_RIGHT:
		if (attack_screen && !hero_moving)
		{
			direct_monster = (direct_monster + 1) % 3;
			PlaySound(TEXT("sound/choose.wav"), NULL, SND_ASYNC);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int _tmain(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glShadeModel(GL_FLAT);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
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
	glutSpecialFunc(select_table);
	//glutTimerFunc(100, Timer, 1);
	glutMainLoop();
	return 0;
}

