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
	int show_x;
	int show_y;
	State state = NEUTRAL;
	Rect rect;

	// ���_�v�Z
	bool is_searched = false;
	bool is_end = false;
};

