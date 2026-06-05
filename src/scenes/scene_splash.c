#include "scenes.h"
#include "globals.h"
#include "input.h"
#include <string.h>

// スプラッシュ（オープニング）：起動時に少しだけ表示してタイトルへ。
// コードだけの軽量演出（動画ファイル不要）。A でスキップ。
// 不要なら main.c の最初の遷移を SCENE_TITLE に変えるだけで外せる。
// 本物の動画が要るなら pd->graphics->video（.pdv）を使う（上級）。

#define HOLD 1.8f   // 表示時間（秒）

typedef struct { float t; } SplashState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	SplashState* s = mem_alloc(sizeof(SplashState));
	s->t = 0;
	self->state = s;
}

static void update(Scene* self, float dt)
{
	SplashState* s = self->state;
	s->t += dt;
	if (s->t >= HOLD || btn_pressed(kButtonA))
		scene_switch(SCENE_TITLE, NULL, TRANSITION_FADE);
}

static void draw(Scene* self)
{
	SplashState* s = self->state;
	pd->graphics->clear(kColorWhite);

	const char* who = "kobuo";
	draw_text_centered(who, 104);

	// 名前の下に、時間に応じて伸びる下線（ちょっとした演出）
	int tw = text_width(who);
	int full = tw + 24;
	float p = s->t / 0.7f; if (p > 1) p = 1;
	int w = (int)(full * p);
	pd->graphics->fillRect((SCREEN_WIDTH - w) / 2, 126, w, 2, kColorBlack);

	if (s->t > 0.8f)
		draw_text_centered("A: skip", 210);
}

static void leave(Scene* self) { mem_free(self->state); }

Scene scene_splash(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
