#pragma once
#include <Siv3D.hpp>
#include "Main.h"
#include "AgentObject.h"

class TileObject
{
public:
	Rect rect;
	int posx = 0;
	int posy = 0;
	int state = 0;
	int point = -100000;
	bool before_over = false;
	bool before_select = false;
	void init(int _posx, int _posy);
	void draw(Font font, AgentObject  * agent);
	void update(Font font, AgentObject * agent);
};

