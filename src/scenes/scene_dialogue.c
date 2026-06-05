#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "dialogue.h"
#include <string.h>
#include <stdio.h>

// 会話ウィンドウ（オーバーレイ）：本編の上に重ねて表示する。
//  - タイプライター表示（1文字ずつ）
//  - A: 表示途中なら全文表示／表示済みなら次の行。最後の行で閉じる
//  - B: いつでも閉じる
//  - draw_below=1 なので下の画面が見えたまま
// 文字送りは UTF-8 のコードポイント単位（日本語でも崩れない）。

#define MAX_ROWS  3
#define ROW_BUF   256
#define CPS       28.0f   // 1秒あたりの表示文字数

// ウィンドウ枠
#define BOX_X 18
#define BOX_Y 150
#define BOX_W (SCREEN_WIDTH - BOX_X * 2)
#define BOX_H 78
#define PAD   12
#define LINE_H 22

typedef struct {
	const char** lines;
	int   count;
	int   cur;                    // 現在の行
	char  rows[MAX_ROWS][ROW_BUF]; // 折り返し後の表示行
	int   nrows;
	int   total_cp;               // 現在行の総コードポイント数
	int   reveal;                 // 表示済みコードポイント数
	float timer;
	const char* name;             // 話者名（NULL可）
	const char** choices;         // 選択肢（NULL可）
	int   choice_count;
	int   choosing;               // 選択肢モードか
	int   sel;                    // 選択中の選択肢
} DlgState;

// --- UTF-8 ユーティリティ ---
static int utf8_len(unsigned char c)
{
	if (c < 0x80) return 1;
	if ((c >> 5) == 0x6) return 2;
	if ((c >> 4) == 0xE) return 3;
	if ((c >> 3) == 0x1E) return 4;
	return 1;
}
static int utf8_count(const char* s)
{
	int n = 0;
	for (const char* p = s; *p; p += utf8_len((unsigned char)*p)) n++;
	return n;
}
static int utf8_bytes_for(const char* s, int cp)
{
	const char* p = s;
	while (cp-- > 0 && *p) p += utf8_len((unsigned char)*p);
	return (int)(p - s);
}

// 現在行を枠幅に合わせて折り返す（コードポイント単位。日本語向け）
static void layout(DlgState* s)
{
	const char* text = s->lines[s->cur];
	int maxw = BOX_W - PAD * 2;
	int nr = 0, rl = 0;
	s->rows[0][0] = 0;

	for (const char* p = *text ? text : ""; *p && nr < MAX_ROWS; ) {
		int cl = utf8_len((unsigned char)*p);
		if (rl + cl >= ROW_BUF - 1) break;
		memcpy(s->rows[nr] + rl, p, cl);
		s->rows[nr][rl + cl] = 0;
		if (text_width(s->rows[nr]) > maxw && rl > 0) {
			s->rows[nr][rl] = 0;          // 今の文字を外して
			if (++nr >= MAX_ROWS) { nr = MAX_ROWS - 1; break; }
			memcpy(s->rows[nr], p, cl);   // 次の行の先頭へ
			s->rows[nr][cl] = 0;
			rl = cl;
		} else {
			rl += cl;
		}
		p += cl;
	}
	s->nrows = nr + 1;

	s->total_cp = 0;
	for (int i = 0; i < s->nrows; i++)
		s->total_cp += utf8_count(s->rows[i]);
	s->reveal = 0;
	s->timer = 0.0f;
}

static void enter(Scene* self, void* arg)
{
	DlgState* s = mem_alloc(sizeof(DlgState));
	DialogueScript* sc = arg;
	s->lines = sc ? sc->lines : NULL;
	s->count = sc ? sc->count : 0;
	s->name = sc ? sc->name : NULL;
	s->choices = sc ? sc->choices : NULL;
	s->choice_count = sc ? sc->choice_count : 0;
	s->choosing = 0;
	s->sel = 0;
	s->cur = 0;
	self->state = s;
	if (s->count > 0)
		layout(s);
	else
		s->nrows = 0, s->total_cp = 0, s->reveal = 0;
}

static void update(Scene* self, float dt)
{
	DlgState* s = self->state;

	// 選択肢モード
	if (s->choosing) {
		if (btn_pressed(kButtonUp))   s->sel = (s->sel - 1 + s->choice_count) % s->choice_count;
		if (btn_pressed(kButtonDown)) s->sel = (s->sel + 1) % s->choice_count;
		if (btn_pressed(kButtonA))    scene_pop(INT_TO_ARG(s->sel));   // 選んだindexを返す
		if (btn_pressed(kButtonB))    scene_pop(NULL);                 // キャンセル(=0)
		return;
	}

	// タイプライター進行
	if (s->reveal < s->total_cp) {
		s->timer += dt;
		while (s->timer >= 1.0f / CPS && s->reveal < s->total_cp) {
			s->timer -= 1.0f / CPS;
			s->reveal++;
		}
	}

	if (btn_pressed(kButtonB)) {
		scene_pop(NULL);
		return;
	}
	if (btn_pressed(kButtonA)) {
		if (s->reveal < s->total_cp) {
			s->reveal = s->total_cp;          // 一気に全文表示
		} else if (s->cur + 1 < s->count) {
			s->cur++;                          // 次の行
			layout(s);
		} else if (s->choice_count > 0) {
			s->choosing = 1;                   // 最後の行 → 選択肢へ
		} else {
			scene_pop(NULL);                   // 最後 → 閉じる
		}
	}
}

static void draw(Scene* self)
{
	DlgState* s = self->state;

	// 名前ボックス（あれば枠の左上に）
	if (s->name) {
		int nw = text_width(s->name) + 16;
		pd->graphics->fillRect(BOX_X, BOX_Y - 22, nw, 22, kColorWhite);
		pd->graphics->drawRect(BOX_X, BOX_Y - 22, nw, 22, kColorBlack);
		draw_text(s->name, BOX_X + 8, BOX_Y - 18);
	}

	// 枠
	pd->graphics->fillRect(BOX_X, BOX_Y, BOX_W, BOX_H, kColorWhite);
	pd->graphics->drawRect(BOX_X, BOX_Y, BOX_W, BOX_H, kColorBlack);

	// 選択肢モード：選択肢を一覧表示
	if (s->choosing) {
		for (int i = 0; i < s->choice_count; i++) {
			int y = BOX_Y + PAD + i * LINE_H;
			char line[ROW_BUF];
			snprintf(line, sizeof(line), "%s %s", i == s->sel ? ">" : " ", s->choices[i]);
			draw_text(line, BOX_X + PAD, y);
		}
		return;
	}

	// 本文（reveal 文字まで）
	int remaining = s->reveal;
	int y = BOX_Y + PAD;
	for (int i = 0; i < s->nrows; i++) {
		int row_cp = utf8_count(s->rows[i]);
		int show = remaining < row_cp ? remaining : row_cp;
		if (show > 0) {
			char buf[ROW_BUF];
			int b = utf8_bytes_for(s->rows[i], show);
			memcpy(buf, s->rows[i], b);
			buf[b] = 0;
			draw_text(buf, BOX_X + PAD, y);
		}
		remaining -= show;
		y += LINE_H;
	}

	// 全文表示し終わったら▼（送り）を点滅
	if (s->reveal >= s->total_cp && (pd->system->getCurrentTimeMilliseconds() / 400) % 2 == 0)
		draw_text_right("\xe2\x96\xbc", BOX_X + BOX_W - 8, BOX_Y + BOX_H - 18);  // ▼
}

static void leave(Scene* self)
{
	mem_free(self->state);
}

Scene scene_dialogue(void)
{
	return (Scene){
		.enter = enter, .update = update, .draw = draw,
		.leave = leave, .draw_below = 1,
	};
}
