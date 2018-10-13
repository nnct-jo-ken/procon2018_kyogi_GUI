#pragma once
#include "Main.h"
#include "qrSource/include.h"
#include <fstream>
#include <thread>

class QR_reader
{
public:
	Image image;
	DynamicTexture texture;
	std::string binary;
	Array<String> decoded;
	TextWriter info;
	QRData data;
	GUI message;	//情報ウィンドウ
	GUI confirmation;	//確認ウィンドウ

	void init();
	void read();
};

