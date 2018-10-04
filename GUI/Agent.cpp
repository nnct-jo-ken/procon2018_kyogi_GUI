#include "Agent.h"
#include "Tile.h"

void Agent::init(int x_, int y_, State s_) {
	x = x_;
	y = y_;
	state = s_;
	circle = Circle(x * 40 + margin_x + 20, y * 40 + margin_y + 20, 15);
}

void Agent::draw(Font font, Tile tile[12][12]) {
	if (state == TEAM1) {
		circle.draw(Palette::Skyblue);
		circle.drawFrame(0, 1, Palette::Black);
	}
	else if (state == TEAM2) {
		circle.draw(Palette::Orange);
		circle.drawFrame(0, 1, Palette::Black);
	}
	else {
		exit(EXIT_FAILURE);
	}

	if (stepState != STAY) {
		drawStep(tile);
	}

	font(id).drawCenter(circle.x - 2, circle.y - 2, Palette::Black);

}

void Agent::operate(Tile tile[12][12], int row, int column) {
	// 8•ûŒü‚Ì‘I‘ð
	Vec2 Arrow = (Mouse::Pos() - circle.center).normalize() * 45 + circle.center;
	if (circle.mouseOver) {
		circle.drawFrame(0, 2, Palette::Gray);

	}
	else {
		Line(circle.center, Arrow).drawArrow(2, Vec2(8, 8), Palette::Gray);
		circle.drawFrame(1, 1, Palette::Gray);
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < column; j++) {
				tile[i][j].overArrow(Arrow, this);
			}
		}
	}

	if (circle.leftReleased) {
		switch (stepState)
		{
		case MOVE:
			stepState = REMOVE;
			break;
		case REMOVE:
			stepState = STAY;
			break;
		case STAY:
			break;
		}
	}
}

void Agent::drawStep(Tile tile[12][12]) {
	Color c;
	switch (stepState)
	{
	case MOVE:
		c = Palette::Black;
		break;
	case REMOVE:
		c = Palette::Red;
		break;
	}
	Point n_tile = getpos() + nStep;
	Line(circle.center, tile[n_tile.x][n_tile.y].rect.center).drawArrow(2, Vec2(8, 8), c);
	if (!aiStep.isZero()) {
		Point ai_tile = getpos() + aiStep;
		Line(circle.center, circle.center + (Vec2)(tile[ai_tile.x][ai_tile.y].rect.center - circle.center).normalize() * 30).drawArrow(2, Vec2(8, 8), Palette::Purple);
	}
}

void Agent::update() {
	circle = Circle(show_x * 40 + margin_x + 20, show_y * 40 + margin_y + 20, 15);
}

Point Agent::getpos() {
	return Point(x, y);
}