#pragma once

// 画面の種類。新しい画面を足すときはここに ID を追加する。
typedef enum {
	SCENE_SPLASH,
	SCENE_TITLE,
	SCENE_STAGE_SELECT,
	SCENE_LOADING,
	SCENE_STAGE_MOVE,    // 操作別ミニゲーム（十字キー）
	SCENE_STAGE_CRANK,   // クランク
	SCENE_STAGE_TILT,    // 加速度センサ（傾き）
	SCENE_SETTINGS,
	SCENE_CREDITS,
	SCENE_DIALOGUE,
	SCENE_COUNT
} SceneID;

// 画面切替時の演出。
typedef enum {
	TRANSITION_NONE,
	TRANSITION_FADE,
} Transition;

typedef struct Scene Scene;

// 1つの画面が実装する関数群。不要なものは NULL でよい。
struct Scene {
	void (*enter)(Scene* self, void* arg);
	void (*update)(Scene* self, float dt);
	void (*draw)(Scene* self);
	void (*resume)(Scene* self, void* result);  // 上に積んだ画面が pop した時
	void (*leave)(Scene* self);
	void (*build_menu)(Scene* self);             // ⊙システムメニューの項目登録

	int draw_below;  // 1なら自分の下のシーンも描画する（オーバーレイ用）

	// --- 以下はマネージャが管理。シーン側で触らない ---
	void*   state;
	SceneID id;
};

typedef Scene (*SceneFactory)(void);

void scene_register(SceneID id, SceneFactory factory);

void scene_switch(SceneID id, void* arg, Transition transition); // 差し替え
void scene_push(SceneID id, void* arg);                          // 重ねる
void scene_pop(void* result);                                    // 戻る

void    scene_tick(float dt);
SceneID scene_current(void);

#include <stdint.h>
#define INT_TO_ARG(v) ((void*)(intptr_t)(v))
#define ARG_TO_INT(p) ((int)(intptr_t)(p))
