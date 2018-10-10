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
	GUI message;	//情報ウィンドウ
	GUI confirmation;	//確認ウィンドウ
public:
	void init();
};

