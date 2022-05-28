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
private:
	void showMap();
	
public:
	void renderMap();
	void insertMap(vector<vector<char>>* map);
	
};

