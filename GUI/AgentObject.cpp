#include "AgentObject.h"


void AgentObject::init() {
	circle = Circle(posx * tile_size + margin + tile_size / 2, posy * tile_size + margin + tile_size / 2, tile_size / 2 - 3);
}

void AgentObject::update() {
	this->draw();
	this->operate();
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
		if (!remove_tile) {
			Line(circle.center, slct_rect.center).draw(3, Palette::Green);
		}
		else {
			Line(circle.center, slct_rect.center).draw(3, Palette::Red);
		}
	}
}

void AgentObject::operate() {
	// エージェントがクリックされたときの処理
	if (circle.mouseOver && !click_mouse_before) {
		if (circle.leftPressed) {
			click_agent = true;
		}
	}

	// マウスのボタンがが離されたときの処理
	if (Input::MouseL.released) {
		click_agent = false;
	}


	if (click_agent) {
		double dist_atm = circle.center.distanceFrom(Mouse::Pos());
		if (dist_atm > 20) {
			decide_step = true;
			remove_tile = false;
			Vec2 sign_vec = Point(this->circle.x, this->circle.y) - Mouse::Pos();
			sign_vec = sign_vec.normalize();
			Line ln = Line(this->circle.x, this->circle.y, this->circle.x - sign_vec.x * 50, this->circle.y - sign_vec.y * 50).draw(2, Palette::Red);
			pointer = Circle(ln.end, 3);
			pointer.draw(Palette::Red);
		}
		else {
			if (decide_step) {
				if (remove_tile) {
					decide_step = false;
					remove_tile = false;
				}
				else {
					remove_tile = true;
				}
				click_agent = false;
			}
		}
	}
	click_mouse_before = Input::MouseL.pressed;
}

void AgentObject::set_next_step() {
	Point step_point = slct_point - Point(posx, posy);
	if (decide_step) {
		if (step_point == Point(0, -1)) {
			next_step = N;
		}
		else if (step_point == Point(1, -1)) {
			next_step = NE;
		}
		else if (step_point == Point(1, 0)) {
			next_step = E;
		}
		else if (step_point == Point(1, 1)) {
			next_step = SE;
		}
		else if (step_point == Point(0, 1)) {
			next_step = S;
		}
		else if (step_point == Point(-1, 1)) {
			next_step = SW;
		}
		else if (step_point == Point(-1, 0)) {
			next_step = W;
		}
		else if (step_point == Point(-1, -1)) {
			next_step = NW;
		}
		else {
			next_step = STAY;
		}
	}
	else {
		next_step = STAY;
	}
	this->click_agent = false;
	this->click_mouse_before = false;
	this->decide_step = false;
	this->remove_tile = false;
}