#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "sysmenu.h"
#include "save.h"
#include "dialogue.h"
#include <stdio.h>

// 本編（プレースホルダ）：
//  - ステージセレクトから渡されたステージ番号(arg)を受け取る
//  - 方向キーで四角を移動
//  - A でポーズ画面を「重ねて」表示（push）
//  - B でステージセレクトへ戻る（フェード）
//  - ポーズが「Quit」を返したら resume で受け取り戻る

typedef struct {
	int x, y;
	int stage;   // ステージセレクトから受け取った番号(0始まり)
} GameState;

static const int BOX_SIZE  = 20;
static const int BOX_SPEED = 3;

static void enter(Scene* self, void* arg)
{
	GameState* s = mem_alloc(sizeof(GameState));
	s->x = (SCREEN_WIDTH  - BOX_SIZE) / 2;
	s->y = (SCREEN_HEIGHT - BOX_SIZE) / 2;
	s->stage = ARG_TO_INT(arg);
	self->state = s;
}

// ステージを終えてセレクトへ戻る。デモなのでプレイ=クリア扱いで保存する
// （実ゲームでは「勝利条件を満たしたら」save_mark_stage_cleared を呼ぶ）。
static void finish_stage(Scene* self)
{
	GameState* s = self->state;
	save_mark_stage_cleared(s->stage);
	save_store();
	scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
}

static void update(Scene* self, float dt)
{
	(void)dt;
	GameState* s = self->state;

	if (btn_down(kButtonLeft))  s->x -= BOX_SPEED;
	if (btn_down(kButtonRight)) s->x += BOX_SPEED;
	if (btn_down(kButtonUp))    s->y -= BOX_SPEED;
	if (btn_down(kButtonDown))  s->y += BOX_SPEED;

	if (s->x < 0) s->x = 0;
	if (s->y < 0) s->y = 0;
	if (s->x > SCREEN_WIDTH  - BOX_SIZE) s->x = SCREEN_WIDTH  - BOX_SIZE;
	if (s->y > SCREEN_HEIGHT - BOX_SIZE) s->y = SCREEN_HEIGHT - BOX_SIZE;

	if (btn_pressed(kButtonA))
		scene_push(SCENE_PAUSE, NULL);            // ポーズを重ねる

	if (btn_pressed(kButtonB))
		finish_stage(self);
}

// 重ねた画面(ポーズ/会話)が pop した時に呼ばれる。result はその画面が渡した値。
static void resume(Scene* self, void* result)
{
	if (ARG_TO_INT(result) == 1)              // ポーズで Quit が選ばれた
		finish_stage(self);
	// 会話は NULL(=0) を返すので何もしない
}

// --- ⊙ システムメニューの項目（最大3つまで・英語固定）---
static void on_menu_quit(void* ud)
{
	(void)ud;
	scene_switch(SCENE_TITLE, NULL, TRANSITION_FADE);
}

// 会話ウィンドウのデモ。現在の言語の文言を会話スクリプトにして push する。
static const char*    s_demo_lines[3];
static DialogueScript s_demo_script;
static void on_menu_talk(void* ud)
{
	(void)ud;
	s_demo_lines[0] = tr(STR_DEMO_1);
	s_demo_lines[1] = tr(STR_DEMO_2);
	s_demo_lines[2] = tr(STR_DEMO_3);
	s_demo_script.lines = s_demo_lines;
	s_demo_script.count = 3;
	scene_push(SCENE_DIALOGUE, &s_demo_script);
}

static void build_menu(Scene* self)
{
	(void)self;
	sysmenu_add_settings();                                       // 共通部品
	pd->system->addMenuItem("Talk (demo)", on_menu_talk, NULL);   // 独自：会話デモ
	pd->system->addMenuItem("Quit to Title", on_menu_quit, NULL); // 独自：タイトルへ
}

static void draw(Scene* self)
{
	GameState* s = self->state;

	char head[32];
	snprintf(head, sizeof(head), "%s %d", tr(STR_STAGE), s->stage + 1);
	draw_text_centered(head, 10);

	pd->graphics->fillRect(s->x, s->y, BOX_SIZE, BOX_SIZE, kColorBlack);
	draw_text_centered(tr(STR_GAME_HINT), 222);
}

static void leave(Scene* self)
{
	mem_free(self->state);
}

Scene scene_game(void)
{
	return (Scene){
		.enter = enter, .update = update, .draw = draw,
		.resume = resume, .leave = leave, .build_menu = build_menu,
	};
}
