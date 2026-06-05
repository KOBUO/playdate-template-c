#pragma once

// 会話ウィンドウに表示するスクリプト。
// lines は UTF-8 文字列の配列（寿命に注意：表示中は有効である必要があるので
// static か長寿命のものを渡す。tr() の戻り値を static 配列に入れて渡すのが簡単）。
typedef struct {
	const char** lines;
	int          count;
} DialogueScript;

// 使い方:
//   static const char* lines[2];
//   lines[0] = tr(STR_FOO); lines[1] = tr(STR_BAR);
//   static DialogueScript sc; sc.lines = lines; sc.count = 2;
//   scene_push(SCENE_DIALOGUE, &sc);
