#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "save.h"
#include "audio.h"
#include "sysmenu.h"
#include "dialogue.h"

// ステージ「傾き」：本体を傾けてボールをゴールへ転がす。到達でクリア。
// 加速度センサを enter で有効化、leave で無効化。導入で操作説明を会話表示。

#define BALL 16
#define GX 330
#define GY 36
#define GS 48

typedef struct { float x, y; int won; float wt; int stage; } St;

static void enter(Scene* self, void* arg)
{
	St* s = mem_alloc(sizeof(St));
	s->x = 30; s->y = 180; s->won = 0; s->wt = 0; s->stage = ARG_TO_INT(arg);
	self->state = s;
	pd->system->setPeripheralsEnabled(kAccelerometer);
	dialogue_say(tr(STR_NAME_GUIDE), tr(STR_HINT_TILT));   // 導入：操作説明
}

static void update(Scene* self, float dt)
{
	St* s = self->state;
	if (s->won) {
		s->wt += dt;
		if (s->wt >= 1.0f) scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
		return;
	}

	float ax, ay, az;
	pd->system->getAccelerometer(&ax, &ay, &az);
	s->x += ax * 240.0f * dt;
	s->y += ay * 240.0f * dt;
	if (s->x < 0) s->x = 0;
	if (s->y < 0) s->y = 0;
	if (s->x > SCREEN_WIDTH - BALL)  s->x = SCREEN_WIDTH - BALL;
	if (s->y > SCREEN_HEIGHT - BALL) s->y = SCREEN_HEIGHT - BALL;

	if (s->x + BALL > GX && s->x < GX + GS && s->y + BALL > GY && s->y < GY + GS) {
		s->won = 1; s->wt = 0;
		save_mark_stage_cleared(s->stage); save_store();
		audio_play(SFX_CONFIRM);
	}

	if (btn_pressed(kButtonB))
		scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
}

static void draw(Scene* self)
{
	St* s = self->state;
	pd->graphics->clear(kColorWhite);

	pd->graphics->drawRect(GX, GY, GS, GS, kColorBlack);          // ゴール
	draw_text("GOAL", GX + 4, GY + GS / 2 - 6);
	pd->graphics->fillEllipse((int)s->x, (int)s->y, BALL, BALL, 0, 0, kColorBlack);  // ボール

	draw_text_centered(tr(STR_HINT_TILT), 224);
	if (s->won) draw_text_centered(tr(STR_CLEAR), 16);
}

static void leave(Scene* self)
{
	pd->system->setPeripheralsEnabled(kNone);
	mem_free(self->state);
}

static void build_menu(Scene* self) { (void)self; sysmenu_add_settings(); sysmenu_add_quit_to_title(); }

Scene scene_stage_tilt(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave, .build_menu = build_menu };
}
