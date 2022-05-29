#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <thread>
#include <string>
#include <vector>
#include "so2_GRA.h"
#include "Monster.h"
#include "Data.h"
#include <mutex>
#include "MapRenderer.h"
using namespace std;



std::mutex mtx;

MapRenderer mapRenderer;

vector<Monster*> monsters;
vector<thread> threads;
int monstersNumber = 1;
const int n = 50;

vector<vector<char>>* map;
int mapsize;
int spawnFinish[2][2];

point3** points;//siatka punktow
point3** vectors;//wektory punktow powierzchni jajka

static GLfloat viewer[] = { 1.0, 1.5, 0.5 , 1.0, 1.5 , 1.0 };//sterowanie kamera
static GLint gamer[2] = { 0, 5 };
static GLfloat azymuth = 0;
static GLfloat elevation = 0;
char direction = 'w';
const GLfloat speed = 0.2;

static int x_pos_old = 0;//szczegoly sterowania kamera
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;

static GLfloat pix2angle;
static GLint statusMiddle = 0;//statusy 
static GLint statusLeft = 0;
static GLint statusRight = 0;
void RenderScene(void);

void axes(void)
{
	point3  x_min = { 0.0, 0.0, 0.0 };//wspolrzedne koncow osi
	point3  x_max = { 50.0, 0.0, 0.0 };
	point3  y_min = { 0.0, 0.0, 0.0 };
	point3  y_max = { 0.0,  50.0, 0.0 };
	point3  z_min = { 0.0, 0.0, 0.0 };
	point3  z_max = { 0.0, 0.0,  50.0 };

	glColor3f(1.0f, 0.0f, 0.0f);//os x
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);//os y
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);//os z
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void printData() {//wypisanie parametrow symulacji na ekranie czas, predkosc, wspolrzedne kamery
	const int characters = 180;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1000, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2i(10, 970);
	char s[characters];
	sprintf_s(s, " %c", direction);
	for (int i = 0; i < characters; ++i) {
		if (s[i] == ';') {
			break;
		}
		glColor3f(1, 1, 1);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
	}

	glRasterPos2i(10, 950);
	sprintf_s(s, "position x=%f, y=%f", viewer[0], viewer[2]);
	for (int i = 0; i < characters; ++i) {
		if (s[i] == ';') {
			break;
		}
		glColor3f(1, 1, 1);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
	}

	glRasterPos2i(10, 930);
	sprintf_s(s, "position x=%d, y=%d", gamer[0], gamer[1]);
	for (int i = 0; i < characters; ++i) {
		if (s[i] == ';') {
			break;
		}
		glColor3f(1, 1, 1);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
	}


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void egg(Data data) {
	GLfloat x = data.x;
	GLfloat y = data.y;
	float size = data.size / 20.0;
	float u = 0, v = 0;
	float diff = 1.0 / n; //n - liczba punktow na powierzchni jajka
	glTranslated(0, (-(160 * pow(0.5, 4) - 320 * pow(0.5, 3) + 160 * pow(0.5, 2)) / 2) * size, 0);//obnizenie środka figury do centrum ukladu wspolrzednych

	for (int i = 0; i <= n; i++) {//punkty powierzchni
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {//obliczenie wspolrzednych punktow
			points[i][ii][0] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * cos(M_PI * v)) * size;
			points[i][ii][1] = (160 * u4 - 320 * u3 + 160 * u2) * size;
			points[i][ii][2] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * sin(M_PI * v)) * size;
			v = v + diff;
		}
		u = u + diff;
	}

	u = 0;
	for (int i = 0; i <= n; i++) {//wektory normalne
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {
			point9 vector;
			vector[0] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * cos(M_PI * v);
			vector[1] = M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * sin(M_PI * v);
			vector[2] = 640 * u3 - 960 * u2 + 320 * u;
			vector[3] = 0;
			vector[4] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * sin(M_PI * v);
			vector[5] = -1 * M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * cos(M_PI * v);

			vector[6] = vector[2] * vector[5] - vector[4] * vector[3];//wektory 
			vector[7] = vector[4] * vector[1] - vector[0] * vector[5];
			vector[8] = vector[0] * vector[3] - vector[2] * vector[1];
			float vectorSize = sqrt(pow(vector[6], 2) + pow(vector[7], 2) + pow(vector[8], 2));//normalizacja wektora
			if (vectorSize == 0) {
				vectorSize = 1;
			}

			vectors[i][ii][0] = vector[6] / vectorSize;
			vectors[i][ii][1] = vector[7] / vectorSize;
			vectors[i][ii][2] = vector[8] / vectorSize;

			v = v + diff;
		}
		u = u + diff;
	}

	u = 0;
	glTranslated(x, 0.5, y);
	for (int i = 0; i < n; i++) {//rysowanie 
		v = 0;
		for (int ii = 0; ii < n; ii++) {
			float pozycja[4];

			pozycja[0] = sin(M_PI * u);
			pozycja[1] = sin(M_PI * (u + diff));
			pozycja[2] = v;
			pozycja[3] = v + diff;


			if (i < n / 2) {
				glCullFace(GL_BACK);
			}
			else {
				glCullFace(GL_FRONT);
			}
			glBegin(GL_TRIANGLES);//rysowanie pierwszego trojkata
			glNormal3fv(vectors[i][ii]);
			glTexCoord2f(pozycja[0], pozycja[2]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i + 1][ii]);
			glTexCoord2f(pozycja[1], pozycja[2]);
			glVertex3f(points[i + 1][ii][0], points[i + 1][ii][1], points[i + 1][ii][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glTexCoord2f(pozycja[1], pozycja[3]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();


			if (i < n / 2) {
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
			glBegin(GL_TRIANGLES);//rysowanie drugiego trojkata
			glNormal3fv(vectors[i][ii]);
			glTexCoord2f(pozycja[0], pozycja[2]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i][ii + 1]);
			glTexCoord2f(pozycja[0], pozycja[3]);
			glVertex3f(points[i][ii + 1][0], points[i][ii + 1][1], points[i][ii + 1][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glTexCoord2f(pozycja[1], pozycja[3]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();
			v = v + diff;
		}
		u = u + diff;
	}
	glTranslated(-x, -0.5, -y);
}

void turn(bool side) {
	float d = M_PI / 40;
	for (int i = 20; i > 0; i--) {
		if (side) {
			azymuth += d;
		}
		else {
			azymuth -= d;
		}

		viewer[3] = 10 * cos(azymuth) * cos(elevation) + viewer[0];//ustalenie pozycji puktu na ktory patrzymy
		viewer[4] = 10 * sin(elevation) + viewer[1];
		viewer[5] = 10 * sin(azymuth) * cos(elevation) + viewer[2];
		RenderScene();
	}

}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2], viewer[3], viewer[4], viewer[5], 0.0, 1.0, 0.0);//ustawienie kamery
	printData();

	mapRenderer.renderMap();

	for (Monster* monster : monsters) {
		Data data = monster->getData();
		egg(data);
	}

	glutSwapBuffers();//rysoawnie wygenerowanej sceny na ekranie
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal prawy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusRight = 1;        //ustawienie flagi przycisku   
	}
	else {
		statusRight = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal lewy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusLeft = 1;    //ustawienie flagi przycisku        
	}
	else {
		statusLeft = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal scroll
		x_pos_old = x;
		y_pos_old = y;
		statusMiddle = 1;    //ustawienie flagi przycisku    
	}
	else {
		statusMiddle = 0;		//ustawienie flagi przycisku  
	}


	RenderScene();
}

void exitGame() {
	for (auto monster : monsters) {
		monster->executeOrder66();
	}
	cout << "Monsters executed" << endl;

	for (auto& thread : threads) {
		thread.join();
	}
	cout << "Threads joined" << endl;

	monsters.resize(0);
	cout << "Ready to exit" << endl;
	exit(0);
}

bool checkAbility(flag directionFlag)
{//check if can move
	if (directionFlag) {//front
		switch (direction) {
		case 'n':
			if ((*map)[gamer[0]][gamer[1] + 1] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 's':
			if (gamer[1] < 1) {
				return false;
			}
			if ((*map)[gamer[0]][gamer[1] - 1] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 'e':
			if ((*map)[gamer[0] - 1][gamer[1]] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 'w':
			if ((*map)[gamer[0] + 1][gamer[1]] == ' ') {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {  //back
		switch (direction) {
		case 'n':
			if ((*map)[gamer[0]][gamer[1] - 1] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 's':
			if ((*map)[gamer[0]][gamer[1] + 1] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 'e':
			if ((*map)[gamer[0] + 1][gamer[1]] == ' ') {
				return true;
			}
			else {
				return false;
			}
		case 'w':
			if (gamer[0] < 1) {
				return false;
			}
			if ((*map)[gamer[0] - 1][gamer[1]] == ' ') {
				return true;
			}
			else {
				return false;
			}
		}
	}
}

void winCondition() {
	if (gamer[0] == spawnFinish[1][0] && gamer[1] == spawnFinish[1][1]) {
		cout << "You won!!!!" << endl;
		exitGame();
	}
}

void keys(unsigned char key, int x, int y)
{
	mapRenderer.wildChange();
	int tickRate = 10;
	(*map)[gamer[0]][gamer[1]] = ' ';
	switch (key) {
	case 'w':
		if (!checkAbility(true)) {
			break;
		}
		if (direction == 'n') {
			gamer[1] += 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[2] += speed;
				viewer[5] += speed;
				RenderScene();
			}
		}
		if (direction == 's') {
			gamer[1] -= 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[2] -= speed;
				viewer[5] -= speed;
				RenderScene();
			}
		}
		if (direction == 'e') {
			gamer[0] -= 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[0] -= speed;
				viewer[3] -= speed;
				RenderScene();
			}
		}
		if (direction == 'w') {
			gamer[0] += 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[0] += speed;
				viewer[3] += speed;
				RenderScene();
			}
		}
		break;
	case 's':
		if (!checkAbility(false)) {
			break;
		}
		if (direction == 'n') {
			gamer[1] -= 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[2] -= speed;
				viewer[5] -= speed;
				RenderScene();
			}
		}
		if (direction == 's') {
			gamer[1] += 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[2] += speed;
				viewer[5] += speed;
				RenderScene();
			}
		}
		if (direction == 'e') {
			gamer[0] += 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[0] += speed;
				viewer[3] += speed;
				RenderScene();
			}
		}
		if (direction == 'w') {
			gamer[0] -= 1;
			for (int i = 0; i < tickRate; i++) {
				viewer[0] -= speed;
				viewer[3] -= speed;
				RenderScene();
			}

		}
		break;
	case 'a':
		switch (direction) {
		case 'n':
			direction = 'w';
			break;
		case 's':
			direction = 'e';
			break;
		case 'e':
			direction = 'n';
			break;
		case 'w':
			direction = 's';
			break;
		}
		turn(false);
		break;
	case 'd':
		switch (direction) {
		case 'n':
			direction = 'e';
			break;
		case 's':
			direction = 'w';
			break;
		case 'e':
			direction = 's';
			break;
		case 'w':
			direction = 'n';
			break;
		}
		turn(true);
		break;
	case 'q':
		exitGame();
		break;
	}

	(*map)[gamer[0]][gamer[1]] = 'G';

	winCondition();

	RenderScene();
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;//zmiana pozycji x
	x_pos_old = x;

	delta_y = y - y_pos_old;//zmiana pozycji y
	y_pos_old = y;

	glutPostRedisplay();
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;//usatwienie wielkosci do obracania obiektu
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.6, 0.4, 100000.0);

	if (horizontal <= 1.6 * vertical)
		glViewport(0, (vertical - horizontal / 1.6) / 2, 1.6 * vertical, vertical);

	else
		glViewport((horizontal - 1.6 * vertical) / 2, 0, 1.6 * vertical, vertical);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void runMonster(Monster* monster) {
	while (monster->isRunning()) {
		monster->run();
	}
}

void createThreads() {
	for (int i = 0; i < monstersNumber; i++) {
		Monster* monster = new Monster(10, 10);
		monsters.push_back(monster);
	}
	cout << "Monsters generated" << endl;

	for (int i = 0; i < monstersNumber; i++) {
		threads.push_back(thread(runMonster, monsters[i]));
	}
	cout << "Threads started" << endl;

}

bool loadFile(string filename, int mapsize) {
	fstream file;
	file.open(filename, ios::in);
	if (file.good()) {
		std::string line;

		map = new vector<vector<char>>(mapsize, vector<char>(mapsize, ' '));

		for (int i = 0; i < mapsize; i++) {
			getline(file, line);
			for (int j = 0; j < mapsize; j++) {
				(*map)[i][j] = line[j];
			}
		}

		return true;

	}
	else {
		cout << "<Map not found" << endl;
		Sleep(500);
		return false;
	}

}

void loadMap() {
	flag loaded;
	cout << "Difficulty:" << endl;
	cout << "1 - easy" << endl;
	cout << "2 - medium" << endl;
	cout << "3 - hard" << endl;
	int difficulty = 1;
	//cin >> difficulty;
	switch (difficulty) {
	case 1:
		monstersNumber = 4;
		loaded = loadFile("maze5.txt", 11);
		mapsize = 11;
		viewer[2] = 11;
		viewer[5] = 11;
		(*map)[0][5] = 'G';
		gamer[0] = 0;
		gamer[1] = 5;
		spawnFinish[0][0] = 0;
		spawnFinish[0][1] = 5;
		spawnFinish[1][0] = mapsize - 1;
		spawnFinish[1][1] = 5;
		break;
	case 2:
		monstersNumber = 8;
		loaded = loadFile("maze10.txt", 21);
		mapsize = 21;
		viewer[2] = 21;
		viewer[5] = 21;
		(*map)[0][10] = 'G';
		gamer[0] = 0;
		gamer[1] = 12;
		spawnFinish[0][0] = 0;
		spawnFinish[0][1] = 12;
		spawnFinish[1][0] = mapsize - 1;
		spawnFinish[1][1] = 12;
		break;
	case 3:
		monstersNumber = 12;
		loaded = loadFile("maze39.txt", 31);
		mapsize = 31;
		viewer[2] = 31;
		viewer[5] = 31;
		(*map)[0][15] = 'G';
		gamer[0] = 0;
		gamer[1] = 15;
		spawnFinish[0][0] = 0;
		spawnFinish[0][1] = 15;
		spawnFinish[1][0] = mapsize - 1;
		spawnFinish[1][1] = 15;
		break;
	}

	if (!loaded) {
		system("cls");
		loadMap();
	}

	mapRenderer.insertMap(map);
}

int main(void)
{
	loadMap();
	//createThreads();

	//points = new  point3 * [n + 1];//tablica punktow
	//vectors = new point3 * [n + 1];//tablice
	//for (int i = 0; i <= n; i++) {
	//	points[i] = new point3[n + 1];
	//	vectors[i] = new point3[n + 1];
	//}


	srand(time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1600, 1000);
	glutCreateWindow("Uciekaj!!!!!");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(Mouse);//"lapanie" akcji na przyciskach myszy
	glutMotionFunc(Motion);//"lapanie" ruchu myszki
	glutKeyboardFunc(keys);//"lapanie" akcji na klawiaturze
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}