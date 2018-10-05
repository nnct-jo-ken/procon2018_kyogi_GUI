#pragma once
#include<stdlib.h>
#include<time.h>
#include<thread>
#include<string>
#include <fstream>
#include <atomic>

#include "Main.h"
#include "Tile.h"
#include "Agent.h"
#include "rand_board.h"
#include "tcp.h"


void init(int, int);
void updateField(Font);
void operateAgent();
void createBoard();
void createFromQR();
void thread_tcp(u_short, State);
void thread_score();
void to_charArray(std::string, char[]);
void bufftoAgent(char[], State);
void displayInfo(Font, Texture, Texture);
void transitionTurn();
void stringtoarray(std::string, int[]);
void countAscore(int, int, int*, bool*, State);
void undo(Texture);
void redo(Texture);
void rotate_board(Texture);
void copy_show_pos();
int tileScore(State);
int areaScore(State);
bool can_act(int, int);
std::string toBuff_score();
std::string toBuff_state();

int row = 12;
int column = 12;
int what_is_this = 5;
int angle = 0;
bool inMenu = true;
int all_turn;
std::atomic<int> turn = 80;
std::atomic<bool> ready = false;
Tile tile[12][12];
Agent agent[4];
Circle origin;
//�@���_�v�Z�p
bool reach_end = false;
int area_score = 0;

bool a = true;

// undo redo�p �Ƃ肠����120�^�[����
struct command procedure[120];
int p_pointer = -1;

void Main()
{
	// �E�B���h�E�̏����ݒ�
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	System::SetExitEvent(WindowEvent::Manual);

	std::thread tThread1(thread_tcp, 12345, TEAM1);
	std::thread tThread2(thread_tcp, 12346, TEAM2);
	std::thread sThread(thread_score);

	const Font font(15);
	const Texture undo_png(L"./textures/undo.png");
	const Texture redo_png(L"./textures/redo.png");
	const Texture rotate_png(L"./textures/rotate.png");
	const Texture heart_png(L"./textures/heart.png");
	const Texture spade_png(L"./textures/spade.png");

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"�Ֆʂ̐���");
	gui.add(L"auto", GUIButton::Create(L"��������"));
	gui.add(L"qr", GUIButton::Create(L"QR�R�[�h����"));

	// �߂�{�^���̏��
	//std::fstream file;
	//file.open("undo.dat", std::ios::binary | std::ios::out | std::ios::in);


	while (System::Update())
	{
		if (inMenu) {
			if (gui.button(L"auto").pushed) {
				createBoard();
				copy_show_pos();
				inMenu = false;
			}
			if (gui.button(L"qr").pushed) {
				createFromQR();
				copy_show_pos();
				inMenu = false;
			}
			if (!inMenu) {
				gui.hide();
				ready = true;
			}
		}
		else {
			updateField(font);
			displayInfo(font, spade_png, heart_png);
			undo(undo_png);
			redo(redo_png);
			rotate_board(rotate_png);
			if (turn == 0 && a) {
				Println(L"TEAM1:", tileScore(TEAM1), L":", areaScore(TEAM1));
				Println(L"TEAM2:", tileScore(TEAM2), L":", areaScore(TEAM2));
				a = false;
			}
		}

		// x�{�^�����������Ƃ��̏���
		if (System::GetPreviousEvent() & WindowEvent::CloseButton)
		{
			tThread1.detach();
			tThread2.detach();
			sThread.detach();
			System::Exit();
		}
	}
}

// �Ֆʏ�����
void init(int row_, int column_) {
	row = row_;
	column = column_;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			tile[i][j].init(i, j, 1);
		}
	}
	all_turn = turn;
	origin = Circle(margin_x, margin_y, 5);
}

// �ՖʍX�V(�`�� & �X�V)
void updateField(Font font) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			tile[i][j].draw(font);
		}
	}

	for (int i = 0; i < 4; i++) {
		agent[i].draw(font, tile);
	}
	operateAgent();
}

// �G�[�W�F���g�̑���
void operateAgent() {
	if (what_is_this == 5) {
		for (int i = 0; i < 4; i++) {
			if (agent[i].circle.leftPressed) {
				what_is_this = i;
			}
		}
	}

	if (what_is_this != 5) {
		agent[what_is_this].operate(tile, row, column);
	}

	if (Input::MouseL.released) {
		what_is_this = 5;
	}

}

// �Ֆʂ̎�������������
// init���܂�
void createBoard() {
	// row, column�̌���i�����_���j
	std::random_device rnddev;     // �񌈒�I�ȗ���������𐶐�
	std::mt19937 mt(rnddev());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	std::uniform_real_distribution<> randmt(8, 12);        // [0, 99] �͈͂̈�l����

	// row * column >= 80�ɂȂ�܂ŌJ��Ԃ�

	int __row = 0, __column = 0;
	while (__row * __column < 80) {
		__row = (int)randmt(mt);
		__column = (int)randmt(mt);
	}

	int boardScore[12][12];
	init::rand_board(boardScore, __row, __column);

	init(__row, __column);
	for (int i = 0; i < __row; i++) {
		for (int j = 0; j < __column; j++) {
			tile[i][j].score = boardScore[i][j];
		}
	}

	srand((unsigned)time(NULL));
	int x = (rand() % (row - 1)) / 2;
	int y = (rand() % (column - 1)) / 2;
	agent[0].init(x, y, TEAM1);
	agent[1].init(row - x - 1, column - y - 1, TEAM1);
	agent[2].init(x, column - y - 1, TEAM2);
	agent[3].init(row - x - 1, y, TEAM2);

	for (int i = 0; i < 4; i++) {
		tile[agent[i].x][agent[i].y].state = agent[i].state;
		agent[i].id = i;
	}
}

// �Ֆʂ�QR���琶������
void createFromQR() {
	std::ifstream shapeinfo("./shape_info.txt");
	std::string strs[15];
	int data[15][12];
	int num = 0;

	if (shapeinfo.fail()) {
		Println(L"error failed open file");
		return;
	}

	while (getline(shapeinfo, strs[num])) {
		num++;
	}

	// stars�̕������int�z��ɕϊ�
	for (int i = 0; i < num; i++) {
		stringtoarray(strs[i], data[i]);
	}

	// row column�̏�����
	init(data[0][1], data[0][0]);
	
	// �^�C���̓��_����
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			tile[x][y].score = data[y + 1][x];
		}
	}

	// �����G�[�W�F���g�̈ʒu���
	agent[0].init(data[column + 1][1] - 1, data[column + 1][0] - 1, TEAM1);
	agent[1].init(data[column + 2][1] - 1, data[column + 2][0] - 1, TEAM1);
	agent[2].init(agent[0].x, column - agent[0].y - 1, TEAM2);
	agent[3].init(row - agent[0].x - 1, agent[0].y, TEAM2);

	for (int i = 0; i < 4; i++) {
		tile[agent[i].x][agent[i].y].state = agent[i].state;
		agent[i].id = i;
	}
}

// �ʐM�pthread
// �P�N���C�A���g�ɂ��P�X���b�h
void thread_tcp(u_short port, State team) {
	WSADATA wsaData;
	SOCKET sock0;
	SOCKET sock;
	struct sockaddr_in addr;
	int len;
	int flag = turn;
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
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	// �Ֆʂ̐������I���܂ő҂�
	while (!ready);

	// �ڑ���҂Ă��Ԃɂ���
	listen(sock0, 5);
	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr *)&client, &len);

	while (turn >= 0) {
		to_charArray(toBuff_score(), buff1);
		to_charArray(toBuff_state(), buff2);
		tcp::Tsend(buff1, buff2, sock);

		tcp::Trecv(buff, sock);
		bufftoAgent(buff, team);
		while (turn == flag);
		flag = turn;
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
		str += std::to_string(agent[i].x);
		str += ' ';
		str += std::to_string(agent[i].y);
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
	
	int i = 0;
	if (team == TEAM2)
		i = 2;
	for (int j = 0; j < 2; j++) {
		if (data[j * 3] == 0 && data[j * 3 + 1] == 0) {
			continue;
		}
		if (agent[j + i].x + data[j * 3] < 0 || agent[j + i].x + data[j * 3] >= row ||
			agent[j + i].y + data[j * 3 + 1] < 0 || agent[j + i].y + data[j * 3 + 1] >= column) {
			continue;
		}
		else {
			if (data[j * 3 + 2] == 0) {
				agent[j + i].stepState = MOVE;
			}
			else {
				agent[j + i].stepState = REMOVE;
			}
			agent[j + i].nStep = Point(data[j * 3], data[j * 3 + 1]);
			agent[j + i].aiStep = Point(data[j * 3], data[j * 3 + 1]);
		}
	}
}

// �Q�[���̏���`�悷��
void displayInfo(Font font, Texture card_black, Texture card_red) {
	int infox = column * 40 + margin_x + 50;
	if (row > column) {
		infox = row * 40 + margin_x * 2 + 50;
	}
	font(L"�c��", turn, L"�^�[��").draw(infox, margin_y, Palette::Black);
	Rect button(infox, margin_y + 50, 130, 30);
	button.drawFrame(1, 1, Palette::Black);
	if (button.mouseOver) {
		button.draw(Color(200, 200, 200));
	}
	if (button.leftPressed) {
		button.draw(Color(150, 150, 150));
	}
	if (turn > 0) {
		if (button.leftClicked) {
			transitionTurn();
		}
	}
	font(L"���̃^�[����").draw(infox, margin_y + 50, Palette::Black);

	// �g�����v
	int card_index[2];
	for (int i = 0; i < 2; i++) {
		Point agent_direction;
		agent_direction.x = tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].show_x - agent[i].show_x;
		agent_direction.y = tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].show_y - agent[i].show_y;
		if (agent_direction == Point(0, 0)) { card_index[i] = 9; }
		if (agent_direction == Point(0, 1)) { card_index[i] = 1; }
		if (agent_direction == Point(1, 1)) { card_index[i] = 8; }
		if (agent_direction == Point(1, 0)) { card_index[i] = 7; }
		if (agent_direction == Point(1, -1)) { card_index[i] = 6; }
		if (agent_direction == Point(0, -1)) { card_index[i] = 5; }
		if (agent_direction == Point(-1, -1)) { card_index[i] = 4; }
		if (agent_direction == Point(-1, 0)) { card_index[i] = 3; }
		if (agent_direction == Point(-1, 1)) { card_index[i] = 2; }
		if (agent[i].stepState == STAY) { card_index[i] = 9; }
	}
	Rect agent_0_card = Rect(infox, 200, 100, 150);
	Rect agent_1_card = Rect(infox + 100, 200, 100, 150);
	agent_0_card(card_black((card_index[0] - 1)*409, 0, 409, 600)).draw();
	agent_1_card(card_red((card_index[1] - 1) * 409, 0, 409, 600)).draw();
	origin.draw(Palette::Red);
}

// �^�[�����ڂ̊֐�
// displayInfo���ŌĂяo��
void transitionTurn() {

	// ���͂�������������
	// REMOVE�̊ԈႢ��STAY��
	// MOVE�̊ԈႢ��REMOVE��
	for (int i = 0; i < 4; i++) {
		State tile_state = tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].state;
		if (agent[i].stepState == MOVE) {
			if (agent[i].state != tile_state && tile_state != NEUTRAL) {
				agent[i].stepState = REMOVE;
			}
		}
		if (agent[i].stepState == REMOVE) {
			agent[i].deletePoint = agent[i].nStep;
			if (tile_state == NEUTRAL) {
				agent[i].stepState = STAY;
				agent[i].nStep = Point(0, 0);
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (agent[i].getpos() + agent[i].nStep == agent[j].getpos() + agent[j].nStep) {
				agent[i].stepState = STAY;
				agent[j].stepState = STAY;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		if (agent[i].stepState == STAY) {
			agent[i].nStep = Point(0, 0);
		}
	}

	// ����
	for (int i = 0; i < 4; i++) {
		// STAY�̏ꍇ�͊m��
		if (agent[i].stepState != STAY) {
			if (!can_act(i, i)) {
				agent[i].stepState = STAY;
			}
		}
	}

	p_pointer = all_turn - turn;
	// �\���̂����Z�b�g
	// undo redo�̂��߂ɃR�}���h��ۑ�
	for (int i = 0; i < 4; i++) {
		procedure[p_pointer].stepState[i] = agent[i].stepState;
		if (agent[i].stepState == REMOVE) {
			procedure[p_pointer].nStep[i] = agent[i].deletePoint;
			procedure[p_pointer].original_state[i] 
				= tile[agent[i].x + agent[i].deletePoint.x][agent[i].y + agent[i].deletePoint.y].state;
		}
		else {
			procedure[p_pointer].nStep[i] = agent[i].nStep;
			procedure[p_pointer].original_state[i]
				= tile[agent[i].x + agent[i].nStep.x][agent[i].y + agent[i].nStep.y].state;
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
		agent[i].show_x = tile[agent[i].x][agent[i].y].show_x;
		agent[i].show_y = tile[agent[i].x][agent[i].y].show_y;
		agent[i].nStep = Point(0, 0);
		agent[i].deletePoint = Point(0, 0);
		agent[i].aiStep = Point(0, 0);
		agent[i].stepState = STAY;
		agent[i].update();
	}
	turn--;
}

bool can_act(int x, int y) {
	Point pos = agent[x].getpos() + agent[x].nStep;
	for (int i = 0; i < 4; i++) {
		if (i == x) { continue; }
		if (pos == agent[i].getpos()) {
			if (agent[i].stepState == STAY || agent[i].stepState == REMOVE) {
				return false;
			}
			if (i == y) {
				if (agent[i].stepState == MOVE && agent[y].stepState == MOVE) {
					return true;
				}
				return false;
			}
			if (!can_act(i, x)) {
				return false;
			}
		}
	}
	return true;
}

void stringtoarray(std::string strs, int arr[]) {
	std::string buf = "";
	int i = 0;
	int index = 0;
	while (true) {
		if (strs[i] == ' ') {
			arr[index] = std::stoi(buf);
			index++;
			buf = "";
		}
		if ((int)strs[i] > 0) {
			buf += strs[i];
		}
		if (strs[i] == 0) {
			arr[index] = std::stoi(buf);
			return;
		}
		i++;
	}
}

// �^�C���X�R�A�W�v
int tileScore(State team) {
	int score = 0;
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			if (tile[x][y].state == team) {
				score += tile[x][y].score;
			}
		}
	}
	return score;
}

// �G���A�X�R�A�W�v
int areaScore(State team) {
	int result = 0;
	// ������, �؂̐ݒ�
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			tile[x][y].is_searched = false;
			tile[x][y].is_end = false;
			if (x == 0 || y == 0 || x == row - 1 || y == column - 1) {
				tile[x][y].is_end = true;
			}
		}
	}
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			if (tile[x][y].state != team) {
				if (!tile[x][y].is_searched) {
					bool _reach_end = false;
					int sscore = 0;
					countAscore(x, y, &sscore, &_reach_end, team);
					if (!_reach_end) {
						result += sscore;
					}
				}
			}
		}
	}

	return result;
}

void countAscore(int x, int y, int* score, bool * _reach_end, State _team) {
	if (tile[x][y].state == _team || tile[x][y].is_searched) {
		return;
	}
	tile[x][y].is_searched = true;
	*score += (int)std::abs(tile[x][y].score);
	if (tile[x][y].is_end) {
		*_reach_end = true;
	}
	if (x > 0) {
		countAscore(x - 1, y, score, _reach_end, _team);
	}
	if (x < row - 1) {
		countAscore(x + 1, y, score, _reach_end, _team);
	}
	if (y > 0) {
		countAscore(x, y - 1, score, _reach_end, _team);
	}
	if (y < column - 1) {
		countAscore(x, y + 1, score, _reach_end, _team);
	}
}

// �ڑ����Ă����N���C�A���g��2�`�[�����̗̈�|�C���g�A�^�C���|�C���g�𑗐M����
// �`���F"�^�C���|�C���g1:�̈�|�C���g1:�^�C���|�C���g2:�̈�|�C���g2\0"
// port�F15555
void thread_score() {
	WSADATA wsaData;
	SOCKET sock0;
	SOCKET sock;
	int len;
	struct sockaddr_in addr;
	struct sockaddr_in client;

	// winsock2�̏�����
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	// �\�P�b�g�̍쐬
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	// �\�P�b�g�̐ݒ�
	addr.sin_family = AF_INET;
	addr.sin_port = htons(15555);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	// �Ֆʂ̐������I���܂ő҂�
	while (!ready);

	while (turn >= 0) {
		// �ڑ���҂Ă��Ԃɂ���
		listen(sock0, 5);
		len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);

		// ���M�o�b�t�@�̍쐬
		std::string strs = "";
		strs += std::to_string(tileScore(TEAM1));
		strs += ':';
		strs += std::to_string(areaScore(TEAM1));
		strs += ':';
		strs += std::to_string(tileScore(TEAM2));
		strs += ':';
		strs += std::to_string(areaScore(TEAM2));
		strs += '\0';
		
		char buff[32];
		// string -> char[]
		to_charArray(strs, buff);
		// ���M
		send(sock, buff, 32, 0);
		// �Z�b�V�������I��
		closesocket(sock);
	}
	// winsock2�̏I������
	WSACleanup();
}

void undo(Texture undo_png) {
	Rect rect = Rect(50, 10, 40, 40);
	rect(undo_png).draw();
	if (rect.leftPressed) {
		rect(undo_png).draw(Palette::Lightslategray);
	}

	// undo�̏���
	if (rect.leftClicked || (Input::KeyControl + Input::KeyZ).clicked) {
		// 1�^�[���ڈȊO�̎����s����
		int __pointer = all_turn - turn - 1;
		if (__pointer >= 0) {
			struct command now_command = procedure[__pointer];
			for (int i = 0; i < 4; i++) {
				agent[i].nStep = Point(0, 0);
				agent[i].aiStep = Point(0, 0);
				agent[i].stepState = STAY;
				if (now_command.stepState[i] == REMOVE) {
					tile[now_command.nStep[i].x + agent[i].x][now_command.nStep[i].y + agent[i].y].state
						= now_command.original_state[i];
				}
				if (now_command.stepState[i] == MOVE) {
					tile[agent[i].x][agent[i].y].state = now_command.original_state[i];
					agent[i].x -= now_command.nStep[i].x;
					agent[i].y -= now_command.nStep[i].y;
				}
				agent[i].show_x = tile[agent[i].x][agent[i].y].show_x;
				agent[i].show_y = tile[agent[i].x][agent[i].y].show_y;
				agent[i].update();
			}
			turn++;
		}
	}
}

void redo(Texture redo_png) {
	Rect rect = Rect(120, 10, 40, 40);
	rect(redo_png).draw();
	if (rect.leftPressed) {
		rect(redo_png).draw(Palette::Lightslategray);
	}
	// redo�̏���
	if (rect.leftClicked || (Input::KeyControl + Input::KeyY).clicked) {
		if (p_pointer - (all_turn - turn) >= 0) {
			int __pointer = all_turn - turn;
			struct command now_command = procedure[__pointer];
			for (int i = 0; i < 4; i++) {
				agent[i].nStep = Point(0, 0);
				agent[i].aiStep = Point(0, 0);
				agent[i].stepState = STAY;
				if (now_command.stepState[i] == MOVE) {
					tile[agent[i].x + now_command.nStep[i].x][agent[i].y + now_command.nStep[i].y].state = agent[i].state;
					agent[i].x += now_command.nStep[i].x;
					agent[i].y += now_command.nStep[i].y;
				}
				if (now_command.stepState[i] == REMOVE) {
					tile[agent[i].x + now_command.nStep[i].x][agent[i].y + now_command.nStep[i].y].state = NEUTRAL;
				}
				agent[i].show_x = tile[agent[i].x][agent[i].y].show_x;
				agent[i].show_y = tile[agent[i].x][agent[i].y].show_y;
				agent[i].update();
			}
			turn--;
		}
	}
}

void copy_show_pos() {
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			tile[x][y].show_x = tile[x][y].x;
			tile[x][y].show_y = tile[x][y].y;
		}
	}

	for (int i = 0; i < 4; i++) {
		agent[i].show_x = agent[i].x;
		agent[i].show_y = agent[i].y;
	}
}

void rotate_board(Texture tex) {
	Rect rect = Rect(250, 20, 30, 35);
	rect(tex).draw();
	if (rect.leftClicked) {
		int r_column = column;
		if (angle % 180 == 90) {
			r_column = row;
		}
		for (int x = 0; x < row; x++) {
			for (int y = 0; y < column; y++) {
				int rx = tile[x][y].show_x;
				int ry = tile[x][y].show_y;
				tile[x][y].show_x = -ry + r_column - 1;
				tile[x][y].show_y = rx;
				tile[x][y].rect = Rect(tile[x][y].show_x * 40 + margin_x, tile[x][y].show_y * 40 + margin_y, 40);
			}
		}

		for (int i = 0; i < 4; i++) {
			int rx = agent[i].show_x;
			int ry = agent[i].show_y;
			agent[i].show_x = -ry + r_column - 1;
			agent[i].show_y = rx;
			agent[i].circle = Circle(agent[i].show_x * 40 + margin_x + 20, agent[i].show_y * 40 + margin_y + 20, 15);
		}

		angle = (angle + 90) % 360;
	}

	if (angle == 0) { origin = Circle(margin_x, margin_y, 5); }
	if (angle == 90) { origin = Circle(margin_x + column * 40, margin_y, 5); }
	if (angle == 180) { origin = Circle(margin_x + row * 40, margin_y + column * 40, 5); }
	if (angle == 270) { origin = Circle(margin_x, margin_y + row * 40, 5); }
}