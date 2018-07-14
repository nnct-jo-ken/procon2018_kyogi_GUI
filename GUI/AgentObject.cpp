#include "AgentObject.h"


void AgentObject::init() {
	circle = Circle(posx * tile_size + margin + tile_size / 2, posy * tile_size + margin + tile_size / 2, tile_size / 2 - 3);
}

void AgentObject::update() {
	this->draw();
	this->operate();
	Println(slct_rect.x);
}

void AgentObject::draw() {
	switch (state)
	{
	case 1:
		circle.draw(Palette::Skyblue);
		break;
	case 2:
		circle.draw(Palette::Orange);
		break;
	default:
		circle.draw(Palette::Black);
	}
	circle.drawFrame(0, 1, Palette::Black);
	if (decide_step) {
		Line(circle.center, slct_rect.center).draw(3, Palette::Green);
		Println(slct_point);
	}
}

void AgentObject::operate() {
	// エージェントがクリックされたときの処理
	if (circle.mouseOver && !click_mouse_before) {
		if (circle.leftPressed) {
			click_agent = true;
			decide_step = false;
		}
	}

	// マウスのボタンがが離されたときの処理
	if (Input::MouseL.released) {
		click_agent = false;
	}

	if (click_agent) {
		Vec2 sign_vec = Point(this->circle.x, this->circle.y) - Mouse::Pos();
		sign_vec = sign_vec.normalize();
		Line ln = Line(this->circle.x, this->circle.y, this->circle.x - sign_vec.x * 50, this->circle.y - sign_vec.y * 50).draw(2, Palette::Red);
		pointer = Circle(ln.end, 3);
		pointer.draw(Palette::Red);
	}
	click_mouse_before = Input::MouseL.pressed;
}