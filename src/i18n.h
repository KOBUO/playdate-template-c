#pragma once
#include "pd_api.h"

// 対応言語。言語を増やすときはここに足す（例: LANG_FR）。
typedef enum {
	LANG_EN,
	LANG_JA,
	LANG_COUNT
} Language;

// 文字列ID。新しいUI文言を足すときはここに ID を1つ追加し、
// i18n.c の各言語テーブルに対応する訳を書く。
typedef enum {
	STR_PRESS_A,
	STR_SETTINGS,
	STR_CREDITS,
	STR_SOUND,
	STR_LANGUAGE,
	STR_LANG_NAME,      // その言語自身の表示名（English / 日本語）
	STR_ON,
	STR_OFF,
	STR_PAUSED,
	STR_RESUME,
	STR_QUIT_TO_STAGES, // ポーズから抜けてステージセレクトへ
	STR_RETURN_HINT,
	STR_GAME_HINT,
	STR_MENU_HINT,
	STR_STAGE_SELECT,
	STR_STAGE,
	STR_CREDITS_BODY,
	STR_DEMO_1,
	STR_DEMO_2,
	STR_DEMO_3,
	STR_DEMOS,
	STR_DEMO_CRANK,
	STR_DEMO_ACCEL,
	STR_COUNT
} StringID;

void        i18n_init(void);                  // フォント読込＋初期言語設定（main の init で）
void        i18n_set_language(Language lang); // 言語を切替（フォントも切り替わる）
Language    i18n_get_language(void);
const char* tr(StringID id);                  // 現在の言語の文字列を返す
