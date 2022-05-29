#include "MapRenderer.h"

void MapRenderer::showMap()
{
	vector<vector<char>> showmap = *map;
	for (auto row : showmap) {
		for (auto cell : row) {
			cout << cell;
		}
		cout << endl;
	}
}

void MapRenderer::renderMap()
{
	int wallHeight = 3;
	int wallWidth = 2;
	vector<vector<char>> drawMap = *map;
	for (int i = 0; i < drawMap.size(); i++) {
		for (int ii = 0; ii < drawMap[i].size(); ii++) {
			char cell = drawMap[i][ii];
			if (cell == '#') {


				point3 point0 = { i * wallWidth , 0 , ii * wallWidth };
				point3 point1 = { (i + 1) * wallWidth , 0 , ii * wallWidth };
				point3 point2 = { (i + 1) * wallWidth , 0,  (ii + 1) * wallWidth };
				point3 point3_ = { i * wallWidth , 0 , (ii + 1) * wallWidth };
				point3 point4 = { i * wallWidth , wallHeight , ii * wallWidth };
				point3 point5 = { (i + 1) * wallWidth , wallHeight , ii * wallWidth };
				point3 point6 = { (i + 1) * wallWidth , wallHeight , (ii + 1) * wallWidth };
				point3 point7 = { i * wallWidth , wallHeight , (ii + 1) * wallWidth };

				if (ii != 0) {
					glColor3f(currentColors[0][0], currentColors[0][1], currentColors[0][2]);
					glBegin(GL_QUADS); //east side
					glVertex3fv(point0);
					glVertex3fv(point4);
					glVertex3fv(point5);
					glVertex3fv(point1);
					glEnd();
				}

				if (i != drawMap.size() - 1) {
					glColor3f(currentColors[1][0], currentColors[1][1], currentColors[1][2]);
					glBegin(GL_QUADS); //north side
					glVertex3fv(point1);
					glVertex3fv(point5);
					glVertex3fv(point6);
					glVertex3fv(point2);
					glEnd();
				}

				if (ii != drawMap.size() - 1) {
					glColor3f(currentColors[2][0], currentColors[2][1], currentColors[2][2]);
					glBegin(GL_QUADS); //west side
					glVertex3fv(point2);
					glVertex3fv(point6);
					glVertex3fv(point7);
					glVertex3fv(point3_);
					glEnd();
				}

				if (i != 0) {
					glColor3f(currentColors[3][0], currentColors[3][1], currentColors[3][2]);
					glBegin(GL_QUADS); //south side
					glVertex3fv(point3_);
					glVertex3fv(point7);
					glVertex3fv(point4);
					glVertex3fv(point0);
					glEnd();
				}


			}
		}
	}

	point3 ceiling1 = { 0 , wallHeight , 0 };
	point3 ceiling2 = { 0 , wallHeight , map->size() * wallWidth };
	point3 ceiling3 = { map->size() * wallWidth , wallHeight , map->size() * wallWidth };
	point3 ceiling4 = { map->size() * wallWidth , wallHeight , 0 };

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS); //ceiling
	glVertex3fv(ceiling1);
	glVertex3fv(ceiling2);
	glVertex3fv(ceiling3);
	glVertex3fv(ceiling4);
	glEnd();

	point3 floor1 = { 0 , 0 , 0 };
	point3 floor2 = { 0 , 0 , map->size() * wallWidth };
	point3 floor3 = { map->size() * wallWidth , 0 , map->size() * wallWidth };
	point3 floor4 = { map->size() * wallWidth , 0 , 0 };

	glBegin(GL_QUADS); //floor
	glVertex3fv(floor1);
	glVertex3fv(floor2);
	glVertex3fv(floor3);
	glVertex3fv(floor4);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS); //east side
	glVertex3fv(floor1);
	glVertex3fv(ceiling1);
	glVertex3fv(ceiling2);
	glVertex3fv(floor2);
	glEnd();

	glBegin(GL_QUADS); //north side
	glVertex3fv(floor2);
	glVertex3fv(ceiling2);
	glVertex3fv(ceiling3);
	glVertex3fv(floor3);
	glEnd();

	glBegin(GL_QUADS); //west side
	glVertex3fv(floor3);
	glVertex3fv(ceiling3);
	glVertex3fv(ceiling4);
	glVertex3fv(floor4);
	glEnd();

	glBegin(GL_QUADS); //south side
	glVertex3fv(floor4);
	glVertex3fv(ceiling4);
	glVertex3fv(ceiling1);
	glVertex3fv(floor1);
	glEnd();

	int finish;

	switch (drawMap.size()) {
	case 11:
		finish = 5;
		break;
	case 21:
		finish = 12;
		break;
	case 31:
		finish = 15;
		break;
	}


	point3 finish1 = { (map->size() - 1) * wallWidth , 0.001 , finish * wallWidth };
	point3 finish2 = { map->size() * wallWidth , 0.001 , finish * wallWidth };
	point3 finish3 = { map->size() * wallWidth , 0.001 , (finish + 1) * wallWidth };
	point3 finish4 = { (map->size() - 1) * wallWidth , 0.001 , (finish + 1) * wallWidth };


	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS); //finish
	glVertex3fv(finish1);
	glVertex3fv(finish2);
	glVertex3fv(finish3);
	glVertex3fv(finish4);
	glEnd();

}

void MapRenderer::insertMap(vector<vector<char>>* map)
{
	this->map = map;
	cout << "Map loaded" << endl;
}

void MapRenderer::wildChange()
{
	currentColors[0] = colors[rand() % colors.size()];
	currentColors[1] = colors[rand() % colors.size()];
	currentColors[2] = colors[rand() % colors.size()];
	currentColors[3] = colors[rand() % colors.size()];
}
