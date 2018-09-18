#pragma once
#include<stdlib.h>
#include<time.h>
#include<thread>
#include<string>

#include "Main.h"
#include "Tile.h"
#include "Agent.h"
#include "rand_board.h"
#include "tcp.h"


void init(int, int, int);
void updateField(Font);
void operateAgent();
void createBoard();
void thread_tcp();
void to_charArray(std::string, char[]);
void bufftoAgent(char[], State);
void setAI(State);
void displayInfo(Font);
void transitionTurn();
std::string toBuff_score();
std::string toBuff_state();


int row = 12;
int column = 12;
int turn = 60;
int a = 5;
bool inMenu = true;
bool ready = false;
Tile tile[12][12];
Agent agent[4];

void Main()
{
	// �E�B���h�E�̏����ݒ�
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	System::SetExitEvent(WindowEvent::Manual);

	std::thread tThread1(thread_tcp);

	const Font font(15);

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"�Ֆʂ̐���");
	gui.add(L"auto", GUIButton::Create(L"��������"));
	gui.add(L"qr", GUIButton::Create(L"QR�R�[�h����"));

	while (System::Update())
	{
		if (inMenu) {
			if (gui.button(L"auto").pushed) {
				createBoard();
				setAI(TEAM1);
				gui.hide();
				inMenu = false;
				ready = true;
			}
		}
		else {
			updateField(font);
			displayInfo(font);
		}

		// x�{�^�����������Ƃ��̏���
		if (System::GetPreviousEvent() & WindowEvent::CloseButton)
		{
			tThread1.detach();
			System::Exit();
		}
	}
}

// AI�̃`�[�������߂�
// init���Main�֐����Ŏ蓮�ŌĂяo��
void setAI(State s) {
	for (int i = 0; i < 4; i++) {
		if (agent[i].state == s) {
			agent[i].is_ai = true;
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

	for (int i = 0; i < 4; i++) {
		tile[agent[i].x][agent[i].y].state = agent[i].state;
	}

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

// �Ֆʂ̎�������������
// init���܂�
void createBoard() {
	// row, column�̌���i�����_���j
	std::random_device rnddev;     // �񌈒�I�ȗ���������𐶐�
	std::mt19937 mt(rnddev());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	std::uniform_real_distribution<> rand(8, 12);        // [0, 99] �͈͂̈�l����

	int __row, __column;
	__row = (int)rand(mt);
	__column = (int)rand(mt);

	int boardScore[12][12];
	init::rand_board(boardScore, __row, __column);

	init(__row, __column, 60);
	for (int i = 0; i < __row; i++) {
		for (int j = 0; j < __column; j++) {
			tile[i][j].score = boardScore[i][j];
		}
	}
}

// �ʐM�pthread
// �P�N���C�A���g�ɂ��P�X���b�h
void thread_tcp() {
	WSADATA wsaData;
	SOCKET sock0;
	SOCKET sock;
	struct sockaddr_in addr;
	int len;
	struct sockaddr_in client;

	char buff1[512];
	char buff2[512];
	char buff[32];

	// winsock2�̏�����
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	// �\�P�b�g�̍쐬
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	// �\�P�b�g�̐ݒ�
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	// �Ֆʂ̐������I���܂ő҂�
	while (!ready);

	// �ڑ���҂Ă��Ԃɂ���
	listen(sock0, 5);
	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr *)&client, &len);

	while (turn > 1) {
		to_charArray(toBuff_score(), buff1);
		to_charArray(toBuff_state(), buff2);
		tcp::Tsend(buff1, buff2, sock);

		tcp::Trecv(buff, sock);
		bufftoAgent(buff, TEAM1);
	}

	// �Z�b�V�������I��
	closesocket(sock);
	// winsock2�̏I������
	WSACleanup();
}

std::string toBuff_score() {
	std::string str = "";
	// �c���̃T�C�Y
	str += std::to_string(column);	str += ' ';
	str += std::to_string(row);		str += ':';
	// �e�^�C���̓��_
	for (int y = 0; y < column; y++) {
		for (int x = 0; x < row; x++) {
			str += std::to_string(tile[x][y].score);
			if (x == row - 1) {
				str += ':';
			}
			else {
				str += ' ';
			}
		}
	}

	// �e�G�[�W�F���g�̈ʒu
	for (int i = 0; i < 4; i++) {
		str += std::to_string(agent[i].x + 1);
		str += ' ';
		str += std::to_string(agent[i].y + 1);
		str += ':';
	}

	str += '\0';
	return str;
}

std::string toBuff_state() {
	std::string str = "";
	// �c��^�[����
	str += std::to_string(turn);	str += ':';
	// �e�^�C���̏��
	for (int y = 0; y < column; y++) {
		for (int x = 0; x < row; x++) {
			str += std::to_string((int)tile[x][y].state);
			if (x == row - 1) {
				str += ':';
			}
			else {
				str += ' ';
			}
		}
	}

	// �e�G�[�W�F���g�̏��
	for (int i = 0; i < 4; i++) {
		str += std::to_string((int)agent[i].state);
		str += ':';
	}

	str += '\0';
	return str;
}

void to_charArray(std::string str, char buff[]) {
	int i = 0;
	while (true) {
		buff[i] = str[i];
		if (str[i] == '\0') {
			break;
		}
		i++;
	}
}

void bufftoAgent(char buff[], State team) {
	int data[6];
	std::string str = "";
	int index = 0;
	int x = 0;
	while (x < 6) {
		if (buff[index] == ' ' || buff[index] == ':') {
			data[x] = std::stoi(str);
			x++;
			str = "";
		}
		else {
			str += buff[index];
		}
		index++;
	}

	int y = 0;
	for (int i = 0; i < 4; i++) {
		if (agent[i].state == team) {
			if (data[y * 3] == 0 && data[y * 3 + 1] == 0) {
				continue;
			}
			if (agent[i].x + data[y * 3] < 0 || agent[i].x + data[y * 3] >= row) {
				continue;
			}
			if (agent[i].y + data[y * 3 + 1] < 0 || agent[i].y + data[y * 3 + 1] >= column) {
				continue;
			}
			if (data[y * 3 + 2] == 0) {
				agent[i].stepState = MOVE;
			}
			else {
				agent[i].stepState = REMOVE;
			}
			agent[i].nStep = Point(data[y * 3], data[y * 3 + 1]);
			agent[i].aiStep = Point(data[y * 3], data[y * 3 + 1]);
		}
		y++;
	}
}

// �Q�[���̏���`�悷��
void displayInfo(Font font) {
	int infox = tile[row - 1][0].rect.x + 100;
	font(L"�c��", turn, L"�^�[��").draw(infox, margin_y, Palette::Black);
	Rect button(infox, margin_y + 50, 130, 30);
	button.drawFrame(1, 1, Palette::Black);
	if (button.mouseOver) {
		button.draw(Color(200, 200, 200));
	}
	if (button.leftPressed) {
		button.draw(Color(150, 150, 150));
	}
	if (button.leftClicked) {
		transitionTurn();
	}
	font(L"���̃^�[����").draw(infox, margin_y + 50, Palette::Black);
}

// �^�[�����ڂ̊֐�
// displayInfo���ŌĂяo��
void transitionTurn() {


	// �ړ��悪�Ⴄ�`�[���̏ꍇ
	for (int i = 0; i < 4; i++) {
		if (agent[i].stepState == MOVE) {
			State s = tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].state;
			if (s != agent[i].state && s != NEUTRAL) {
				agent[i].stepState = REMOVE;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		if (agent[i].stepState == REMOVE) {
			State s = tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].state;
			if (agent[i].state != s && s != NEUTRAL) {
				agent[i].stepState = STAY;
			}
			agent[i].deletePoint = agent[i].nStep;
			agent[i].nStep = Point(0, 0);
		}
		if (agent[i].stepState == STAY) {
			agent[i].nStep = Point(0, 0);
		}
	}

	// Move�̃G�[�W�F���g���画����͂��߂�
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 4; i++) {
			if (agent[i].stepState == MOVE) {
				for (int j = 0; j < 4; j++) {
					if (j == i) { continue; }
					if (agent[i].x + agent[i].nStep.x == agent[j].x + agent[j].nStep.x &&
						agent[i].y + agent[i].nStep.y == agent[j].y + agent[j].nStep.y) {
						agent[i].stepState = STAY;
						agent[i].nStep = Point(0, 0);
						if (agent[j].stepState != REMOVE) {
							agent[j].stepState = STAY;
							agent[j].nStep = Point(0, 0);
						}
					}
				}
			}
		}
	}

	// Remove�̃G�[�W�F���g�̔���
	for (int i = 0; i < 3; i++) {
		if (agent[i].stepState == REMOVE) {
			for (int j = i + 1; j < 4; j++) {
				if (agent[i].deletePoint == agent[j].nStep) {
					agent[i].stepState = STAY;
				}
				if (agent[j].stepState == REMOVE) {
					if (agent[i].x + agent[i].deletePoint.x == agent[j].x + agent[j].deletePoint.x &&
						agent[i].y + agent[i].deletePoint.y == agent[j].y + agent[j].deletePoint.y) {
						agent[i].stepState = STAY;
						agent[j].stepState = STAY;
					}
				} else {
					if (agent[i].x + agent[i].deletePoint.x == agent[j].x + agent[j].nStep.x &&
						agent[i].y + agent[i].deletePoint.y == agent[j].y + agent[j].nStep.y) {
						agent[i].stepState = STAY;
					}
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		if (agent[i].stepState == REMOVE) {
			tile[agent[i].x + agent[i].deletePoint.x][agent[i].y + agent[i].deletePoint.y].state = NEUTRAL;
		}
		if (agent[i].stepState == MOVE) {
			agent[i].x += agent[i].nStep.x;
			agent[i].y += agent[i].nStep.y;
			tile[agent[i].x][agent[i].y].state = agent[i].state;
		}
		agent[i].nStep = Point(0, 0);
		agent[i].deletePoint = Point(0, 0);
		agent[i].aiStep = Point(0, 0);
		agent[i].stepState = STAY;
		agent[i].update();
	}

	turn--;
}