#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"

// クレジット画面：メインメニューから push される。Bで戻る。
// 名前など固定の文字列は翻訳不要なのでそのまま書く。

static void update(Scene* self, float dt)
{
	(void)self;
	(void)dt;
	if (btn_pressed(kButtonA) || btn_pressed(kButtonB))
		scene_pop(NULL);
}

static void draw(Scene* self)
{
	(void)self;

	pd->graphics->clear(kColorWhite);
	draw_text_centered(tr(STR_CREDITS), 40);

	draw_text_centered(tr(STR_CREDITS_BODY), 110);
	draw_text_centered("kobuo", 140);   // 固定（翻訳不要）

	draw_text_centered(tr(STR_RETURN_HINT), 210);
}

Scene scene_credits(void)
{
	return (Scene){ .update = update, .draw = draw };
}
