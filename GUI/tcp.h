#pragma once
#include "Main.h"

class tcp
{
public:
	static void Tsend(char[], char[], SOCKET sock);
	static void Trecv(char[], SOCKET sock);
};

