#pragma once
#include "Main.h"
#include "Tile.h"
#include "Agent.h"

void init(int, int, int);
void updateField(Font);
void operateAgent();

int row = 12;
int column = 12;
int turn = 60;
int a = 5;
Tile tile[12][12];
Agent agent[4];

void Main()
{
	// ウィンドウの初期設定
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	Window::SetStyle(WindowStyle::Sizeable);

	const Font font(15);

	// 盤面の初期化　削除予定
	init(12, 12, 60);
	while (System::Update())
	{
		updateField(font);
	}
}

// 盤面初期化
void init(int row_, int column_, int turn_) {
	row = row_;
	column = column_;
	turn = turn_;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			tile[i][j].init(i, j, 1);
		}
	}

	// テスト用
	agent[0].init(0, 0, TEAM1);
	agent[1].init(1, 2, TEAM1);
	agent[2].init(2, 4, TEAM2);
	agent[3].init(3, 6, TEAM2);
}

// 盤面更新(描画 & 更新)
void updateField(Font font) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			tile[i][j].draw(font);
		}
	}

	for (int i = 0; i < 4; i++) {
		agent[i].draw();
	}

	operateAgent();
}

// エージェントの操作
void operateAgent() {
	if (a == 5) {
		for (int i = 0; i < 4; i++) {
			if (agent[i].circle.leftPressed) {
				a = i;
			}
		}
	}

	if (a != 5) {
		agent[a].operate(tile, row, column);
	}

	if (Input::MouseL.released) {
		a = 5;
	}

}