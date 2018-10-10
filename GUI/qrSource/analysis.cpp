#include "analysis.h"

// データの整形
Array<String> get_pieceinfo(Array<String> decoded){
	Array<String> acquisition_data;
	int begin = 0, end = 0;

	/*
	for (const auto qr : decoded) {	//decoded配列の要素を順にqrに代入
		begin = qr.indexOf(L':') + 1;	//最初に':'が現れる次の文字をbeginに設定
		int count = FromString<int>(qr.substr(0, qr.indexOf(L':')));	//最初の':'までの文字を数字に変換してcountに代入
		if (qr == decoded.back()) {
			count++;
		}
		for (int i = 0; i < count; i++) {
			end = qr.indexOf(L':', begin);	//beginの位置から':'があるまで探す.見つけたら、その場所をendに入れる
			acquisition_data.push_back(qr.substr(begin, end - begin));	//beginからendまでの文字列を配列に投入
			begin = end + 1;	//beginをendの次へ
		}
	}
	*/

	for (const auto qr : decoded) {	//decoded配列の要素を順にqrに代入
		while (qr.indexOf(L':', begin) != s3d::String::npos) {
			end = qr.indexOf(L':', begin);	//beginの位置から':'があるまで探す.見つけたら、その場所をendに入れる
			acquisition_data.push_back(qr.substr(begin, end - begin));	////beginからendまでの文字列を配列に投入
			begin = end + 1;	//beginをendの次へ
		}
	}

	return acquisition_data;
}
