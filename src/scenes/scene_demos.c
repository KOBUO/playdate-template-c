#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "audio.h"

// Demos ハブ：各機能のミニデモを一覧から開く（開発ガイドのサンプル集の入口）。
// ⊙メニューの "Demos" から push される。A で各デモへ、B で戻る。
// 新しいデモを足すときは下の demos[] に1行追加 + デモシーンを作るだけ。

typedef struct {
	StringID label;
	SceneID  scene;
} DemoEntry;

static const DemoEntry demos[] = {
	{ STR_DEMO_CRANK,  SCENE_DEMO_CRANK },
	{ STR_DEMO_ACCEL,  SCENE_DEMO_ACCEL },
	{ STR_DEMO_SPRITE, SCENE_DEMO_SPRITE },
};
#define DEMO_COUNT ((int)(sizeof(demos) / sizeof(demos[0])))

typedef struct { int selected; } DemosState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	DemosState* s = mem_alloc(sizeof(DemosState));
	s->selected = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	(void)dt;
	DemosState* s = self->state;

	if (btn_pressed(kButtonUp))   { s->selected = (s->selected - 1 + DEMO_COUNT) % DEMO_COUNT; audio_play(SFX_SELECT); }
	if (btn_pressed(kButtonDown)) { s->selected = (s->selected + 1) % DEMO_COUNT; audio_play(SFX_SELECT); }
	if (btn_pressed(kButtonB))    scene_pop(NULL);
	if (btn_pressed(kButtonA))    { audio_play(SFX_CONFIRM); scene_push(demos[s->selected].scene, NULL); }
}

static void draw(Scene* self)
{
	DemosState* s = self->state;

	pd->graphics->clear(kColorWhite);
	draw_text_centered(tr(STR_DEMOS), 34);

	const int row_h = 30, cx = SCREEN_WIDTH / 2;
	int y0 = 92;
	for (int i = 0; i < DEMO_COUNT; i++) {
		int y = y0 + i * row_h;
		const char* label = tr(demos[i].label);
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

Scene scene_demos(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
