#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "sysmenu.h"
#include "audio.h"
#include "save.h"
#include <stdio.h>

// ステージセレクト画面：タイトルの次。ステージを選んで本編へ。
//  - 上下で選択、A で決定（選んだステージ番号を本編に arg で渡す）、B でタイトルへ
//  - 選択行は反転バー（黒地に白文字）
//  - Settings / Credits は ⊙ メニューから（リストには置かない）

#define STAGE_COUNT 3

typedef struct {
	int selected;
} MenuState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	MenuState* s = mem_alloc(sizeof(MenuState));
	s->selected = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	(void)dt;
	MenuState* s = self->state;

	if (btn_pressed(kButtonUp)) {
		s->selected = (s->selected - 1 + STAGE_COUNT) % STAGE_COUNT;
		audio_play(SFX_SELECT);
	}
	if (btn_pressed(kButtonDown)) {
		s->selected = (s->selected + 1) % STAGE_COUNT;
		audio_play(SFX_SELECT);
	}

	if (btn_pressed(kButtonB))
		scene_switch(SCENE_TITLE, NULL, TRANSITION_FADE);

	if (btn_pressed(kButtonA)) {
		audio_play(SFX_CONFIRM);
		scene_switch(SCENE_GAME, INT_TO_ARG(s->selected), TRANSITION_FADE);
	}
}

static void draw(Scene* self)
{
	MenuState* s = self->state;

	draw_text_centered(tr(STR_STAGE_SELECT), 34);

	const int row_h = 32;
	const int cx = SCREEN_WIDTH / 2;
	int y0 = 92;

	for (int i = 0; i < STAGE_COUNT; i++) {
		int y = y0 + i * row_h;
		char label[40];
		// クリア済みは ★ を付ける（★は収録フォントにあるJIS記号）
		if (save_is_stage_cleared(i))
			snprintf(label, sizeof(label), "%s %d \xe2\x98\x85", tr(STR_STAGE), i + 1);
		else
			snprintf(label, sizeof(label), "%s %d", tr(STR_STAGE), i + 1);
		int tw = text_width(label);

		if (i == s->selected) {
			int bw = tw + 48;
			pd->graphics->fillRect(cx - bw / 2, y - 5, bw, 24, kColorBlack);  // 反転バー
			pd->graphics->setDrawMode(kDrawModeFillWhite);
			draw_text(label, cx - tw / 2, y);
			pd->graphics->setDrawMode(kDrawModeCopy);
		} else {
			draw_text(label, cx - tw / 2, y);
		}
	}

	draw_text_centered(tr(STR_MENU_HINT), 216);
}

static void leave(Scene* self)
{
	mem_free(self->state);
}

// ⊙ システムメニュー：Settings / Credits はここから
static void build_menu(Scene* self)
{
	(void)self;
	sysmenu_add_settings();
	sysmenu_add_demos();
	sysmenu_add_credits();
}

Scene scene_stage_select(void)
{
	return (Scene){
		.enter = enter, .update = update, .draw = draw,
		.leave = leave, .build_menu = build_menu,
	};
}
