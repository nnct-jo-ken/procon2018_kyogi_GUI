#pragma once
#include "Main.h"
class QR_reader
{
private:
	Webcam webcam;
	Image image;
	DynamicTexture texture;
	std::string binary;
	Array<String> decoded;
	TextWriter info;
	QRData data;
	GUI message;	//���E�B���h�E
	GUI confirmation;	//�m�F�E�B���h�E
public:
	void init();
};

