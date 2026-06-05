#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "save.h"
#include "audio.h"
#include "sysmenu.h"
#include "dialogue.h"
#include <math.h>

// ステージ「クランク」：クランクで針を回し、的（マーカー）に合わせて A でクリア。
// 金庫破りのような操作。導入で操作説明を会話表示。

#define TARGET 210.0f   // 的の角度（度・0=上）
#define TOLER  16.0f    // 許容ずれ（度）
#define CX 200
#define CY 124
#define R  56

typedef struct { int won; float wt; int stage; } St;

static const char*    s_intro[1];
static DialogueScript s_dlg;

static void enter(Scene* self, void* arg)
{
	St* s = mem_alloc(sizeof(St));
	s->won = 0; s->wt = 0; s->stage = ARG_TO_INT(arg);
	self->state = s;
	s_intro[0] = tr(STR_HINT_CRANK);
	s_dlg.lines = s_intro; s_dlg.count = 1;
	scene_push(SCENE_DIALOGUE, &s_dlg);
}

static void update(Scene* self, float dt)
{
	St* s = self->state;
	if (s->won) {
		s->wt += dt;
		if (s->wt >= 1.0f) scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
		return;
	}

	if (btn_pressed(kButtonA) && !crank_docked()) {
		float diff = crank_angle() - TARGET;
		while (diff > 180) diff -= 360;
		while (diff < -180) diff += 360;
		if (fabsf(diff) <= TOLER) {
			s->won = 1; s->wt = 0;
			save_mark_stage_cleared(s->stage); save_store();
			audio_play(SFX_CONFIRM);
		}
	}

	if (btn_pressed(kButtonB))
		scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
}

static void draw(Scene* self)
{
	St* s = self->state;
	pd->graphics->clear(kColorWhite);

	pd->graphics->drawEllipse(CX - R, CY - R, R * 2, R * 2, 2, 0, 0, kColorBlack);   // ダイヤル

	// 的（マーカー）
	float ta = TARGET * (float)M_PI / 180.0f;
	int tx = CX + (int)(sinf(ta) * R), ty = CY - (int)(cosf(ta) * R);
	pd->graphics->fillEllipse(tx - 6, ty - 6, 12, 12, 0, 0, kColorBlack);

	if (crank_docked()) {
		draw_text_centered("undock the crank", CY + R + 18);
	} else {
		float a = crank_angle() * (float)M_PI / 180.0f;            // 針
		int nx = CX + (int)(sinf(a) * (R - 8)), ny = CY - (int)(cosf(a) * (R - 8));
		pd->graphics->drawLine(CX, CY, nx, ny, 3, kColorBlack);
		pd->graphics->fillRect(CX - 3, CY - 3, 6, 6, kColorBlack);
	}

	draw_text_centered(tr(STR_HINT_CRANK), 224);
	if (s->won) draw_text_centered(tr(STR_CLEAR), 16);
}

static void leave(Scene* self) { mem_free(self->state); }
static void build_menu(Scene* self) { (void)self; sysmenu_add_settings(); sysmenu_add_quit_to_title(); }

Scene scene_stage_crank(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave, .build_menu = build_menu };
}
