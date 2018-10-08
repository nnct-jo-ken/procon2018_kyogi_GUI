#pragma once
#include "Main.h"

class tcp
{
public:
	static int Tsend(char[], char[], SOCKET sock);
	static int Trecv(char[], SOCKET sock);
};

