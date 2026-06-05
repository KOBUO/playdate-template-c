#include "scenes.h"
#include "globals.h"
#include "i18n.h"
#include <stdio.h>

// ローディング画面：ステージ開始前に挟む「読み込み中」演出。
// 実際の読み込みは一瞬で終わるので、ここでは擬似的に LOAD_TIME 秒だけ待つ。
// （注：本物の sleep でブロックするとアニメが止まるので、タイマーで待つ）
// ロードアニメ: Source/images/loading-table-40-28.png（tools/gen_loading.py）

#define FW 40
#define FH 28
#define FRAMES 8
#define SCALE 2
#define LOAD_TIME 1.6f

typedef struct {
	LCDBitmapTable* table;
	SceneID next;
	int     param;
	float   t;
} LoadState;

static void enter(Scene* self, void* arg)
{
	LoadState* s = mem_alloc(sizeof(LoadState));
	LoadRequest* r = arg;
	const char* err;
	s->table = pd->graphics->loadBitmapTable("images/loading", &err);
	if (!s->table) pd->system->logToConsole("loading: %s", err);
	s->next  = r ? r->next : SCENE_TITLE;
	s->param = r ? r->param : 0;
	s->t = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	LoadState* s = self->state;
	s->t += dt;
	if (s->t >= LOAD_TIME)
		scene_switch(s->next, INT_TO_ARG(s->param), TRANSITION_FADE);
}

static void draw(Scene* self)
{
	LoadState* s = self->state;
	pd->graphics->clear(kColorWhite);

	// アニメ（中央・拡大）
	int frame = ((int)(s->t * 12.0f)) % FRAMES;
	int w = FW * SCALE, h = FH * SCALE;
	LCDBitmap* bmp = s->table ? pd->graphics->getTableBitmap(s->table, frame) : NULL;
	if (bmp)
		pd->graphics->drawScaledBitmap(bmp, (SCREEN_WIDTH - w) / 2, 100, SCALE, SCALE);

	// "Loading" + 動くドット
	int dots = ((int)(s->t * 2.0f)) % 4;
	char buf[32];
	snprintf(buf, sizeof(buf), "%s%.*s", tr(STR_LOADING), dots, "...");
	draw_text_centered(buf, 150);
}

static void leave(Scene* self)
{
	LoadState* s = self->state;
	if (s->table) pd->graphics->freeBitmapTable(s->table);
	mem_free(s);
}

Scene scene_loading(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
