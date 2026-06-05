#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "sysmenu.h"
#include "audio.h"
#include "save.h"
#include "tween.h"
#include <stdio.h>

#define ROW_H 32
#define ROW_Y0 92

// ステージセレクト：各ステージ＝操作別のミニゲーム。
//  - 上下で選択、A で「ローディング → そのステージ」、B でタイトルへ
//  - クリア済みは ★ 表示
//  - Settings / Credits は ⊙ メニューから

typedef struct {
	StringID label;
	SceneID  scene;
} StageDef;

static const StageDef stages[] = {
	{ STR_STAGE_MOVE,    SCENE_STAGE_MOVE    },
	{ STR_STAGE_CRANK,   SCENE_STAGE_CRANK   },
	{ STR_STAGE_TILT,    SCENE_STAGE_TILT    },
	{ STR_STAGE_TILEMAP, SCENE_STAGE_TILEMAP },
};
#define STAGE_COUNT ((int)(sizeof(stages) / sizeof(stages[0])))

typedef struct { int selected; float bar_y; } SelState;

static LoadRequest s_req;   // ローディングへ渡す（次フレームまで生存させるため static）

static void enter(Scene* self, void* arg)
{
	(void)arg;
	SelState* s = mem_alloc(sizeof(SelState));
	s->selected = 0;
	s->bar_y = ROW_Y0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	SelState* s = self->state;

	// 選択バーを目標行へなめらかに移動（トゥイーン）
	s->bar_y = tween_approach(s->bar_y, (float)(ROW_Y0 + s->selected * ROW_H), 20.0f, dt);

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

	draw_text_centered(tr(STR_STAGE_SELECT), 30);

	const int cx = SCREEN_WIDTH / 2;
	char labels[STAGE_COUNT][48];
	for (int i = 0; i < STAGE_COUNT; i++) {
		if (save_is_stage_cleared(i))
			snprintf(labels[i], sizeof(labels[i]), "%s \xe2\x98\x85", tr(stages[i].label));  // ★
		else
			snprintf(labels[i], sizeof(labels[i]), "%s", tr(stages[i].label));
	}

	// 選択バー（トゥイーンで移動中の bar_y に描く）
	int sel_tw = text_width(labels[s->selected]);
	pd->graphics->fillRect(cx - (sel_tw + 48) / 2, (int)s->bar_y - 5, sel_tw + 48, 24, kColorBlack);

	// ラベル（選択行は白文字）
	for (int i = 0; i < STAGE_COUNT; i++) {
		int y = ROW_Y0 + i * ROW_H;
		int tw = text_width(labels[i]);
		if (i == s->selected) {
			pd->graphics->setDrawMode(kDrawModeFillWhite);
			draw_text(labels[i], cx - tw / 2, y);
			pd->graphics->setDrawMode(kDrawModeCopy);
		} else {
			draw_text(labels[i], cx - tw / 2, y);
		}
	}

	draw_text_centered(tr(STR_MENU_HINT), 220);
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
