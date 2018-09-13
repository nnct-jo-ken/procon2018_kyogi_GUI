#pragma once
#include<stdlib.h>
#include<time.h>

#include "Main.h"
#include "Tile.h"
#include "Agent.h"
#include "rand_board.h"


void init(int, int, int);
void updateField(Font);
void operateAgent();
void createBoard();

int row = 12;
int column = 12;
int turn = 60;
int a = 5;
bool inMenu = true;
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

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"盤面の生成");
	gui.add(L"auto", GUIButton::Create(L"自動生成"));
	gui.add(L"qr", GUIButton::Create(L"QRコードから"));


	// 盤面の初期化　削除予定
	while (System::Update())
	{
		if (inMenu) {
			if (gui.button(L"auto").pushed) {
				createBoard();
				gui.hide();
				inMenu = false;
			}
		}
		else {
			updateField(font);
			createBoard();
			if (Input::MouseR.clicked) {
				inMenu = true;
				gui.show();
			}
		}
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

	srand((unsigned)time(NULL));
	int x = (rand() % (row_ - 1)) / 2;
	int y = (rand() % (column_ - 1)) / 2;
	agent[0].init(x, y, TEAM1);
	agent[1].init(row_ - x - 1, column_ - y - 1, TEAM1);
	agent[2].init(x, column_ - y - 1, TEAM2);
	agent[3].init(row_ - x - 1, y, TEAM2);
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

void createBoard() {
	// row, columnの決定（ランダム）
	std::random_device rnddev;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnddev());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_real_distribution<> rand(8, 12);        // [0, 99] 範囲の一様乱数

	int __row, __column;
	__row = rand(mt);
	__column = rand(mt);

	int boardScore[12][12];
	init::rand_board(boardScore, __row, __column);

	init(__row, __column, 60);
	for (int i = 0; i < __row; i++) {
		for (int j = 0; j < __column; j++) {
			tile[i][j].score = boardScore[i][j];
		}
	}

	Println(L"column:", __column, L", row:", __row);
}