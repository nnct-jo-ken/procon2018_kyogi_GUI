#pragma once
#include"quirc.h"
#include<Siv3D.hpp>

/// <summary>
/// �G���R�[�f�B���O���[�h�̕ϊ�
/// </summary>
/// <param name="qdata">�f�[�^�^�C�v</param>
/// <returns>�G���R�[�f�B���O���[�h</returns>
QREncodingMode ConvertEncodingMode(int qdata);

/// <summary>
/// QR�̃f�R�[�h
/// </summary>
/// <param name="image">QR�̃C���[�W</param>
/// <param name="data">�f�[�^�̕ۑ���</param>
/// <returns>����</returns>
bool decode(const Image& image, QRData& data);