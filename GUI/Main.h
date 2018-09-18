#pragma comment(lib, "wsock32.lib")

#pragma once
#include <Siv3D.hpp>
#include <random>
#include <winsock2.h>
#include <ws2tcpip.h>

static const int margin_x = 50;
static const int margin_y = 50;

enum State
{
	NEUTRAL, TEAM1, TEAM2
};

enum StepState
{
	MOVE, REMOVE, STAY
};

