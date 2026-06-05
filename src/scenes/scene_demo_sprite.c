#include "scenes.h"
#include "globals.h"
#include "input.h"
#include <stdio.h>

// スプライト＆アニメ デモ：
//  - imagetable（コマを並べた画像）を loadBitmapTable で読み込み
//  - 左右キーで歩行アニメ（コマ送り）＋向き反転（drawBitmap の flip）
//  - 止まると待機コマ
// 画像: Source/images/anim-table-32-32.png（tools/gen_sprites.py で生成）

#define FRAMES   4
#define SPRITE   32
#define SPEED    2.5f
#define FRAME_T  0.12f   // 1コマの表示秒

typedef struct {
	LCDBitmapTable* table;
	float x;
	int   frame;
	float timer;
	int   facing_left;
} SprState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	SprState* s = mem_alloc(sizeof(SprState));
	const char* err;
	s->table = pd->graphics->loadBitmapTable("images/anim", &err);
	if (s->table == NULL)
		pd->system->logToConsole("sprite demo: %s", err);
	s->x = (SCREEN_WIDTH - SPRITE) / 2.0f;
	s->frame = 0;
	s->timer = 0;
	s->facing_left = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	SprState* s = self->state;

	int moving = 0;
	if (btn_down(kButtonLeft))  { s->x -= SPEED; s->facing_left = 1; moving = 1; }
	if (btn_down(kButtonRight)) { s->x += SPEED; s->facing_left = 0; moving = 1; }
	if (s->x < 0) s->x = 0;
	if (s->x > SCREEN_WIDTH - SPRITE) s->x = SCREEN_WIDTH - SPRITE;

	if (moving) {                       // 歩いている間だけコマ送り
		s->timer += dt;
		while (s->timer >= FRAME_T) {
			s->timer -= FRAME_T;
			s->frame = (s->frame + 1) % FRAMES;
		}
	} else {
		s->frame = 0;                   // 待機コマ
	}

	if (btn_pressed(kButtonB))
		scene_pop(NULL);
}

static void draw(Scene* self)
{
	SprState* s = self->state;

	pd->graphics->clear(kColorWhite);
	draw_text_centered("SPRITE & ANIMATION", 16);

	int y = 120;
	LCDBitmap* bmp = s->table ? pd->graphics->getTableBitmap(s->table, s->frame) : NULL;
	if (bmp)
		pd->graphics->drawBitmap(bmp, (int)s->x, y,
		                         s->facing_left ? kBitmapFlippedX : kBitmapUnflipped);

	char buf[32];
	snprintf(buf, sizeof(buf), "frame: %d", s->frame);
	draw_text_centered(buf, 70);
	draw_text_centered("D-pad: walk      B: back", 222);
}

static void leave(Scene* self)
{
	SprState* s = self->state;
	if (s->table)
		pd->graphics->freeBitmapTable(s->table);
	mem_free(s);
}

Scene scene_demo_sprite(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
