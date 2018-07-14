#pragma once
#include <Siv3D.hpp>
class GUI_Button
{
private:
	String print_string;
	bool pressed;
	Rect rect_button;
public:
	void init(int x, int y, int w, int h, String str);
	void draw_button(Font font);
};

