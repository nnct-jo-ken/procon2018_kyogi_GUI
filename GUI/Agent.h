#pragma once
#include "Main.h"

class Tile;

class Agent
{
public:
	void init(int, int, State);
	void draw(Font font, Tile tile[12][12]);
	void operate(Tile tile[12][12], int, int);
	void drawStep(Tile tile[12][12]);
	void update();
	void setter_update(Font, Tile[12][12]);
	Point getpos();
	int x = 0;
	int y = 0;
	int show_x;
	int show_y;
	int id = 0;
	bool flag = false;
	Point nStep = Point(0, 0);
	Point aiStep = Point(0, 0);
	Point deletePoint = Point(0, 0);
	State state = NEUTRAL;
	StepState stepState = STAY;
	Circle circle;
};

