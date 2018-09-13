#pragma once
#include"Main.h"

class AgentObject
{
public:
	bool click_agent = false;
	bool click_mouse_before = false;
	bool decide_step = false;
	bool remove_tile = false;
	Circle circle;
	Circle pointer;
	Rect slct_rect;
	Point slct_point;
	Step next_step = STAY;
	int posx;
	int posy;
	int state;
	void init();
	void draw();
	void operate();
	void update();
	void set_next_step();
};

