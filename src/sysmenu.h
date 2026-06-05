#pragma once

// ⊙ システムメニュー用の再利用ヘルパー。各シーンの build_menu() から呼ぶ。
// （⊙メニューの項目名は OS が英字フォントで描くため英語固定。最大3項目まで）

void sysmenu_add_settings(void);        // "Settings" → SCENE_SETTINGS を push
void sysmenu_add_credits(void);         // "Credits"  → SCENE_CREDITS を push
void sysmenu_add_quit_to_title(void);   // "Quit to Title" → タイトルへ
