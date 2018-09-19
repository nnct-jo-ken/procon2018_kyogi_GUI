#pragma once
#include "Main.h"

class Agent;

class Tile
{
public:
	void init(int, int, int);
	void draw(Font);
	void update(State);
	void overArrow(Vec2, Agent*);
	int score = 0;
	int x = 0;
	int y = 0;
	State state = NEUTRAL;
	Rect rect;

	// “¾“_ŒvŽZ
	bool is_sarched = false;
	bool is_end = false;
};

