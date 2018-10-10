#include "text_set.h"

void node_set(TextWriter info, Array<String> node) {	//nodeには整形済みデータが入る
	String buf;

	for (const auto vertex : node) {
		//buf = vertex.substr(vertex.indexOf(L" ") + 1);
		if (vertex.front() == ' ') {	//なぜか半角コロンの後ろにスペースを含んでいるときのスペース除去
			buf = vertex.substr(1);	//2番目の文字から最後まで（最後は省略）
		}
		else {
			buf = vertex;
		}
		info.writeln(buf);	//実際にファイルへの書き込み処理？
	}
}