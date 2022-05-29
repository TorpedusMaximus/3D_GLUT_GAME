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

class MapRenderer
{
	vector<vector<char>>* map;
	vector<vector<float>> colors = {
		{1,1,1},
		{1,0,1},
		{1,1,0},
		{0,1,1},
		{1,0,0},
		{0,1,0},
		{0,0,1},
		{0,0,0}
	};
	
	vector<vector<float>> currentColors = {
		{1,1,1},
		{1,0,1},
		{1,1,0},
		{0,1,1}
	};
	
private:
	void showMap();
	
public:
	void renderMap();
	void insertMap(vector<vector<char>>* map);
	void wildChange();
	
};

