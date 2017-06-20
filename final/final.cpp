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
#include <time.h>

using namespace std;
int over_count = 0;

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
GLuint set_subtextureId[20];// 用來儲存subtextureID		

/*貼圖*/
float  table_y = 0; //控制箭頭
int game_over_table_y = 0;
int game_over_direct_which = 0;
bool attack_screen = false;
bool hero_moving = false;
bool monster_get_damage[3];	// 怪物是否受傷
bool hero_get_damage[3];	// 英雄是否受傷
int damage;
bool monster_round = false;
bool monster_attack_over[3];
double hero_move_dis_X = 0; // 當英雄移動時，控制的x座標
double hero_move_dis_Y = 0; // 當英雄移動時，控制的y座標
double hero_move_dis_Z = 0; // 當英雄移動時，控制的z座標
double hero_move_scale_X = 1; // 當英雄移動時，控制的x倍率
double hero_move_scale_Y = 1; // 當英雄移動時，控制的y倍率
double hero_move_scale_Z = 1; // 當英雄移動時，控制的z倍率
double monster_move_dis_X = 0; // 當怪物移動時，控制的x座標
double monster_move_dis_Y = 0; // 當怪物移動時，控制的y座標
double monster_move_dis_Z = 0; // 當怪物移動時，控制的z座標

/*藥水控制*/
int red_water_number = 3, blue_water_number = 3;
#define red_amount 50; // 紅藥水回復的量
#define blue_amount 80; // 藍藥水回復的量
int red_water_resume_amount = red_amount;
int blue_water_resume_amount = blue_amount;
BOOLEAN powerful = false;
BOOLEAN left_hero_live = true;
BOOLEAN mid_hero_live = true;
BOOLEAN right_hero_live = true;

/*遊戲設置*/
#define init_texture 0
#define skill 1 //技能建
#define bagpack 2 //背包
#define diagram 3 //圖鑑
BOOLEAN bool_diagram = true;
int table_texture_control = init_texture; // 偵測"案"確定(e)之後，要貼哪一張texture
int direct_which = 0; //指向目前箭頭指向哪一點

#define nothing -1;
#define attack 0	//怪物 做 普攻
#define skill_1 1	//怪物 做 技能1
#define skill_2 2	//怪物 做 技能2
#define skill_3 3	//怪物 做 技能3
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

bool victory, judge_game_over;

int monster_Max_HP[3], monster_Now_HP[3], hero_Max_HP[3], hero_Now_HP[3], hero_Max_MP[3], hero_Now_MP[3]
, hero_attack[3], monster_attack[3];
int game_round = 0;
/*技能控制*/
int hero_skill_attack[4];
int hero_skill_blue_consume[4];
#define sub_z -35//英雄z座標-怪物z座標
int z = sub_z;
bool blue_no_enough = false;
/*Function*/
GLuint load_texture(char* texture_name);
void draw_Monster_heal(int Now_HP, int Max_HP);
void draw_Hero_state();
void draw_floor();
void draw_Table();
void draw_Hero_direct();
void draw_Monster_direct();
void draw_Monster_get_damage();
void draw_Hero_get_damage();
void load_monster();
void game_over();
void monster_move(int id);
void output(float x, float y, char *string);


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
	double percent = double(Now_HP) / double(Max_HP);
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
void no_blue_flash(int id){
	glColor3f(0.0, 0.0, 1.0);
	glutPostRedisplay();
}
void draw_Hero_state(){
	glDisable(GL_DEPTH_TEST);

	double HP_x[3], MP_x[3];
	int hero_head = 12; // 12為左英雄的圖片;11為中英雄的圖片;10為右英雄的圖片

	for (int i = 2; i >= 0; i--)
	{
		char s[100], tmp[100];

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
		if (i == direct_hero&&blue_no_enough){

			blue_no_enough = false;
			glColor3f(1, 1, 1);
			glutTimerFunc(100, no_blue_flash, 1);
		}
		glBegin(GL_POLYGON);
		glVertex3f(-55.0, 35.0, 0.0);
		glVertex3f(MP_x[i], 35.0, 0.0);
		glVertex3f(MP_x[i], 33.5, 0.0);
		glVertex3f(-55.0, 33.5, 0.0);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, set_textureId[hero_head--]);
		glBegin(GL_POLYGON); // 畫人物的照片
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-60, 37.4, 0.0);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(-55.5, 37.4, 0.0);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(-55.5, 33.2, 0.0);
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-60, 33.2, 0.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		_itoa(hero_Now_HP[i], s, 10);
		_itoa(hero_Max_HP[i], tmp, 10);
		strcat(s, " / ");
		strcat(s, tmp);
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		output(-35, 35.7, s);
		glPopMatrix();

		_itoa(hero_Now_MP[i], s, 10);
		_itoa(hero_Max_MP[i], tmp, 10);
		strcat(s, " / ");
		strcat(s, tmp);
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		output(-35, 33.7, s);
		glPopMatrix();
	}

	glEnable(GL_DEPTH_TEST);
}

void draw_background(){

	glEnable(GL_TEXTURE_2D);

	/*****Sky****/
	glBindTexture(GL_TEXTURE_2D, set_textureId[3]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0 - 1.1); glVertex3f(-50.0, 30.0, 50.0);
	glTexCoord2f(1.0, 1.0 - 1.0); glVertex3f(50.0, 30.0, 50.0);
	glTexCoord2f(1.0, 1.0 - 0.0); glVertex3f(50.0, 30.0, -50.0);
	glTexCoord2f(0.0, 1.0 - 0.0); glVertex3f(-50.0, 30.0, -50.0);
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
	glEnable(GL_TEXTURE_2D);
	glTranslatef(camera_x, 0, camera_z - 30);




	glTranslatef(0, 0, 21);
	glRotatef(-25, 1, 0, 0);
	glTranslatef(0, 0, -21);





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





	if (table_texture_control == bagpack){

		/*藥水數量*/
		switch (red_water_number)// 看目前箭頭只到哪一個地方以及要貼哪一個貼圖
		{
		case 3:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[0]);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[1]);
			break;
		case 1:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[2]);
			break;
		case 0:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[3]);
			break;
		}

		//glBindTexture(GL_TEXTURE_2D, set_subtextureId[0]);

		glBegin(GL_POLYGON);// 
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-11.6, 1.8, 21);
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-11.6, 4.3, 21);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(-9.2, 4.3, 21);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(-9.2, 1.8, 21);
		glEnd();

		switch (blue_water_number)// 看目前箭頭只到哪一個地方以及要貼哪一個貼圖
		{
		case 3:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[0]);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[1]);
			break;
		case 1:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[2]);
			break;
		case 0:
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[3]);
			break;
		}

		glBegin(GL_POLYGON);// 
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-11.6, -0.6, 21);
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-11.6, 1.9, 21);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(-9.2, 1.9, 21);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(-9.2, -0.6, 21);
		glEnd();
		if (powerful){
			glBindTexture(GL_TEXTURE_2D, set_subtextureId[4]);
			glBegin(GL_POLYGON);// 
			glTexCoord2f(0.0, 1 - 0); glVertex3f(-19.3, -3, 21);
			glTexCoord2f(0.0, 1 - 1); glVertex3f(-19.3, -0.5, 21);
			glTexCoord2f(1.0, 1 - 1); glVertex3f(-8.9, -0.5, 21);
			glTexCoord2f(1.0, 1 - 0); glVertex3f(-8.9, -3, 21);
			glEnd();
		}
	}
	///////////////////////////////////
	glPushMatrix();
	//glTranslatef();
	glRotatef(25, 1, 0, 0);
	glTranslatef(0, 0, 22);
	glRotatef(-25, 1, 0, 0);
	glTranslatef(0, 0, -22);
	if (judge_game_over){
		glBindTexture(GL_TEXTURE_2D, set_textureId[13]);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-15, 10, 22);
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-15, 30, 22);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(15, 30, 22);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(15, 10, 22);

		glEnd();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, set_textureId[7]);
		game_over_table_y = game_over_direct_which*-6;
		glTranslatef(0, game_over_table_y, 0);

		glBegin(GL_POLYGON); //箭頭
		glTexCoord2f(0.0, 1 - 0); glVertex3f(-9.5, 22.3, 22);
		glTexCoord2f(0.0, 1 - 1); glVertex3f(-9.5, 24.3, 22);
		glTexCoord2f(1.0, 1 - 1); glVertex3f(-3.5, 24.3, 22);
		glTexCoord2f(1.0, 1 - 0); glVertex3f(-3.5, 22.3, 22);
		glEnd();
	}
	glPopMatrix();
	/////////////////////////////


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

void draw_Monster_get_damage(){
	char tmp[100];
	_itoa(damage, tmp, 10);
	glColor3f(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslated(0.0, 0.0, 2.0);
	output(-2, 9, tmp);
	glPopMatrix();
	glColor3f(0.5, 0.5, 0.5);
}

void draw_Hero_get_damage(){
	char tmp[100];
	_itoa(-monster_attack[direct_monster], tmp, 10);
	glColor3f(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslated(0.0, 0.0, 2.0);
	output(0, 5, tmp);
	glPopMatrix();
	glColor3f(0.5, 0.5, 0.5);
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
	set_textureId[0] = load_texture("pic/grass.png");
	set_textureId[1] = load_texture("pic/first.png");	// table 畫面
	set_textureId[2] = load_texture("pic/landscape.png");
	set_textureId[3] = load_texture("pic/cloud.png");
	set_textureId[4] = load_texture("pic/skill.png");	// 技能 畫面
	set_textureId[5] = load_texture("pic/bag.png");		// 背包 畫面
	set_textureId[6] = load_texture("pic/diagram.png");	// 圖鑑 畫面
	set_textureId[7] = load_texture("pic/arrow.png");	// 箭頭 
	set_textureId[8] = load_texture("pic/Hero_direct.png"); // 英雄頭上的綠箭頭
	set_textureId[9] = load_texture("pic/Monster_direct.png"); // 敵人頭上的紅箭頭
	set_textureId[10] = load_texture("pic/left_hero.png"); // 右英雄圖片
	set_textureId[11] = load_texture("pic/mid_hero.png"); //中英雄圖片
	set_textureId[12] = load_texture("pic/right_hero.png"); // 左英雄圖片
	set_textureId[13] = load_texture("pic/game_over.png"); // 左英雄圖片
	set_subtextureId[0] = load_texture("pic/x3.png"); // 藥水X0圖片
	set_subtextureId[1] = load_texture("pic/x2.png"); // 藥水X1圖片
	set_subtextureId[2] = load_texture("pic/x1.png"); // 藥水X2圖片
	set_subtextureId[3] = load_texture("pic/x0.png"); // 藥水X3圖片
	set_subtextureId[4] = load_texture("pic/xxxxx.png"); // 無敵藥水
}

void game_init(){
	direct_hero = right_hero;
	judge_game_over = false;
	victory = false;
	powerful = false;

	for (int i = 0; i < 3; i++)
	{
		monster_Max_HP[i] = 100;
		monster_Now_HP[i] = monster_Max_HP[i];
		hero_Max_HP[i] = 100;
		hero_Now_HP[i] = hero_Max_HP[i];
		hero_Max_MP[i] = 200;
		hero_Now_MP[i] = hero_Max_MP[i];
		monster_attack[i] = 10 + over_count * 5;
		hero_get_damage[i] = false;
		monster_get_damage[i] = false;
		monster_attack_over[i] = false;
	}
	hero_attack[0] = 20;
	hero_attack[1] = 20;
	hero_attack[2] = 20;

	hero_skill_attack[skill_1] = 60;
	hero_skill_attack[skill_2] = 70;
	hero_skill_attack[skill_3] = 80;

	hero_skill_blue_consume[skill_1] = 50;
	hero_skill_blue_consume[skill_2] = 65;
	hero_skill_blue_consume[skill_3] = 120;

	monster_move_dis_X = 0;
	monster_move_dis_Y = 0; 
	monster_move_dis_Z = 0;
}


void round_over(){

	game_round++;

	if (hero_Now_HP[0] == 0 && hero_Now_HP[1] == 0 && hero_Now_HP[2] == 0)
	{
		victory = false;
		judge_game_over = true;
		game_over();
	}
	else if (monster_Now_HP[0] == 0 && monster_Now_HP[1] == 0 && monster_Now_HP[2] == 0)
	{
		victory = true;
		judge_game_over = true;
		game_over();
	}
	else
	{
		table_texture_control = init_texture;
		direct_which = 0;

		direct_hero--;

		while (hero_Now_HP[direct_hero] == 0)
		{
			game_round++;
			direct_hero--;
		}

		hero_do = nothing;

		if (game_round % 3 == 0)
		{
			monster_round = true;
			do
			{
				direct_hero = rand() % 3;
			} while (hero_Now_HP[direct_hero] == 0);

			glutTimerFunc(20, monster_move, NULL);
		}
	}
}

void game_over(){
	game_round = 0;
	monster_round = false;
	over_count++;
	//glEnable(GL_TEXTURE_2D);



	//glBindTexture(GL_TEXTURE_2D, set_textureId[13]);

	//glDisable(GL_TEXTURE_2D);
	if (victory)
	{
		printf("你真棒");
	}
	else
	{
		printf("你真爛");
	}

}

void output(float x, float y, char *string)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void damage_to_monster(int to_damage){

	damage = -to_damage;

	if (monster_Now_HP[direct_monster] - to_damage <= 0)
	{
		monster_Now_HP[direct_monster] = 0;
	}
	else
	{
		monster_Now_HP[direct_monster] -= to_damage;
	}
}

void damage_to_hero(int to_damage){

	if (hero_Now_HP[direct_hero] - to_damage <= 0)
	{
		hero_Now_HP[direct_hero] = 0;
	}
	else
	{
		hero_Now_HP[direct_hero] -= to_damage;
	}
}
bool detect_use_skill(int which_skill){
	if (hero_Now_MP[direct_hero] - hero_skill_blue_consume[which_skill] < 0)//無法使用技能
	{
		blue_no_enough = true;
		//hero_Now_MP[direct_hero] = 0;
		return false;
	}
	else
	{
		return true;
		//hero_Now_MP[direct_hero] -= blue;
	}
}
void consume_blue(int which_skill){
	hero_Now_MP[direct_hero] -= hero_skill_blue_consume[which_skill];
}
void monster_move(int id){

	bool over = false;

	if (monster_Now_HP[2] != 0 && !monster_attack_over[2])
	{
		direct_monster = right_monster;
		static bool clock = true;

		if (hero_Now_HP[0] != 0 || hero_Now_HP[1] != 0 || hero_Now_HP[2] != 0)
		{

			if (clock)
			{
				if (monster_move_dis_Z < 25)
				{
					monster_move_dis_X += (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z += 1;
				}
				else if (monster_move_dis_Z == 25)
				{
					clock = false;
					hero_get_damage[direct_hero] = true;
					damage_to_hero(monster_attack[direct_monster]);
				}
			}
			else
			{
				if (monster_move_dis_Z > 0)
				{
					monster_move_dis_X -= (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z -= 1;
				}
				else if (monster_move_dis_Z == 0)
				{
					hero_get_damage[direct_hero] = false;

					if (monster_Now_HP[0] == 0 && monster_Now_HP[1] == 0)
					{
						over = true;
						direct_hero = right_hero;
						while (hero_Now_HP[direct_hero] == 0)
						{
							game_round++;
							direct_hero--;
						}

						for (int i = 0; i < 3; i++)
						{
							monster_attack_over[i] = false;
						}
						monster_round = false;
					}
					else
					{
						do
						{
							direct_hero = rand() % 3;
						} while (hero_Now_HP[direct_hero] == 0);
						monster_attack_over[2] = true;
					}
					clock = true;
				}
			}
		}
		else
		{
			clock = true;
			over = true;
			victory = false;
			judge_game_over = true;
			game_over();
		}
	}
	else if (monster_Now_HP[1] != 0 && !monster_attack_over[1])
	{
		direct_monster = middle_monster;
		static bool clock = true;

		if (hero_Now_HP[0] != 0 || hero_Now_HP[1] != 0 || hero_Now_HP[2] != 0)
		{

			if (clock)
			{
				if (monster_move_dis_Z < 25)
				{
					monster_move_dis_X += (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z += 1;
				}
				else if (monster_move_dis_Z == 25)
				{
					clock = false;
					hero_get_damage[direct_hero] = true;
					damage_to_hero(monster_attack[direct_monster]);
				}
			}
			else
			{
				if (monster_move_dis_Z > 0)
				{
					monster_move_dis_X -= (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z -= 1;
				}
				else if (monster_move_dis_Z == 0)
				{
					hero_get_damage[direct_hero] = false;

					if (monster_Now_HP[0] == 0)
					{
						over = true;
						direct_hero = right_hero;
						while (hero_Now_HP[direct_hero] == 0)
						{
							game_round++;
							direct_hero--;
						}
						for (int i = 0; i < 3; i++)
						{
							monster_attack_over[i] = false;
						}
						monster_round = false;
					}
					else
					{
						do
						{
							direct_hero = rand() % 3;
						} while (hero_Now_HP[direct_hero] == 0);
						monster_attack_over[1] = true;
					}
					clock = true;
				}
			}
		}
		else
		{
			clock = true;
			victory = false;
			over = true;
			judge_game_over = true;
			game_over();
		}
	}
	else if (monster_Now_HP[0] != 0 && !monster_attack_over[0])
	{
		direct_monster = left_monster;
		static bool clock = true;

		if (hero_Now_HP[0] != 0 || hero_Now_HP[1] != 0 || hero_Now_HP[2] != 0)
		{

			if (clock)
			{
				if (monster_move_dis_Z < 25)
				{
					monster_move_dis_X += (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z += 1;
				}
				else if (monster_move_dis_Z == 25)
				{
					clock = false;
					hero_get_damage[direct_hero] = true;
					damage_to_hero(monster_attack[direct_monster]);
				}
			}
			else
			{
				if (monster_move_dis_Z > 0)
				{
					monster_move_dis_X -= (double(direct_hero - direct_monster)) * 10 / 25;
					monster_move_dis_Z -= 1;
				}
				else if (monster_move_dis_Z == 0)
				{
					hero_get_damage[direct_hero] = false;
					clock = true;
					over = true;
					direct_hero = right_hero;
					while (hero_Now_HP[direct_hero] == 0)
					{
						game_round++;
						direct_hero--;
					}
					for (int i = 0; i < 3; i++)
					{
						monster_attack_over[i] = false;
					}
					monster_round = false;
				}
			}
		}
		else
		{
			clock = true;
			victory = false;
			over = true;
			judge_game_over = true;
			game_over();
		}
	}

	if (!over)
	{
		glutTimerFunc(20, monster_move, 0);
	}

	glutPostRedisplay();

}
void detect_hero_blood(){
	if (hero_Now_HP[direct_hero] >= (hero_Max_HP[direct_hero] - red_water_resume_amount))
	{
		hero_Now_HP[direct_hero] = hero_Max_HP[direct_hero];
	}
	else
	{
		hero_Now_HP[direct_hero] += red_water_resume_amount;
	}
	red_water_number -= 1;

}
void hero_move(int id)
{
	static bool clock = true;
	bool over = false;
	static bool has_sub_blue = false; //偵測是否扣過魔力
	switch (id)
	{
	case 0: // 普攻
		if (clock){
			if (hero_move_dis_Z > -25)
			{
				hero_move_dis_X += (double(direct_monster - direct_hero)) * 10 / 25;
				hero_move_dis_Z -= 1;
			}
			else if (hero_move_dis_Z == -25)
			{
				clock = false;
				monster_get_damage[direct_monster] = true;
				damage_to_monster(hero_attack[direct_hero]);
			}
		}
		else{

			if (hero_move_dis_Z < 0)
			{
				hero_move_dis_X -= (double(direct_monster - direct_hero)) * 10 / 25;
				hero_move_dis_Z += 1;
			}
			else if (hero_move_dis_Z == 0)
			{
				monster_get_damage[direct_monster] = false;
				clock = true;
				over = true;
			}
		}

		if (!over)
		{
			glutTimerFunc(33, hero_move, 0);
		}
		else
		{
			hero_moving = false;
			attack_screen = false;
			hero_do = -1;

			round_over();
		}
		break;
	case 1: // 技能1
		if (!has_sub_blue){//耗磨
			consume_blue(skill_1);
			has_sub_blue = true;
		}

		if (has_sub_blue){
			if (clock){
				if (!has_sub_blue){

					has_sub_blue = true;
				}
				if (hero_move_dis_Y < 100)
				{
					hero_move_scale_X = 3;
					hero_move_scale_Y = 3;
					hero_move_scale_Z = 3;
					hero_move_dis_Y += 4;
				}
				else if (hero_move_dis_Y == 100)
				{


					hero_move_dis_X += (direct_monster - direct_hero) * 10;
					hero_move_dis_Z = hero_move_dis_Z - 35;
					clock = false;
				}
			}
			else{
				if (hero_move_dis_Y > 0)
				{
					hero_move_dis_Y -= 4;
				}
				else if (hero_move_dis_Y == 0)
				{
					if (hero_move_dis_Z == -35){// 已經飛到敵人身上 且 距離最遠即z最小的時候造成傷害
						monster_get_damage[direct_monster] = true;
						damage_to_monster(hero_skill_attack[skill_1]);//技能2造成傷害
					}

					if (hero_move_dis_Z < 0)
					{
						hero_move_scale_X -= double(1.0 * 2.0 / 35.0);
						hero_move_scale_Y -= double(1.0 * 2.0 / 35.0);
						hero_move_scale_Z -= double(1.0 * 2.0 / 35.0);
						hero_move_dis_X -= (double(direct_monster - direct_hero)) * 10 / 35;
						hero_move_dis_Z += 1;
					}
					else if (hero_move_dis_Z == 0)
					{
						hero_move_scale_X = double(1.0);
						hero_move_scale_Y = double(1.0);
						hero_move_scale_Z = double(1.0);
						monster_get_damage[direct_monster] = false;
						clock = true;
						over = true;
					}

				}
			}

			if (!over)
			{
				glutTimerFunc(33, hero_move, 1);
			}
			else
			{
				has_sub_blue = false;
				hero_moving = false;
				attack_screen = false;
				hero_do = nothing;
				round_over();
			}
		}
		break;
	case 2: // 技能2
		if (!has_sub_blue){//耗磨
			consume_blue(skill_2);
			has_sub_blue = true;
		}

		if (has_sub_blue){
			if (clock){
				if (!has_sub_blue){

					has_sub_blue = true;
				}
				if (hero_move_dis_Y < 100)
				{
					hero_move_dis_Y += 4;
				}
				else if (hero_move_dis_Y == 100)
				{


					hero_move_dis_X += (direct_monster - direct_hero) * 10;
					hero_move_dis_Z = hero_move_dis_Z - 35;
					clock = false;
				}
			}
			else{
				if (hero_move_dis_Y > 0)
				{
					hero_move_dis_Y -= 4;
				}
				else if (hero_move_dis_Y == 0)
				{
					if (hero_move_dis_Z == -35){// 已經飛到敵人身上 且 距離最遠即z最小的時候造成傷害
						monster_get_damage[direct_monster] = true;
						damage_to_monster(hero_skill_attack[skill_2]);//技能2造成傷害
					}

					if (hero_move_dis_Z < 0)
					{
						hero_move_dis_X -= (double(direct_monster - direct_hero)) * 10 / 35;
						hero_move_dis_Z += 1;
					}
					else if (hero_move_dis_Z == 0)
					{
						monster_get_damage[direct_monster] = false;
						clock = true;
						over = true;
					}

				}
			}

			if (!over)
			{
				glutTimerFunc(33, hero_move, 2);
			}
			else
			{
				has_sub_blue = false;
				hero_moving = false;
				attack_screen = false;
				hero_do = nothing;
				round_over();
			}
		}
		break;
	case 3: // 技能3
		if (!has_sub_blue){//耗磨
			consume_blue(skill_3);
			has_sub_blue = true;
		}

		if (has_sub_blue){
			if (clock){
				if (!has_sub_blue){

					has_sub_blue = true;
				}


				if (hero_move_dis_Z > -15)
				{
					hero_move_dis_X += (double(direct_monster - direct_hero)) * 10 / 25;
					hero_move_dis_Z -= 0.5;
				}
				else if (hero_move_dis_Z == -15)
				{
					clock = false;
					monster_get_damage[direct_monster] = true;
					damage_to_monster(50);
					red_water_number += 1;
					detect_hero_blood();
				}
			}
			else{

				if (hero_move_dis_Z < 0)
				{
					hero_move_dis_X -= (double(direct_monster - direct_hero)) * 10 / 25;
					hero_move_dis_Z += 0.5;
				}
				else if (hero_move_dis_Z == 0)
				{
					monster_get_damage[direct_monster] = false;
					clock = true;
					over = true;
				}
			}

			if (!over)
			{
				glutTimerFunc(33, hero_move, 3);
			}
			else
			{
				hero_moving = false;
				attack_screen = false;
				hero_do = -1;

				round_over();
			}
		}
		break;

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
	if (monster_round && direct_monster == right_monster)
	{
		glTranslatef(monster_move_dis_X, monster_move_dis_Y, monster_move_dis_Z); //怪物移動
	}
	if (monster_Now_HP[2] != 0)
	{
		draw_Monster_heal(monster_Now_HP[2], monster_Max_HP[2]);
		if (attack_screen && direct_monster == right_monster)
		{
			//PlaySound(TEXT("sound/choose.wav"), NULL, SND_ASYNC);
			draw_Monster_direct();
		}
		glutSolidCube(5.0);
		//glCallList(list_id);
	}
	if (monster_get_damage[2])
	{
		draw_Monster_get_damage();
	}
	
	glTranslatef(-10.0, 0.0, 15.0);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0, 0.0, -15.0);
	if (monster_round && direct_monster == middle_monster)
	{
		glTranslatef(monster_move_dis_X, monster_move_dis_Y, monster_move_dis_Z); //怪物移動
	}
	if (monster_Now_HP[1] != 0)
	{
		draw_Monster_heal(monster_Now_HP[1], monster_Max_HP[1]);
		if (attack_screen && direct_monster == middle_monster)
		{
			//PlaySound(TEXT("sound/choose.wav"), NULL, SND_ASYNC);
			draw_Monster_direct();
		}
		glutSolidCube(5.0);
		//glScaled(15.0, 5.0, 5.0);
		//glCallList(list_id);
	}
	if (monster_get_damage[1])
	{
		draw_Monster_get_damage();
	}
	
	glTranslatef(0.0, 0.0, 15.0);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(-10.0, 0.0, -15.0);
	if (monster_round && direct_monster == left_monster)
	{
		glTranslatef(monster_move_dis_X, monster_move_dis_Y, monster_move_dis_Z); //怪物移動
	}
	if (monster_Now_HP[0] != 0)
	{
		draw_Monster_heal(monster_Now_HP[0], monster_Max_HP[0]);
		if (attack_screen && direct_monster == left_monster)
		{
			//PlaySound(TEXT("sound/choose.wav"), NULL, SND_ASYNC);
			draw_Monster_direct();
		}
		glutSolidCube(5.0);
	}
	if (monster_get_damage[0])
	{
		draw_Monster_get_damage();
	}
	//glCallList(list_id);
	glTranslatef(10.0, 0.0, 15.0);
	glPopMatrix();


	/****Hero*****/


	//glBindTexture(GL_TEXTURE_2D, set_textureId[10]);
	glColor3f(0.5, 0.5, 0.5);
	if (hero_Now_HP[2] != 0)
	{
		glPushMatrix();
		glTranslatef(10.0, 0.0, 20.0);
		if (direct_hero == right_hero && !monster_round){
			glTranslatef(hero_move_dis_X, hero_move_dis_Y, hero_move_dis_Z); //英雄移動

			draw_Hero_direct();
			glScalef(hero_move_scale_X, hero_move_scale_Y, hero_move_scale_Z);
		}
		if (hero_get_damage[2])
		{
			draw_Hero_get_damage();
		}
		glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(-10.0, 0.0, -20.0);
		glPopMatrix();
	}

	if (hero_Now_HP[1] != 0)
	{
		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);
		if (direct_hero == middle_hero && !monster_round){
			glTranslatef(hero_move_dis_X, hero_move_dis_Y, hero_move_dis_Z);
			draw_Hero_direct();
			glScalef(hero_move_scale_X, hero_move_scale_Y, hero_move_scale_Z);
		}
		if (hero_get_damage[1])
		{
			draw_Hero_get_damage();
		}
		glutSolidCube(5.0);
		//glCallList(list_id);
		glTranslatef(0.0, 0.0, -20.0);
		glPopMatrix();
	}

	if (hero_Now_HP[0] != 0)
	{
		glPushMatrix();
		glTranslatef(-10.0, 0.0, 20.0);
		if (direct_hero == left_hero && !monster_round){
			glTranslatef(hero_move_dis_X, hero_move_dis_Y, hero_move_dis_Z);
			draw_Hero_direct();
			glScalef(hero_move_scale_X, hero_move_scale_Y, hero_move_scale_Z);
		}
		if (hero_get_damage[0])
		{
			draw_Hero_get_damage();
		}
		glutSolidCube(5.0);
		//glCallList(list_id);
		glPopMatrix();
	}
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

void detect_hero_blue(){


	if (hero_Now_MP[direct_hero] >= (hero_Max_MP[direct_hero] - blue_water_resume_amount)){
		hero_Now_MP[direct_hero] = hero_Max_MP[direct_hero];
	}
	else{
		hero_Now_MP[direct_hero] += blue_water_resume_amount;
	}
	blue_water_number -= 1;

}



void detect_key_Enter(){
	if (!judge_game_over){
		if (!monster_round)
		{
			if (attack_screen)
			{
				hero_moving = true;
				switch (hero_do)
				{
				case 0:
					glutTimerFunc(33, hero_move, 0);
					break;
				case 1:
					glutTimerFunc(33, hero_move, 1);
					break;
				case 2:
					glutTimerFunc(33, hero_move, 2);
					break;
				case 3:
					glutTimerFunc(33, hero_move, 3);
					break;
				}

				//monster_do = attack;
				//hero_do = attack;

			}
			else
			{

				if (table_texture_control == init_texture){ // 在首頁欄
					if (direct_which == 0){
						hero_do = attack;
						attack_screen = true;
						while (monster_Now_HP[direct_monster] == 0){
							direct_monster = (direct_monster + 1) % 3;
						}
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
						direct_which = 3;
						bool_diagram = false;
						table_texture_control = diagram;// 切換到圖鑑texture
					}
				}
				else if (table_texture_control == skill){ // 在技能欄
					if (direct_which == 0 && detect_use_skill(skill_1)){
						//第一招技能
						attack_screen = true;
						monster_do = skill_1;
						hero_do = skill_1;
						while (monster_Now_HP[direct_monster] == 0){
							direct_monster = (direct_monster + 1) % 3;
						}
					}
					else if (direct_which == 1 && detect_use_skill(skill_2))
					{
						//第二招技能
						attack_screen = true;
						monster_do = skill_2;
						hero_do = skill_2;
						while (monster_Now_HP[direct_monster] == 0){
							direct_monster = (direct_monster + 1) % 3;
						}
					}
					else if (direct_which == 2 && detect_use_skill(skill_3)){
						//第三招技能
						attack_screen = true;
						monster_do = skill_3;
						hero_do = skill_3;
						while (monster_Now_HP[direct_monster] == 0){
							direct_monster = (direct_monster + 1) % 3;
						}
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
						if (red_water_number > 0){
							detect_hero_blood();
							round_over();
						}
						printf("紅藥水量 = %d\n", red_water_number);
						//紅藥水
					}
					else if (direct_which == 1)
					{
						if (blue_water_number > 0){
							detect_hero_blue();
							round_over();
						}
						printf("藍藥水量 = %d\n", blue_water_number);
						//藍藥水
					}
					else if (direct_which == 2){
						//無敵藥水
						if (powerful == false){
							for (int i = 2; i >= 0; i--)
							{

								hero_Max_HP[i] = 500000;
								hero_Now_HP[i] = 500000;
								hero_Max_MP[i] = 500000;
								hero_Now_MP[i] = 500000;

							}
							powerful = true;
							round_over();
						}
						printf("你已經按了無敵鍵\n");
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
						bool_diagram = true;
						table_texture_control = init_texture;
					}

				}
			}
		}
	}
	else{
		if (game_over_direct_which == 0){
			game_init();//重新執行的韓式
		}
		else{
			exit(0);
		}
	}
}

void keyPress(unsigned char key, int x, int y){

	switch (key)
	{
	case 13: //Enter key
		if (!monster_round & !hero_moving)
		{
			detect_key_Enter();
		}
		break;
	}
	glutPostRedisplay();
}

void select_table(GLint key, GLint x, GLint y){

	switch (key)
	{
	case GLUT_KEY_UP:
		if (!attack_screen && !hero_moving && bool_diagram && !monster_round && !judge_game_over)
		{
			direct_which = (direct_which + 3) % 4;
			PlaySound(TEXT("sound/choose_table.wav"), NULL, SND_ASYNC);
		}
		else if (!attack_screen && !hero_moving && bool_diagram && judge_game_over){
			game_over_direct_which = 0;
			PlaySound(TEXT("sound/choose_table.wav"), NULL, SND_ASYNC);
		}
		break;
	case GLUT_KEY_DOWN:
		if (!attack_screen && !hero_moving && bool_diagram && !monster_round && !judge_game_over)
		{
			direct_which = (direct_which + 1) % 4;
			PlaySound(TEXT("sound/choose_table.wav"), NULL, SND_ASYNC);
		}
		else if (!attack_screen && !hero_moving && bool_diagram && judge_game_over){
			game_over_direct_which = 1;
			PlaySound(TEXT("sound/choose_table.wav"), NULL, SND_ASYNC);
		}
		break;
	case GLUT_KEY_LEFT:
		if (attack_screen && !hero_moving && !monster_round && !judge_game_over)
		{
			do{
				direct_monster = (direct_monster + 2) % 3;
			} while (monster_Now_HP[direct_monster] == 0);
			PlaySound(TEXT("sound/choose_enemy.wav"), NULL, SND_ASYNC);
		}
		break;
	case GLUT_KEY_RIGHT:
		if (attack_screen && !hero_moving && !monster_round && !judge_game_over)
		{
			do
			{
				direct_monster = (direct_monster + 1) % 3;
			} while (monster_Now_HP[direct_monster] == 0);
			PlaySound(TEXT("sound/choose_enemy.wav"), NULL, SND_ASYNC);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int _tmain(int argc, char* argv[])
{
	srand(time(NULL));
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

	glutDisplayFunc(&myDisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(select_table);
	//glutTimerFunc(100, Timer, 1);

	glutMainLoop();
	return 0;
}

