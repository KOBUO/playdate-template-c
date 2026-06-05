#pragma once
#include "pd_api.h"

// どこからでも使える PlaydateAPI（main の init で代入）
extern PlaydateAPI* pd;

// 全シーン共通で使うデフォルトフォント（main の init で読込）
extern LCDFont* g_font;

// ゲーム全体で共有する設定/進行（save.c で永続化）
extern int      g_sound_on;
extern uint32_t g_cleared_stages;   // クリア済みステージのビットマスク

// 画面サイズ（Playdate は 400x240 固定）
#define SCREEN_WIDTH  LCD_COLUMNS  // 400
#define SCREEN_HEIGHT LCD_ROWS     // 240

// --- テキスト描画ヘルパー（すべて g_font / UTF-8）---
int  text_width(const char* text);                       // 描画幅(px)
void draw_text(const char* text, int x, int y);          // 左寄せ
void draw_text_right(const char* text, int right_x, int y); // 右端を right_x に合わせる
void draw_text_centered(const char* text, int y);        // 横中央

// --- 画像ヘルパー ---
LCDBitmap* img_load(const char* path);          // 失敗時 NULL（ログ出力）。path は拡張子なし(例 "images/title")
void       img_draw(LCDBitmap* b, int x, int y);
void       img_free(LCDBitmap* b);

// Playdate 流のメモリ確保/解放（実機では malloc ではなくこれを使う）。
// シーンの state はこれで確保し、leave で解放する。
void* mem_alloc(size_t size);
void  mem_free(void* ptr);
