#include "scenes.h"
#include "globals.h"
#include "input.h"
#include <stdio.h>

// 加速度センサ デモ：本体の傾きを読む。
//  - 使う前に setPeripheralsEnabled(kAccelerometer) で有効化（電池節約のため既定はOFF）
//  - getAccelerometer(&x,&y,&z) で重力方向ベクトル(およそ -1..1)
//  - 使い終わったら kNone に戻す
// シミュレータは下部の Accelerometer のUIで傾きを与えられる。

typedef struct { float bx, by; } AccelState;
#define BALL 14

static void enter(Scene* self, void* arg)
{
	(void)arg;
	AccelState* s = mem_alloc(sizeof(AccelState));
	s->bx = (SCREEN_WIDTH - BALL) / 2.0f;
	s->by = (SCREEN_HEIGHT - BALL) / 2.0f;
	self->state = s;
	pd->system->setPeripheralsEnabled(kAccelerometer);   // 有効化
}

static void update(Scene* self, float dt)
{
	AccelState* s = self->state;

	float x, y, z;
	pd->system->getAccelerometer(&x, &y, &z);
	s->bx += x * 220.0f * dt;     // 傾きで転がす
	s->by += y * 220.0f * dt;
	if (s->bx < 0) s->bx = 0;
	if (s->by < 0) s->by = 0;
	if (s->bx > SCREEN_WIDTH - BALL)  s->bx = SCREEN_WIDTH - BALL;
	if (s->by > SCREEN_HEIGHT - BALL) s->by = SCREEN_HEIGHT - BALL;

	if (btn_pressed(kButtonB))
		scene_pop(NULL);
}

static void draw(Scene* self)
{
	AccelState* s = self->state;
	pd->graphics->clear(kColorWhite);
	draw_text_centered("ACCELEROMETER", 16);

	float x, y, z;
	pd->system->getAccelerometer(&x, &y, &z);
	char buf[80];
	snprintf(buf, sizeof(buf), "x:%.2f  y:%.2f  z:%.2f", x, y, z);
	draw_text_centered(buf, 40);

	pd->graphics->fillEllipse((int)s->bx, (int)s->by, BALL, BALL, 0, 0, kColorBlack);
	draw_text_centered("tilt to roll      B: back", 222);
}

static void leave(Scene* self)
{
	pd->system->setPeripheralsEnabled(kNone);   // 使い終わったら無効化
	mem_free(self->state);
}

Scene scene_demo_accel(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
