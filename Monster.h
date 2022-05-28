#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <thread>
#include "so2_GRA.h"
#include "Data.h"
#include <vector>
#include <mutex>
using namespace std;


class Monster
{
	GLfloat x, y;
	GLfloat speed=0.2;
	GLint size = 1;
	char direction;
	vector<vector<char>> map;
	flag running = true;

public:
	void run();
	Monster(int x, int y);
	Monster();
	Data getData();
	void executeOrder66();
	bool isRunning();
	

private:
	void move();
	void battle();

};

