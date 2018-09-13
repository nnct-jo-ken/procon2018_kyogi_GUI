#include "TileObject.h"



void TileObject::init(int _posx, int _posy)
{
	posx = _posx;
	posy = _posy;
	rect = Rect(margin + posx * tile_size, margin + posy * tile_size, tile_size, tile_size);
}

void TileObject::update(Font font, AgentObject * agent) {
	draw(font, agent);
}

void TileObject::draw(Font font, AgentObject * agent) {
	switch (state)
	{
	case 0:
		rect.draw(Palette::White);
		break;
	case 1:
		rect.draw(Palette::Orange);
		break;
	case 2:
		rect.draw(Palette::Skyblue);
		break;
	default:
		exit(1);
	}
	rect.drawFrame(1, 1, Palette::Black);
	font(point).drawCenter(rect.x + rect.w / 2, rect.y + rect.h / 2, Palette::Black);
	if (agent->click_agent && agent->decide_step) {
		if (rect.contains(agent->pointer)) {
			rect.drawFrame(5, 0, Palette::Green);
			agent->slct_rect = rect;
			agent->slct_point = Point(posx, posy);
		}
	}
}