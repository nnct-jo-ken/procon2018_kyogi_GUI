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
	// �E�B���h�E�̏����ݒ�
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	Window::SetStyle(WindowStyle::Sizeable);

	const Font font(15);

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"�Ֆʂ̐���");
	gui.add(L"auto", GUIButton::Create(L"��������"));
	gui.add(L"qr", GUIButton::Create(L"QR�R�[�h����"));


	// �Ֆʂ̏������@�폜�\��
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

// �Ֆʏ�����
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

// �ՖʍX�V(�`�� & �X�V)
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

// �G�[�W�F���g�̑���
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
	// row, column�̌���i�����_���j
	std::random_device rnddev;     // �񌈒�I�ȗ���������𐶐�
	std::mt19937 mt(rnddev());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	std::uniform_real_distribution<> rand(8, 12);        // [0, 99] �͈͂̈�l����

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