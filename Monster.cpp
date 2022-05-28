#include "Monster.h"



Monster::Monster(int startx, int starty)
{
	x = startx;
	y = starty;
	direction = 'n';
}

Monster::Monster()
{
	size = 1;
	x = 10;
	y = 10;
}

Data Monster::getData()
{
	Data data;
	data.size = size;
	data.x = x;
	data.y = y;
	return data;
}

void Monster::executeOrder66()
{
	//mtx.lock();
	running = false;
	cout << running << endl;
	//mtx.unlock();
}

bool Monster::isRunning()
{
	return running;
}

void Monster::run()
{
	x += speed;
	Sleep(200);
	
}

void Monster::move()
{
	
}

void Monster::battle()
{
	
}
