#pragma once

#include<Siv3D.hpp>

/// <summary>
/// QR情報の解析→ピースの情報ごとに分割
/// </summary>
/// <param name="decoded">デコードしたデータ</param>
/// <returns>ピース情報の配列</returns>
Array<String> get_pieceinfo(Array<String> decoded);