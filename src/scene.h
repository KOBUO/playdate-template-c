#pragma once

// 画面の種類。新しい画面を足すときはここに ID を追加する。
typedef enum {
	SCENE_SPLASH,
	SCENE_TITLE,
	SCENE_STAGE_SELECT,
	SCENE_GAME,
	SCENE_PAUSE,
	SCENE_SETTINGS,
	SCENE_CREDITS,
	SCENE_DIALOGUE,
	SCENE_DEMOS,
	SCENE_DEMO_CRANK,
	SCENE_DEMO_ACCEL,
	SCENE_DEMO_SPRITE,
	SCENE_DEMO_CRANKLOAD,
	SCENE_COUNT   // 番兵（シーン総数）。常に末尾に置く
} SceneID;

// 画面切替時の演出。
typedef enum {
	TRANSITION_NONE,  // 即座に切替
	TRANSITION_FADE,  // 黒フェードで切替
} Transition;

typedef struct Scene Scene;

// 1つの画面が実装する関数群。不要なものは NULL でよい。
// self->state にシーン固有のデータを持たせる（enter で確保 / leave で解放）。
struct Scene {
	void (*enter)(Scene* self, void* arg);      // 生成時に1回。arg = 呼び出し側が渡した引数
	void (*update)(Scene* self, float dt);      // 毎フレーム：ロジック（トップのシーンのみ）
	void (*draw)(Scene* self);                  // 毎フレーム：描画
	void (*resume)(Scene* self, void* result);  // 上に積まれた画面が pop し、再びトップになった時
	void (*leave)(Scene* self);                 // 破棄時に1回
	void (*build_menu)(Scene* self);            // ⊙システムメニューの項目を登録（トップになるたび呼ばれる）

	int draw_below;  // 1なら自分の下のシーンも描画する（オーバーレイ用）

	// --- 以下はマネージャが管理。シーン側で触らない ---
	void*   state;   // シーン固有データ
	SceneID id;
};

// ID -> シーン生成関数。
typedef Scene (*SceneFactory)(void);

void scene_register(SceneID id, SceneFactory factory);

// --- 画面操作（すべて次フレーム頭で安全に適用される）---

// スタックを空にして1枚に差し替える（通常の画面遷移）。
void scene_switch(SceneID id, void* arg, Transition transition);

// 今の画面の上に重ねる（ポーズ・会話など。遷移演出なし）。
void scene_push(SceneID id, void* arg);

// 一番上の画面を外して下に戻る。result は下の画面の resume に渡る。
void scene_pop(void* result);

void    scene_tick(float dt);  // マネージャを1フレーム進める（main から）
SceneID scene_current(void);   // 現在トップの画面 ID

// 小さな整数を arg / result として渡すためのヘルパー（ポインタ詰め）。
// 例: scene_switch(SCENE_GAME, INT_TO_ARG(level), TRANSITION_FADE);
//     int level = ARG_TO_INT(arg);
#include <stdint.h>
#define INT_TO_ARG(v) ((void*)(intptr_t)(v))
#define ARG_TO_INT(p) ((int)(intptr_t)(p))
