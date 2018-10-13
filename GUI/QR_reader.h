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
	GUI message;	//���E�B���h�E
	GUI confirmation;	//�m�F�E�B���h�E

	void init();
	void read();
};

