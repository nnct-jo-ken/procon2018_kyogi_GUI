#include "tcp.h"

void tcp::Tsend(char buff_score[], char buff_state[], SOCKET sock) {
	char cache[16];
	send(sock, buff_score, 512, 0);
	memset(cache, 0, sizeof(cache));
	recv(sock, cache, sizeof(cache), 0);
	send(sock, buff_state, 512, 0);
	memset(cache, 0, sizeof(cache));
	recv(sock, cache, sizeof(cache), 0);
}

void tcp::Trecv(char buff[], SOCKET sock) {
	memset(buff, 0, 32);
	recv(sock, buff, 32, 0);
	send(sock, "end\0", 8, 0);
}