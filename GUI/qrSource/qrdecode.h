#pragma once
#include"quirc.h"
#include<Siv3D.hpp>

/// <summary>
/// エンコーディングモードの変換
/// </summary>
/// <param name="qdata">データタイプ</param>
/// <returns>エンコーディングモード</returns>
QREncodingMode ConvertEncodingMode(int qdata);

/// <summary>
/// QRのデコード
/// </summary>
/// <param name="image">QRのイメージ</param>
/// <param name="data">データの保存先</param>
/// <returns>成否</returns>
bool decode(const Image& image, QRData& data);