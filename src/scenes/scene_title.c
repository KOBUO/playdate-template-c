#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "sysmenu.h"

// タイトル画面：背景画像 + 点滅する「Press A」。Aでメニューへ。
// 画像は Source/images/title.png（拡張子なしで "images/title" を指定）。
// 本番アートに差し替えるときはこのファイルを置き換えるだけ。

typedef struct {
	LCDBitmap* bg;
	float blink_timer;
	int   show_prompt;
} TitleState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	TitleState* s = mem_alloc(sizeof(TitleState));
	s->bg = img_load("images/title");
	s->blink_timer = 0.0f;
	s->show_prompt = 1;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	TitleState* s = self->state;

	s->blink_timer += dt;
	if (s->blink_timer >= 0.5f) {
		s->blink_timer = 0.0f;
		s->show_prompt = !s->show_prompt;
	}

	if (btn_pressed(kButtonA))
		scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
}

static void draw(Scene* self)
{
	TitleState* s = self->state;

	img_draw(s->bg, 0, 0);              // 背景画像（全画面）
	if (s->show_prompt)
		draw_text_centered(tr(STR_PRESS_A), 222);   // 横中央・下部
}

static void leave(Scene* self)
{
	TitleState* s = self->state;
	img_free(s->bg);
	mem_free(s);
}

// ⊙ システムメニュー：タイトル画面からも Settings / Credits を選べる（共通部品）
static void build_menu(Scene* self)
{
	(void)self;
	sysmenu_add_settings();
	sysmenu_add_credits();
}

Scene scene_title(void)
{
	return (Scene){
		.enter = enter, .update = update, .draw = draw,
		.leave = leave, .build_menu = build_menu,
	};
}
