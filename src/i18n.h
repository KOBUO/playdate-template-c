#pragma once
#include "pd_api.h"

// 対応言語。増やすときはここに足す。
typedef enum {
	LANG_EN,
	LANG_JA,
	LANG_COUNT
} Language;

// 文字列ID。追加時は i18n.c の各言語テーブルにも訳を書く。
typedef enum {
	STR_PRESS_A,
	STR_SETTINGS,
	STR_CREDITS,
	STR_SOUND,
	STR_LANGUAGE,
	STR_LANG_NAME,      // その言語自身の表示名
	STR_ON,
	STR_OFF,
	STR_RETURN_HINT,
	STR_MENU_HINT,
	STR_STAGE_SELECT,
	STR_STAGE_MOVE,     // ステージ名（操作別ミニゲーム）
	STR_STAGE_CRANK,
	STR_STAGE_TILT,
	STR_STAGE_TILEMAP,
	STR_HINT_MOVE,      // 各ステージの操作説明（導入会話＆画面）
	STR_HINT_CRANK,
	STR_HINT_TILT,
	STR_HINT_TILEMAP,
	STR_LOADING,
	STR_CLEAR,
	STR_NAME_GUIDE,     // 会話の話者名（デモ）
	STR_GO,             // 選択肢
	STR_CANCEL,
	STR_CREDITS_BODY,
	STR_COUNT
} StringID;

void        i18n_init(void);
void        i18n_set_language(Language lang);
Language    i18n_get_language(void);
const char* tr(StringID id);
