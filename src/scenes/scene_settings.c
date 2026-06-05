#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "audio.h"
#include "save.h"

// 設定シーン：⊙メニューの "Settings" や メインメニューから push される自作画面。
// 上下で項目選択、左右/Aで変更、Bで戻る（pop）。

enum { ITEM_SOUND, ITEM_LANGUAGE, ITEM_CREDITS, ITEM_COUNT };

typedef struct {
	int selected;
} SettingsState;

static void enter(Scene* self, void* arg)
{
	(void)arg;
	SettingsState* s = mem_alloc(sizeof(SettingsState));
	s->selected = 0;
	self->state = s;
}

static void cycle_language(void)
{
	Language next = (i18n_get_language() + 1) % LANG_COUNT;
	i18n_set_language(next);
}

static void update(Scene* self, float dt)
{
	(void)dt;
	SettingsState* s = self->state;

	if (btn_pressed(kButtonUp)) {
		s->selected = (s->selected - 1 + ITEM_COUNT) % ITEM_COUNT;
		audio_play(SFX_SELECT);
	}
	if (btn_pressed(kButtonDown)) {
		s->selected = (s->selected + 1) % ITEM_COUNT;
		audio_play(SFX_SELECT);
	}

	int change = btn_pressed(kButtonA) || btn_pressed(kButtonLeft) || btn_pressed(kButtonRight);

	switch (s->selected) {
		case ITEM_SOUND:
			if (change) { g_sound_on = !g_sound_on; audio_refresh(); audio_play(SFX_CONFIRM); save_store(); }
			break;
		case ITEM_LANGUAGE:
			if (change) { cycle_language(); audio_play(SFX_CONFIRM); save_store(); }
			break;
		case ITEM_CREDITS:
			if (btn_pressed(kButtonA)) { audio_play(SFX_CONFIRM); scene_push(SCENE_CREDITS, NULL); }
			break;
	}

	if (btn_pressed(kButtonB))
		scene_pop(NULL);   // B で戻る（"B: return" に一本化）
}

// 統一レイアウト:
//   ラベルは COLON_X で右寄せ（→ コロンの位置が全行で揃う）
//   値は COLON_X の右に左寄せ（→ 値の開始位置も揃う）
//   選択マーカーは各行のラベルのすぐ左
#define COLON_X  200
#define LABEL_R  (COLON_X - 4)        // ラベルの右端
#define VALUE_X  (COLON_X + 12)
#define ROW_Y0   95
#define ROW_DY   30

static void draw_row(int row, int selected, const char* label, const char* value)
{
	int y = ROW_Y0 + row * ROW_DY;

	if (value) {
		// 「ラベル : 値」行：コロンを COLON_X(画面中央)に揃える
		if (row == selected)
			draw_text_right(">", LABEL_R - text_width(label) - 6, y);
		draw_text_right(label, LABEL_R, y);
		draw_text(":", COLON_X, y);
		draw_text(value, VALUE_X, y);
	} else {
		// 値なし（Back などのアクション）：中央寄せ（コロン列と同じ中心に揃う）
		int lx = (SCREEN_WIDTH - text_width(label)) / 2;
		if (row == selected)
			draw_text_right(">", lx - 6, y);
		draw_text(label, lx, y);
	}
}

static void draw(Scene* self)
{
	SettingsState* s = self->state;

	pd->graphics->clear(kColorWhite);
	draw_text_centered(tr(STR_SETTINGS), 40);

	draw_row(ITEM_SOUND,    s->selected, tr(STR_SOUND),    g_sound_on ? tr(STR_ON) : tr(STR_OFF));
	draw_row(ITEM_LANGUAGE, s->selected, tr(STR_LANGUAGE), tr(STR_LANG_NAME));
	draw_row(ITEM_CREDITS,  s->selected, tr(STR_CREDITS),  NULL);

	draw_text_centered(tr(STR_RETURN_HINT), 210);
}

static void leave(Scene* self)
{
	mem_free(self->state);
}

Scene scene_settings(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave };
}
