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
		[STR_PRESS_A]      = "Press A",
		[STR_SETTINGS]     = "Settings",
		[STR_CREDITS]      = "Credits",
		[STR_SOUND]        = "Sound",
		[STR_LANGUAGE]     = "Language",
		[STR_LANG_NAME]    = "English",
		[STR_ON]           = "ON",
		[STR_OFF]          = "OFF",
		[STR_RETURN_HINT]  = "B: return",
		[STR_MENU_HINT]    = "A: Select    B: Back",
		[STR_STAGE_SELECT] = "SELECT STAGE",
		[STR_STAGE_MOVE]    = "Move (D-pad)",
		[STR_STAGE_CRANK]   = "Crank",
		[STR_STAGE_TILT]    = "Tilt",
		[STR_STAGE_TILEMAP] = "Maze (Tilemap)",
		[STR_HINT_MOVE]     = "Use the D-pad to reach the goal.",
		[STR_HINT_CRANK]    = "Crank the pointer to the target, then A.",
		[STR_HINT_TILT]     = "Tilt the device to roll into the goal.",
		[STR_HINT_TILEMAP]  = "D-pad through the maze to the exit.",
		[STR_LOADING]       = "Loading",
		[STR_CLEAR]         = "CLEAR!",
		[STR_NAME_GUIDE]    = "Guide",
		[STR_GO]            = "Let's go",
		[STR_CANCEL]        = "Cancel",
		[STR_CREDITS_BODY]  = "Made with the Playdate SDK",
	},
	[LANG_JA] = {
		[STR_PRESS_A]      = "Aボタンを押してください",
		[STR_SETTINGS]     = "設定",
		[STR_CREDITS]      = "クレジット",
		[STR_SOUND]        = "サウンド",
		[STR_LANGUAGE]     = "言語",
		[STR_LANG_NAME]    = "日本語",
		[STR_ON]           = "オン",
		[STR_OFF]          = "オフ",
		[STR_RETURN_HINT]  = "B: 戻る",
		[STR_MENU_HINT]    = "A: 決定    B: 戻る",
		[STR_STAGE_SELECT] = "ステージせんたく",
		[STR_STAGE_MOVE]    = "いどう (十字キー)",
		[STR_STAGE_CRANK]   = "クランク",
		[STR_STAGE_TILT]    = "かたむき",
		[STR_STAGE_TILEMAP] = "めいろ (タイル)",
		[STR_HINT_MOVE]     = "十字キーでゴールへ進もう。",
		[STR_HINT_CRANK]    = "クランクで的に合わせ、Aを押そう。",
		[STR_HINT_TILT]     = "本体を傾けてゴールへ転がそう。",
		[STR_HINT_TILEMAP]  = "十字キーで迷路を出口まで進もう。",
		[STR_LOADING]       = "よみこみ中",
		[STR_CLEAR]         = "クリア！",
		[STR_NAME_GUIDE]    = "ガイド",
		[STR_GO]            = "はじめる",
		[STR_CANCEL]        = "やめる",
		[STR_CREDITS_BODY]  = "Playdate SDK で制作",
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
