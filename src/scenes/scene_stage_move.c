#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "save.h"
#include "audio.h"
#include "sysmenu.h"
#include "dialogue.h"

// ステージ「移動」：十字キーで歩いてゴールへ。到達でクリア。
// 歩行アニメは images/anim（32x32 x4コマ）を流用。導入で操作説明を会話表示。

#define SPR 32
#define FRAMES 4
#define SPEED 2.5f
#define FRAME_T 0.12f
#define GX 330
#define GY 40
#define GS 44

typedef struct {
	LCDBitmapTable* tbl;
	float x, y, ft;
	int frame, facing, won;
	float wt;
	int stage;
} St;

static const char*    s_intro[1];
static DialogueScript s_dlg;

static void enter(Scene* self, void* arg)
{
	St* s = mem_alloc(sizeof(St));
	const char* err;
	s->tbl = pd->graphics->loadBitmapTable("images/anim", &err);
	s->x = 30; s->y = 150; s->ft = 0; s->frame = 0; s->facing = 0; s->won = 0; s->wt = 0;
	s->stage = ARG_TO_INT(arg);
	self->state = s;
	// 導入：操作説明を会話で
	s_intro[0] = tr(STR_HINT_MOVE);
	s_dlg.lines = s_intro; s_dlg.count = 1;
	scene_push(SCENE_DIALOGUE, &s_dlg);
}

static void update(Scene* self, float dt)
{
	St* s = self->state;

	if (s->won) {                                  // クリア表示後に戻る
		s->wt += dt;
		if (s->wt >= 1.0f) scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
		return;
	}

	int moving = 0;
	if (btn_down(kButtonLeft))  { s->x -= SPEED; s->facing = 1; moving = 1; }
	if (btn_down(kButtonRight)) { s->x += SPEED; s->facing = 0; moving = 1; }
	if (btn_down(kButtonUp))    { s->y -= SPEED; moving = 1; }
	if (btn_down(kButtonDown))  { s->y += SPEED; moving = 1; }
	if (s->x < 0) s->x = 0;
	if (s->y < 0) s->y = 0;
	if (s->x > SCREEN_WIDTH - SPR)  s->x = SCREEN_WIDTH - SPR;
	if (s->y > SCREEN_HEIGHT - SPR) s->y = SCREEN_HEIGHT - SPR;

	if (moving) { s->ft += dt; while (s->ft >= FRAME_T) { s->ft -= FRAME_T; s->frame = (s->frame + 1) % FRAMES; } }
	else s->frame = 0;

	// ゴール判定（矩形重なり）
	if (s->x + SPR > GX && s->x < GX + GS && s->y + SPR > GY && s->y < GY + GS) {
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

	// ゴール
	pd->graphics->drawRect(GX, GY, GS, GS, kColorBlack);
	draw_text("GOAL", GX + 4, GY + GS / 2 - 6);

	// プレイヤー
	LCDBitmap* b = s->tbl ? pd->graphics->getTableBitmap(s->tbl, s->frame) : NULL;
	if (b) pd->graphics->drawBitmap(b, (int)s->x, (int)s->y, s->facing ? kBitmapFlippedX : kBitmapUnflipped);

	draw_text_centered(tr(STR_HINT_MOVE), 224);
	if (s->won) draw_text_centered(tr(STR_CLEAR), 16);
}

static void leave(Scene* self)
{
	St* s = self->state;
	if (s->tbl) pd->graphics->freeBitmapTable(s->tbl);
	mem_free(s);
}

static void build_menu(Scene* self) { (void)self; sysmenu_add_settings(); sysmenu_add_quit_to_title(); }

Scene scene_stage_move(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave, .build_menu = build_menu };
}
