#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "sysmenu.h"
#include "audio.h"
#include "save.h"
#include <stdio.h>

// ステージセレクト：各ステージ＝操作別のミニゲーム。
//  - 上下で選択、A で「ローディング → そのステージ」、B でタイトルへ
//  - クリア済みは ★ 表示
//  - Settings / Credits は ⊙ メニューから

typedef struct {
	StringID label;
	SceneID  scene;
} StageDef;

static const StageDef stages[] = {
	{ STR_STAGE_MOVE,  SCENE_STAGE_MOVE  },
	{ STR_STAGE_CRANK, SCENE_STAGE_CRANK },
	{ STR_STAGE_TILT,  SCENE_STAGE_TILT  },
};
#define STAGE_COUNT ((int)(sizeof(stages) / sizeof(stages[0])))

typedef struct { int selected; } SelState;

static LoadRequest s_req;   // ローディングへ渡す（次フレームまで生存させるため static）

static void enter(Scene* self, void* arg)
{
	(void)arg;
	SelState* s = mem_alloc(sizeof(SelState));
	s->selected = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	(void)dt;
	SelState* s = self->state;

	if (btn_pressed(kButtonUp))   { s->selected = (s->selected - 1 + STAGE_COUNT) % STAGE_COUNT; audio_play(SFX_SELECT); }
	if (btn_pressed(kButtonDown)) { s->selected = (s->selected + 1) % STAGE_COUNT; audio_play(SFX_SELECT); }
	if (btn_pressed(kButtonB))    scene_switch(SCENE_TITLE, NULL, TRANSITION_FADE);

	if (btn_pressed(kButtonA)) {
		audio_play(SFX_CONFIRM);
		// ローディングを経由してステージへ（ローディング中にアニメ表示）
		s_req.next  = stages[s->selected].scene;
		s_req.param = s->selected;
		scene_switch(SCENE_LOADING, &s_req, TRANSITION_FADE);
	}
}

static void draw(Scene* self)
{
	SelState* s = self->state;

	draw_text_centered(tr(STR_STAGE_SELECT), 34);

	const int row_h = 32, cx = SCREEN_WIDTH / 2;
	int y0 = 92;
	for (int i = 0; i < STAGE_COUNT; i++) {
		int y = y0 + i * row_h;
		char label[48];
		if (save_is_stage_cleared(i))
			snprintf(label, sizeof(label), "%s \xe2\x98\x85", tr(stages[i].label));  // ★
		else
			snprintf(label, sizeof(label), "%s", tr(stages[i].label));
		int tw = text_width(label);

		if (i == s->selected) {
			pd->graphics->fillRect(cx - (tw + 48) / 2, y - 5, tw + 48, 24, kColorBlack);
			pd->graphics->setDrawMode(kDrawModeFillWhite);
			draw_text(label, cx - tw / 2, y);
			pd->graphics->setDrawMode(kDrawModeCopy);
		} else {
			draw_text(label, cx - tw / 2, y);
		}
	}

	draw_text_centered(tr(STR_MENU_HINT), 216);
}

static void leave(Scene* self) { mem_free(self->state); }

static void build_menu(Scene* self)
{
	(void)self;
	sysmenu_add_settings();
	sysmenu_add_credits();
}

Scene scene_stage_select(void)
{
	return (Scene){
		.enter = enter, .update = update, .draw = draw,
		.leave = leave, .build_menu = build_menu,
	};
}
