#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"

// ポーズ画面：本編の上に「重ねて」表示するオーバーレイ。
//  - draw_below = 1 なので、下の本編もそのまま描画される
//  - A で再開（pop して result=0）
//  - B で終了（pop して result=1 → 本編の resume がメニューへ）

static void update(Scene* self, float dt)
{
	(void)self;
	(void)dt;

	if (btn_pressed(kButtonA))
		scene_pop(INT_TO_ARG(0));   // 再開
	if (btn_pressed(kButtonB))
		scene_pop(INT_TO_ARG(1));   // 終了
}

static void draw(Scene* self)
{
	(void)self;

	int w = 240, h = 110;
	int x = (SCREEN_WIDTH - w) / 2;
	int y = (SCREEN_HEIGHT - h) / 2;
	pd->graphics->fillRect(x, y, w, h, kColorWhite);
	pd->graphics->drawRect(x, y, w, h, kColorBlack);

	draw_text_centered(tr(STR_PAUSED), y + 20);
	draw_text_centered(tr(STR_RESUME), y + 52);
	draw_text_centered(tr(STR_QUIT_TO_STAGES), y + 76);
}

Scene scene_pause(void)
{
	return (Scene){ .update = update, .draw = draw, .draw_below = 1 };
}
