#include "scenes.h"
#include "globals.h"
#include "input.h"
#include <stdio.h>
#include <math.h>

// クランク・ロード デモ：Playdate 定番の「クランクを回してロード」演出。
//  - クランクの回転量を貯めて進捗(0〜100%)を進める
//  - 回転に合わせてアニメ(8コマ imagetable)が回る
//  - 100% で完了表示。B で戻る
// 画像: Source/images/crankload-table-32-32.png（tools/gen_crankload.py で生成）

#define FRAMES   8
#define FRAME_PX 48
#define SCALE    2
#define FULL_DEG 720.0f   // 2回転で満タン

typedef struct {
	LCDBitmapTable* table;
	float accum;      // 累積回転量(度)
	float progress;   // 0..1
} CLState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	CLState* s = mem_alloc(sizeof(CLState));
	const char* err;
	s->table = pd->graphics->loadBitmapTable("images/crankload", &err);
	if (!s->table)
		pd->system->logToConsole("crankload: %s", err);
	s->accum = 0;
	s->progress = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	(void)dt;
	CLState* s = self->state;

	if (!crank_docked()) {
		float c = fabsf(crank_change());     // このフレームの回転量(度)
		s->accum += c;
		s->progress += c / FULL_DEG;
		if (s->progress > 1.0f) s->progress = 1.0f;
	}

	if (btn_pressed(kButtonB))
		scene_pop(NULL);
}

static void draw(Scene* self)
{
	CLState* s = self->state;

	pd->graphics->clear(kColorWhite);
	draw_text_centered("CRANK LOADING", 16);

	// 回転アニメ（拡大して中央に）
	int frame = ((int)(s->accum / 30.0f)) % FRAMES;
	int sz = FRAME_PX * SCALE;
	LCDBitmap* bmp = s->table ? pd->graphics->getTableBitmap(s->table, frame) : NULL;
	if (bmp)
		pd->graphics->drawScaledBitmap(bmp, (SCREEN_WIDTH - sz) / 2, 52, SCALE, SCALE);

	// 進捗バー
	int bx = 80, by = 175, bw = SCREEN_WIDTH - bx * 2, bh = 14;
	pd->graphics->drawRect(bx, by, bw, bh, kColorBlack);
	pd->graphics->fillRect(bx + 2, by + 2, (int)((bw - 4) * s->progress), bh - 4, kColorBlack);

	char buf[48];
	if (crank_docked())
		draw_text_centered("undock the crank", 150);
	else if (s->progress >= 1.0f)
		draw_text_centered("Loaded!", 150);
	else {
		snprintf(buf, sizeof(buf), "crank to load... %d%%", (int)(s->progress * 100));
		draw_text_centered(buf, 150);
	}

	draw_text_centered("B: back", 222);
}

static void leave(Scene* self)
{
	CLState* s = self->state;
	if (s->table)
		pd->graphics->freeBitmapTable(s->table);
	mem_free(s);
}

Scene scene_demo_crankload(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
