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


void init(int, int, int);
void updateField(Font);
void operateAgent();
void createBoard();
void createFromQR();
void thread_tcp();
void thread_score();
void to_charArray(std::string, char[]);
void bufftoAgent(char[], State);
void setAI(State);
void displayInfo(Font);
void transitionTurn();
void stringtoarray(std::string, int[]);
void countAscore(int, int, int*, bool*, State);
int tileScore(State);
int areaScore(State);
bool can_act(int, int);
std::string toBuff_score();
std::string toBuff_state();

int row = 12;
int column = 12;
int a = 5;
bool inMenu = true;
std::atomic<int> turn = 100;
std::atomic<bool> ready = false;
Tile tile[12][12];
Agent agent[4];
//　得点計算用
bool reach_end = false;
int area_score = 0;

void Main()
{
	// ウィンドウの初期設定
	Graphics::SetBackground(Palette::White);
	Window::SetTitle(L"Procon29 - GUI - version2");
	Window::Resize(900, 600);
	System::SetExitEvent(WindowEvent::Manual);

	std::thread tThread1(thread_tcp);
	std::thread sThread(thread_score);

	const Font font(15);

	GUI gui(GUIStyle::Default);
	gui.setTitle(L"盤面の生成");
	gui.add(L"auto", GUIButton::Create(L"自動生成"));
	gui.add(L"qr", GUIButton::Create(L"QRコードから"));

	// 戻るボタンの情報
	//std::fstream file;
	//file.open("undo.dat", std::ios::binary | std::ios::out | std::ios::in);


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
			if (gui.button(L"qr").pushed) {
				createFromQR();
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
			sThread.detach();
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
}

// 盤面更新(描画 & 更新)
void updateField(Font font) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			tile[i][j].draw(font);
		}
	}

	for (int i = 0; i < 4; i++) {
		agent[i].draw(font);
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
	std::uniform_real_distribution<> randmt(8, 12);        // [0, 99] 範囲の一様乱数

	// row * column >= 80になるまで繰り返す

	int __row = 0, __column = 0;
	while (__row * __column < 80) {
		__row = (int)randmt(mt);
		__column = (int)randmt(mt);
	}

	int boardScore[12][12];
	init::rand_board(boardScore, __row, __column);

	init(__row, __column, turn);
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

// 盤面をQRから生成する
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

	// starsの文字列をint配列に変換
	for (int i = 0; i < num; i++) {
		stringtoarray(strs[i], data[i]);
	}

	// row columnの初期化
	init(data[0][1], data[0][0], 60);
	
	// タイルの得点を代入
	for (int x = 0; x < row; x++) {
		for (int y = 0; y < column; y++) {
			tile[x][y].score = data[y + 1][x];
		}
	}

	// 味方エージェントの位置代入
	agent[0].init(data[column + 1][1] - 1, data[column + 1][0] - 1, TEAM1);
	agent[1].init(data[column + 2][1] - 1, data[column + 2][0] - 1, TEAM1);
	agent[2].init(agent[0].x, column - agent[0].y - 1, TEAM2);
	agent[3].init(row - agent[0].x - 1, agent[0].y, TEAM2);

	for (int i = 0; i < 4; i++) {
		tile[agent[i].x][agent[i].y].state = agent[i].state;
		agent[i].id = i;
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
	int flag = turn;
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

	while (turn >= 0) {
		to_charArray(toBuff_score(), buff1);
		to_charArray(toBuff_state(), buff2);
		tcp::Tsend(buff1, buff2, sock);

		tcp::Trecv(buff, sock);
		bufftoAgent(buff, TEAM1);
		while (turn == flag);
		flag = turn;
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

	// 入力が正しいか判定
	// REMOVEの間違いはSTAYに
	// MOVEの間違いはREMOVEに
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

	// 判定
	for (int i = 0; i < 4; i++) {
		// STAYの場合は確定
		if (agent[i].stepState != STAY) {
			if (!can_act(i, i)) {
				agent[i].stepState = STAY;
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

// タイルスコア集計
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

// エリアスコア集計
int areaScore(State team) {
	int result = 0;
	// 初期化, 筋の設定
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

// 接続してきたクライアントに2チーム分の領域ポイント、タイルポイントを送信する
// 形式："タイルポイント1:領域ポイント1:タイルポイント2:領域ポイント2\0"
// port：15555
void thread_score() {
	WSADATA wsaData;
	SOCKET sock0;
	SOCKET sock;
	int len;
	struct sockaddr_in addr;
	struct sockaddr_in client;

	// winsock2の初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	// ソケットの作成
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	// ソケットの設定
	addr.sin_family = AF_INET;
	addr.sin_port = htons(15555);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	// 盤面の生成が終わるまで待つ
	while (!ready);

	while (turn >= 0) {
		// 接続を待てる状態にする
		listen(sock0, 5);
		len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);

		// 送信バッファの作成
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
		// 送信
		send(sock, buff, 32, 0);
		// セッションを終了
		closesocket(sock);
	}
	// winsock2の終了処理
	WSACleanup();
}