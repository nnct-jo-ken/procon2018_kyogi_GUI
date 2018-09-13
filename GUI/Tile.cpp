#include "Tile.h"
#include "Agent.h"

void Tile::init(int x_, int y_, int score_) {
	x = x_;
	y = y_;
	score = score_;
	rect = Rect(x * 40 + 50, y * 40 + 50, 40);
}

void Tile::draw(Font font) {
	if (state == TEAM1) {
		rect.draw(Palette::Orange);
	}
	else if (state == TEAM2) {
		rect.draw(Palette::Skyblue);
	}
	else {
		rect.draw(Palette::White);
	}

	rect.drawFrame(1, 1, Palette::Black);

	font(score).drawCenter(x * 40 + 50 + 20, y * 40 + 50 + 20, Palette::Black);
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