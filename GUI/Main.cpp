#include <iostream>
#include <Siv3D.hpp>
#include "Main.h"
#include "TileObject.h"
#include "AgentObject.h"
#include "GUI_Button.h"

#include <winsock2.h>
#include <ws2tcpip.h>

WSADATA wsaData;
struct sockaddr_in server;
SOCKET sock;
char destination[] = "127.0.0.1";

// 盤面情報をchar[][]に変換する
void make_init_info(char buff[6][600], AgentObject agent[4], TileObject tile[12][12], int row, int column, int turn) {
	std::string str;
	str += std::to_string(row);
	str += ' ';
	str += std::to_string(column);
	str += ' ';
	str += std::to_string(turn);
	for (int i = 0;; i++) {
		buff[0][i] = str[i];
		if (str[i] == '\0') {
			break;
		}
	}

	str = "";
	for (int y = 0; y < column; y++) {
		for (int x = 0; x < row; x++) {
			str += std::to_string(tile[x][y].point);
			str += ' ';
		}
	}

	for (int i = 0;; i++) {
		buff[1][i] = str[i];
		if (str[i] == '\0') {
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		str = "";
		str += std::to_string(agent[i].posx);
		str += ' ';
		str += std::to_string(agent[i].posy);
		str += ' ';
		str += std::to_string(agent[i].state);
		for (int j = 0;; j++) {
			buff[2 + i][j] = str[j];
			if (str[j] == '\0') {
				break;
			}
		}
	}

}

// サーバーから受けとったchar配列を盤面情報へ変換する
// get_borad_info内で呼び出す専用
void buff_to_board(char buff[7][600], int * row, int * column, int * turn, AgentObject agent[4], TileObject field[12][12]) {
	// 変換に使用するキャッシュ
	std::string strs;
	// row, column, turnの変換

	// rowを取り出す
	int i = 0;
	while (true) {
		strs += buff[0][i];
		i++;
		if (buff[0][i] == ' ' || buff[0][i] == '\0') {
			*row = std::stoi(strs);
			break;
		}
	}
	// columnを取り出す
	i++;
	strs = "";
	while (true) {
		strs += buff[0][i];
		i++;
		if (buff[0][i] == ' ' || buff[0][i] == '\0') {
			*column = std::stoi(strs);
			break;
		}
	}
	// turnを取り出す
	i++;
	strs = "";
	while (true) {
		strs += buff[0][i];
		i++;
		if (buff[0][i] == ' ' || buff[0][i] == '\0') {
			*turn = std::stoi(strs);
			break;
		}
	}

	// 盤面の得点を取り出す
	i = 0;
	for (int y = 0; y < *column; y++) {
		for (int x = 0; x < *row; x++) {
			strs = "";
			while (true) {
				strs += buff[1][i];
				i++;
				if (buff[1][i] == ' ' || buff[1][i] == '\0') {
					field[x][y].point = std::stoi(strs);
					i++;
					break;
				}
			}
		}
	}
	// 盤面の色を取り出す
	i = 0;
	for (int y = 0; y < *column; y++) {
		for (int x = 0; x < *row; x++) {
			strs = "";
			while (true) {
				strs += buff[2][i];
				i++;
				if (buff[2][i] == ' ' || buff[2][i] == '\0') {
					field[x][y].state = std::stoi(strs);
					i++;
					break;
				}
			}
		}
	}
	// 各種エージェントの色を取り出す
	for (int j = 0; j < 4; j++) {
		i = 0;
		strs = "";
		// x座標を取り出す
		while (true) {
			strs += buff[3 + j][i];
			i++;
			if (buff[3 + j][i] == ' ' || buff[3 + j][i] == '\0') {
				agent[j].posx = std::stoi(strs);
				i++;
				break;
			}
		}
		strs = "";
		// y座標を取り出す
		while (true) {
			strs += buff[3 + j][i];
			i++;
			if (buff[3 + j][i] == ' ' || buff[3 + j][i] == '\0') {
				agent[j].posy = std::stoi(strs);
				i++;
				break;
			}
		}
		strs = "";
		// stateを取り出す
		while (true) {
			strs += buff[3 + j][i];
			i++;
			if (buff[3 + j][i] == ' ' || buff[3 + j][i] == '\0') {
				agent[j].state = std::stoi(strs);
				i++;
				break;
			}
		}
	}
}

// 盤面の情報を受け取る メインの処理
void get_borad_info(int * row, int * column, int * turn, AgentObject agent[4], TileObject field[12][12]) {
	char end_serial[] = "end";
	char board_indo[7][600];
	for (int i = 0; i < 7; i++) {
		recv(sock, board_indo[i], sizeof(board_indo[i]), 0);

		// タイミング合わせ
		if (send(sock, end_serial, sizeof(end_serial), 0) == SOCKET_ERROR) {
			std::cerr << WSAGetLastError() << std::endl;
		}
	}

	buff_to_board(board_indo, row, column, turn, agent, field);
}

// タイルとエージェントのオブジェクトを初期化する
void init_agent_tile(int row, int column, AgentObject agent[4], TileObject field[12][12]) {
	for (int i = 0; i < 4; i++) {
		agent[i].init();
	}

	for (int y = 0; y < column; y++) {
		for (int x = 0; x < row; x++) {
			field[x][y].init(x,y);
		}
	}
}

// 盤面を更新、描画する
void update_board(int row, int column, AgentObject agent[4], TileObject field[12][12], Font font) {
	bool selecting = false;
	int agent_index = 0;
	for (int i = 0; i < 4; i++) {
		if (agent[i].click_agent) {
			selecting = true;
			agent_index = i;
		}
	}
	for (int y = 0; y < column; y++) {
		for (int x = 0; x < row; x++) {
			field[x][y].update(font, selecting, &agent[agent_index]);
		}
	}
	for (int i = 0; i < 4; i++) {
		agent[i].update();
	}
}

void Main()
{
	const Font font(13);
	int row = 0;
	int column = 0;
	int turn = 0;
	TileObject field[12][12];
	AgentObject agent[4];
	GUI_Button next_turn;

	bool can_move_agent[] = { false, false, false, false};

	// 盤面とエージェントの初期配置を送るかどうか
	bool isSendInitInfo = false;

	char end_serial[] = "end";

	Window::Resize(800, 600);
	Graphics::SetBackground(Palette::White);

	Println(L"サーバーに接続します...");

	// winsock2の初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	// ソケットの作成
	sock = socket(AF_INET, SOCK_STREAM, 0);
	// 接続先指定用構造体の準備
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	inet_pton(server.sin_family, destination, &server.sin_addr.S_un.S_addr);

	// サーバに接続
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
		std::cerr << "接続に失敗しました" << std::endl;
	}

	Println(L"サーバーに接続しました");

	/* 盤面とエージェントの情報を渡すための配列
	
	row column
	フィールドの得点
	x y state (エージェント0)
	x y state (エージェント1)
	x y state (エージェント2)
	x y state (エージェント3)
	*/
	char init_board_buff[6][600];

	// 盤面情報の配列を生成
	make_init_info(init_board_buff, agent, field, row, column, turn);

	// 盤面とエージェントの情報を送信、または送信しない
	if (isSendInitInfo) {
		if (send(sock, "1\n", 2, 0)) {
			std::cerr << WSAGetLastError() << std::endl;
		}

		for (int i = 0; i < 6; i++) {
			if (send(sock, init_board_buff[i], sizeof(init_board_buff[i]), 0)) {
				std::cerr << WSAGetLastError() << std::endl;
			}
			char cache[20];
			recv(sock, cache, sizeof(cache), 0);
		}
	}
	else {
		if (send(sock, "0\n", 2, 0)) {
			std::cerr << WSAGetLastError() << std::endl;
		}
	}

	closesocket(sock);
	// winsock2の終了処理
	WSACleanup();
	Sleep(100);

	// winsock2の初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	// ソケットの作成
	sock = socket(AF_INET, SOCK_STREAM, 0);
	// 接続先指定用構造体の準備
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	inet_pton(server.sin_family, destination, &server.sin_addr.S_un.S_addr);

	// サーバに接続
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
		std::cerr << "接続に失敗しました" << std::endl;
	}

	char test[] = { "2" };
	if (send(sock, test, sizeof(test), 0)) {
		std::cerr << WSAGetLastError() << std::endl;
	}
	char cache[20];
	recv(sock, cache, sizeof(cache), 0);


	// 盤面から操作すべきagentのidを受け取る
	char id_cache[10];
	recv(sock, id_cache, sizeof(id_cache), 0);
	// タイミング合わせ
	if (send(sock, end_serial, sizeof(end_serial), 0) == SOCKET_ERROR) {
		std::cerr << WSAGetLastError() << std::endl;
	}

	for (int i = 0;; i++) {
		if (id_cache[i] == '\0') {
			break;
		}
		if (id_cache[i] != ' ') {
			std::string str;
			str += id_cache[i];
			can_move_agent[std::stoi(str)] = true;
		}
	}

	// サーバーから盤面情報を受け取る
	get_borad_info(&row, &column, &turn, agent, field);

	init_agent_tile(row, column, agent, field);

	next_turn.init(field[row-1][0].rect.x + 50, 100, 120, 40, L"移動を決定");

	while (System::Update())
	{
		update_board(row, column, agent, field, font);
		next_turn.draw_button(font);
	}
}
