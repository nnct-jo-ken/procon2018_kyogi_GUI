#pragma comment(lib, "wsock32.lib")

#pragma once
#include <Siv3D.hpp>
#include <random>
#include <winsock2.h>
#include <ws2tcpip.h>

enum State
{
	NEUTRAL, TEAM1, TEAM2
};

enum StepState
{
	MOVE, REMOVE, STAY
};

