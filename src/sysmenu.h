#pragma once

// ⊙ システムメニュー用の「よく使う項目」を登録する再利用ヘルパー。
// 各シーンの build_menu() の中から、必要なものを呼んで組み立てる。
// （⊙メニューの項目名は OS が英字フォントで描くため英語固定）
//
// 例:
//   static void build_menu(Scene* self) {
//       sysmenu_add_settings();          // 共通部品
//       sysmenu_add_credits();           // 共通部品
//       pd->system->addMenuItem("...");  // このシーン独自の項目
//   }

void sysmenu_add_settings(void);  // "Settings" → SCENE_SETTINGS を push
void sysmenu_add_credits(void);   // "Credits"  → SCENE_CREDITS を push
void sysmenu_add_demos(void);     // "Demos"    → SCENE_DEMOS を push
