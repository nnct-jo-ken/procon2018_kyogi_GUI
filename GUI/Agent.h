#pragma once
#include "Main.h"

class Tile;

class Agent
{
public:
	void init(int, int, State);
	void draw(Font font);
	void operate(Tile tile[12][12], int, int);
	void drawStep();
	void update();
	Point getpos();
	int x = 0;
	int y = 0;
	int id = 0;
	bool is_ai = false;
	Point nStep = Point(0, 0);
	Point aiStep = Point(0, 0);
	Point deletePoint = Point(0, 0);
	State state = NEUTRAL;
	StepState stepState = STAY;
	Circle circle;
};

