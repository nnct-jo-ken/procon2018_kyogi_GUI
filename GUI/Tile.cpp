#include "Tile.h"
#include "Agent.h"

void Tile::init(int x_, int y_, int score_) {
	x = x_;
	y = y_;
	score = score_;
	rect = Rect(x * 40 + margin_x, y * 40 + margin_y, 40);
}

void Tile::draw(Font font) {
	if (state == TEAM1) {
		rect.draw(Palette::Skyblue);
	}
	else if (state == TEAM2) {
		rect.draw(Palette::Orange);
	}
	else {
		rect.draw(Palette::White);
	}

	rect.drawFrame(1, 1, Palette::Gray);

	font(score).drawCenter(rect.x + 20, rect.y + 20, Palette::Gray);
}

void Tile::update(State s_) {
	state = s_;
}

void Tile::overArrow(Vec2 v2, Agent *agent) {
	if (v2.intersects(rect)) {
		rect.drawFrame(2, 2, Palette::Gray);
		if (!Input::MouseL.pressed) {
			agent->nStep = Point(this->x - agent->x, this->y - agent->y);
			agent->stepState = MOVE;
		}
	}
}

void Tile::setter_update(Font font) {
	rect.draw(Palette::White);
	rect.drawFrame(1, 1, Palette::Gray);
	font(score).drawCenter(rect.x + 20, rect.y + 20, Palette::Gray);
}