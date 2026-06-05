#pragma once

// 会話ウィンドウに表示するスクリプト。
// lines / choices は寿命に注意（表示中は有効である必要があるので static 等で渡す）。
typedef struct {
	const char** lines;        // 本文（UTF-8）の配列
	int          count;        // 行数
	const char*  name;         // 話者名（NULL なら名前ボックスなし）
	const char** choices;      // 末尾で出す選択肢（NULL/0 なら無し）
	int          choice_count;
} DialogueScript;

// 選択結果は scene_pop の result で返る：
//   選択肢ありなら INT_TO_ARG(選んだindex)、
//   選択肢なし/キャンセル(B)なら NULL(=0)。
//
// 例:
//   static const char* lines[1]   = { "..." };
//   static const char* choices[2] = { "Yes", "No" };
//   static DialogueScript sc;  // static はゼロ初期化される
//   sc.lines = lines; sc.count = 1; sc.name = "Guide";
//   sc.choices = choices; sc.choice_count = 2;
//   scene_push(SCENE_DIALOGUE, &sc);
