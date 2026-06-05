#pragma once
#include "scene.h"

// 各シーンの生成関数。実体は scenes/ 以下の各ファイル。
Scene scene_splash(void);
Scene scene_title(void);
Scene scene_stage_select(void);
Scene scene_loading(void);
Scene scene_stage_move(void);
Scene scene_stage_crank(void);
Scene scene_stage_tilt(void);
Scene scene_settings(void);
Scene scene_credits(void);
Scene scene_dialogue(void);

// ローディングへ渡す依頼：演出後に next シーンへ param を渡して遷移する。
typedef struct {
	SceneID next;
	int     param;
} LoadRequest;
