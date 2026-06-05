#include "scenes.h"
#include "globals.h"
#include "input.h"
#include <stdio.h>
#include <math.h>

// クランク デモ：Playdate 固有の入力。
//  - getCrankAngle()  : 角度(度)。0=真上、時計回りに増加
//  - getCrankChange() : 前フレームからの変化量(度)
//  - isCrankDocked()  : 収納されているか
// シミュレータは右下のクランクUI（またはマウスドラッグ）で操作できる。

static void update(Scene* self, float dt) { (void)self; (void)dt; if (btn_pressed(kButtonB)) scene_pop(NULL); }

static void draw(Scene* self)
{
	(void)self;
	pd->graphics->clear(kColorWhite);
	draw_text_centered("CRANK", 16);

	int cx = SCREEN_WIDTH / 2, cy = 130, r = 60;
	pd->graphics->drawEllipse(cx - r, cy - r, r * 2, r * 2, 2, 0, 0, kColorBlack);  // ダイヤル

	if (crank_docked()) {
		draw_text_centered("(dock... undock the crank)", cy + r + 16);
	} else {
		float a = crank_angle() * (float)M_PI / 180.0f;          // 度→ラジアン
		int nx = cx + (int)(sinf(a) * (r - 6));                  // 0°=上
		int ny = cy - (int)(cosf(a) * (r - 6));
		pd->graphics->drawLine(cx, cy, nx, ny, 3, kColorBlack);  // 針
		pd->graphics->fillRect(cx - 3, cy - 3, 6, 6, kColorBlack);

		char buf[64];
		snprintf(buf, sizeof(buf), "angle: %d", (int)crank_angle());
		draw_text_centered(buf, cy + r + 14);
		snprintf(buf, sizeof(buf), "change/frame: %d", (int)crank_change());
		draw_text_centered(buf, cy + r + 36);
	}

	draw_text_centered("B: back", 222);
}

Scene scene_demo_crank(void)
{
	return (Scene){ .update = update, .draw = draw };
}
