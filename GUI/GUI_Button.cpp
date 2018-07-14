#include "GUI_Button.h"

void GUI_Button::init(int x, int y, int w, int h, String str)
{
	print_string = str;
	pressed = false;
	rect_button = Rect(x, y, w, h);
}

void GUI_Button::draw_button(Font font) {
	if (pressed) {
		pressed = false;
	}
	rect_button.draw(Palette::White);
	if (rect_button.mouseOver) {
		rect_button.draw(Palette::Gray);
	}
	if (rect_button.leftPressed) {
		rect_button.draw(Palette::Black);
	}
	if (rect_button.leftClicked) {
		pressed = true;
	}
	rect_button.drawFrame(0, 2, Palette::Skyblue);
	font(print_string).drawCenter(rect_button.x + rect_button.w / 2, rect_button.y + rect_button.h / 2, Palette::Black);
}