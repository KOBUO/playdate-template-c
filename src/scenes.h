#pragma once
#include "scene.h"

// 各シーンの生成関数。実体は scenes/ 以下の各ファイルにある。
// 新しい画面を足すときは、ここに1行 + scene 1ファイル + main で register。
Scene scene_title(void);
Scene scene_stage_select(void);
Scene scene_game(void);
Scene scene_pause(void);
Scene scene_settings(void);
Scene scene_credits(void);
Scene scene_dialogue(void);
Scene scene_demos(void);
Scene scene_demo_crank(void);
Scene scene_demo_accel(void);
Scene scene_demo_sprite(void);
Scene scene_splash(void);
