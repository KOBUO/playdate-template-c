#include "i18n.h"
#include "globals.h"

// 言語ごとのフォント。
// 英語・日本語で見た目(サイズ/太さ)を揃えるため、両言語とも同じピクセルフォント
// (PixelMplus12-Bold から生成、英数字＋かな＋第1水準漢字) を使う。
// 実体は Source/fonts/main.fnt → pdc が main.pft にコンパイル。
// 言語ごとに別フォントにしたい場合はここを分ければよい。
static const char* FONT_PATH[LANG_COUNT] = {
	[LANG_EN] = "fonts/main",
	[LANG_JA] = "fonts/main",
};

// 翻訳テーブル。[言語][文字列ID]。
// 訳が NULL（未翻訳）の場合は英語にフォールバックする（tr 内）。
static const char* TABLE[LANG_COUNT][STR_COUNT] = {
	[LANG_EN] = {
		[STR_PRESS_A]        = "Press A",
		[STR_SETTINGS]       = "Settings",
		[STR_CREDITS]        = "Credits",
		[STR_SOUND]          = "Sound",
		[STR_LANGUAGE]       = "Language",
		[STR_LANG_NAME]      = "English",
		[STR_ON]             = "ON",
		[STR_OFF]            = "OFF",
		[STR_PAUSED]         = "- PAUSED -",
		[STR_RESUME]         = "A: resume",
		[STR_QUIT_TO_STAGES] = "B: quit to stages",
		[STR_RETURN_HINT]    = "B: return",
		[STR_GAME_HINT]      = "A: pause   B: stages   (Menu btn: options)",
		[STR_MENU_HINT]      = "A: Select    B: Back",
		[STR_STAGE_SELECT]   = "SELECT STAGE",
		[STR_STAGE]          = "Stage",
		[STR_CREDITS_BODY]   = "Made with the Playdate SDK",
		[STR_DEMO_1]         = "This is the dialogue window.",
		[STR_DEMO_2]         = "Press A to advance, B to close.",
		[STR_DEMO_3]         = "Use it for story, clues and choices.",
		[STR_DEMOS]          = "DEMOS",
		[STR_DEMO_CRANK]     = "Crank",
		[STR_DEMO_ACCEL]     = "Accelerometer",
		[STR_DEMO_SPRITE]    = "Sprite & Animation",
	},
	[LANG_JA] = {
		[STR_PRESS_A]        = "Aボタンを押してください",
		[STR_SETTINGS]       = "設定",
		[STR_CREDITS]        = "クレジット",
		[STR_SOUND]          = "サウンド",
		[STR_LANGUAGE]       = "言語",
		[STR_LANG_NAME]      = "日本語",
		[STR_ON]             = "オン",
		[STR_OFF]            = "オフ",
		[STR_PAUSED]         = "- ポーズ -",
		[STR_RESUME]         = "A: 再開",
		[STR_QUIT_TO_STAGES] = "B: ステージへ",
		[STR_RETURN_HINT]    = "B: 戻る",
		[STR_GAME_HINT]      = "A: ポーズ  B: ステージ  (メニューボタン: オプション)",
		[STR_MENU_HINT]      = "A: 決定    B: 戻る",
		[STR_STAGE_SELECT]   = "ステージせんたく",
		[STR_STAGE]          = "ステージ",
		[STR_CREDITS_BODY]   = "Playdate SDK で制作",
		[STR_DEMO_1]         = "これは会話ウィンドウです。",
		[STR_DEMO_2]         = "Aで送り、Bで閉じます。",
		[STR_DEMO_3]         = "物語・手がかり・選択肢に使えます。",
		[STR_DEMOS]          = "デモ",
		[STR_DEMO_CRANK]     = "クランク",
		[STR_DEMO_ACCEL]     = "加速度センサ",
		[STR_DEMO_SPRITE]    = "スプライト/アニメ",
	},
};

static Language s_lang = LANG_EN;
static LCDFont* s_fonts[LANG_COUNT];

void i18n_init(void)
{
	const char* err;
	for (int i = 0; i < LANG_COUNT; i++) {
		s_fonts[i] = pd->graphics->loadFont(FONT_PATH[i], &err);
		if (s_fonts[i] == NULL)
			pd->system->logToConsole("i18n: font '%s' not found (%s)", FONT_PATH[i], err);
	}
	// 英語フォントは必須。無ければ致命的。
	if (s_fonts[LANG_EN] == NULL)
		pd->system->error("i18n: English font missing: %s", FONT_PATH[LANG_EN]);

	// 不足言語は英語フォントで代替（豆腐になるが落ちない）
	for (int i = 0; i < LANG_COUNT; i++)
		if (s_fonts[i] == NULL)
			s_fonts[i] = s_fonts[LANG_EN];

	i18n_set_language(LANG_EN);
}

void i18n_set_language(Language lang)
{
	s_lang = lang;
	g_font = s_fonts[lang];   // 描画ヘルパーが使う共通フォントを切替
}

Language i18n_get_language(void)
{
	return s_lang;
}

const char* tr(StringID id)
{
	const char* s = TABLE[s_lang][id];
	return s ? s : TABLE[LANG_EN][id];   // 未翻訳は英語へフォールバック
}
