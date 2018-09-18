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
	// ウィンドウの初期設定
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	System::SetExitEvent(WindowEvent::Manual);

	std::thread tThread1(thread_tcp);

	const Font font(15);

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"盤面の生成");
	gui.add(L"auto", GUIButton::Create(L"自動生成"));
	gui.add(L"qr", GUIButton::Create(L"QRコードから"));

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

		// xボタンを押したときの処理
		if (System::GetPreviousEvent() & WindowEvent::CloseButton)
		{
			tThread1.detach();
			System::Exit();
		}
	}
}

// AIのチームを決める
// init後にMain関数内で手動で呼び出す
void setAI(State s) {
	for (int i = 0; i < 4; i++) {
		if (agent[i].state == s) {
			agent[i].is_ai = true;
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

	for (int i = 0; i < 4; i++) {
		tile[agent[i].x][agent[i].y].state = agent[i].state;
	}

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

// 盤面の自動生成をする
// initを含む
void createBoard() {
	// row, columnの決定（ランダム）
	std::random_device rnddev;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnddev());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_real_distribution<> rand(8, 12);        // [0, 99] 範囲の一様乱数

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

// 通信用thread
// １クライアントにつき１スレッド
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

	// winsock2の初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	// ソケットの作成
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	// ソケットの設定
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	// 盤面の生成が終わるまで待つ
	while (!ready);

	// 接続を待てる状態にする
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

	// セッションを終了
	closesocket(sock);
	// winsock2の終了処理
	WSACleanup();
}

std::string toBuff_score() {
	std::string str = "";
	// 縦横のサイズ
	str += std::to_string(column);	str += ' ';
	str += std::to_string(row);		str += ':';
	// 各タイルの得点
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

	// 各エージェントの位置
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
	// 残りターン数
	str += std::to_string(turn);	str += ':';
	// 各タイルの状態
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

	// 各エージェントの状態
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

// ゲームの情報を描画する
void displayInfo(Font font) {
	int infox = tile[row - 1][0].rect.x + 100;
	font(L"残り", turn, L"ターン").draw(infox, margin_y, Palette::Black);
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
	font(L"次のターンへ").draw(infox, margin_y + 50, Palette::Black);
}

// ターン推移の関数
// displayInfo内で呼び出す
void transitionTurn() {


	// 移動先が違うチームの場合
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

	// Moveのエージェントから判定をはじめる
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

	// Removeのエージェントの判定
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