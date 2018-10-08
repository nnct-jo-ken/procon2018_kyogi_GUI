#include "tcp.h"

int tcp::Tsend(char buff_score[], char buff_state[], SOCKET sock) {
	char cache[16];
	if (send(sock, buff_score, 512, 0) < 0) {
		return -1;
	}
	memset(cache, 0, sizeof(cache));
	if (recv(sock, cache, sizeof(cache), 0) < 0) {
		return -1;
	}
	if (send(sock, buff_state, 512, 0) < 0) {
		return -1;
	}
	return 0;
}

int tcp::Trecv(char buff[], SOCKET sock) {
	memset(buff, 0, sizeof(buff));
	if (recv(sock, buff, 32, 0) < 0) {
		return -1;
	}
	return 0;
}