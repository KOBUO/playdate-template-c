#pragma once

// セーブ/ロード。設定(Sound/言語)とステージのクリア状況を、
// ゲームの Data フォルダ内のファイルに保存する（再起動しても残る）。

void save_load(void);    // 起動時に呼ぶ。無ければ既定値のまま
void save_store(void);   // 現在の設定/進行を書き出す

void save_mark_stage_cleared(int stage);  // ステージをクリア済みにする
int  save_is_stage_cleared(int stage);    // クリア済みか
