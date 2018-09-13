#pragma once
#include "Main.h"

class Tile;

class Agent
{
public:
	void init(int, int, State);
	void draw();
	void operate(Tile tile[12][12], int, int);
	void drawStep();
	int x = 0;
	int y = 0;
	bool is_remove = false;
	Point nStep = Point(0, 0);
	State state = NEUTRAL;
	StepState stepState = STAY;
	Circle circle;
};

