#include "scene.h"
#include "globals.h"
#include <stdint.h>

#define MAX_STACK   8      // 同時に積めるシーン数の上限
#define TRANS_HALF  0.18f  // フェードの片道（秒）。往復で約0.36秒

// --- 登録テーブルとシーンスタック ---
static SceneFactory s_factory[SCENE_COUNT];
static Scene        s_stack[MAX_STACK];
static int          s_depth = 0;   // 積まれている数。トップ = s_depth - 1

// --- 予約された操作（次フレーム頭で適用）---
typedef enum { OP_NONE, OP_SWITCH, OP_PUSH, OP_POP } Op;
static Op         s_op = OP_NONE;
static SceneID    s_op_id;
static void*      s_op_arg;
static Transition s_op_trans;

// --- 遷移演出の状態 ---
typedef enum { T_IDLE, T_OUT, T_IN } TransPhase;
static TransPhase s_phase = T_IDLE;
static float      s_phase_t = 0.0f;

void scene_register(SceneID id, SceneFactory factory)
{
	s_factory[id] = factory;
}

void scene_switch(SceneID id, void* arg, Transition transition)
{
	s_op = OP_SWITCH;
	s_op_id = id;
	s_op_arg = arg;
	s_op_trans = transition;
}

void scene_push(SceneID id, void* arg)
{
	s_op = OP_PUSH;
	s_op_id = id;
	s_op_arg = arg;
}

void scene_pop(void* result)
{
	s_op = OP_POP;
	s_op_arg = result;
}

SceneID scene_current(void)
{
	return s_depth > 0 ? s_stack[s_depth - 1].id : SCENE_COUNT;
}

// --- スタック操作 ---

// トップのシーンに合わせて ⊙ システムメニューの項目を貼り直す。
// スタックが変わるたび（push/pop/switch）に呼ぶ。
static void refresh_menu(void)
{
	pd->system->removeAllMenuItems();
	if (s_depth > 0) {
		Scene* top = &s_stack[s_depth - 1];
		if (top->build_menu)
			top->build_menu(top);
	}
}

static void instantiate(SceneID id, void* arg)
{
	if (s_depth >= MAX_STACK) {
		pd->system->error("scene stack overflow");
		return;
	}
	Scene sc = s_factory[id]();   // 生成関数から vtable + フラグを得る
	sc.id = id;
	sc.state = NULL;
	s_stack[s_depth] = sc;
	Scene* self = &s_stack[s_depth];
	s_depth++;
	if (self->enter)
		self->enter(self, arg);
	refresh_menu();
}

static void destroy_top(void)
{
	Scene* self = &s_stack[s_depth - 1];
	if (self->leave)
		self->leave(self);
	s_depth--;
}

static void clear_all(void)
{
	while (s_depth > 0)
		destroy_top();
}

// --- 描画 ---

// Bayer 8x8 ディザによる黒フェード。f = 黒の割合 (0..1)。
static const int BAYER8[8][8] = {
	{  0, 32,  8, 40,  2, 34, 10, 42 },
	{ 48, 16, 56, 24, 50, 18, 58, 26 },
	{ 12, 44,  4, 36, 14, 46,  6, 38 },
	{ 60, 28, 52, 20, 62, 30, 54, 22 },
	{  3, 35, 11, 43,  1, 33,  9, 41 },
	{ 51, 19, 59, 27, 49, 17, 57, 25 },
	{ 15, 47,  7, 39, 13, 45,  5, 37 },
	{ 63, 31, 55, 23, 61, 29, 53, 21 },
};

static void draw_fade(float f)
{
	if (f <= 0.001f)
		return;
	if (f >= 0.999f) {
		pd->graphics->fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, kColorBlack);
		return;
	}

	int threshold = (int)(f * 64.0f);
	LCDPattern pat;
	for (int y = 0; y < 8; y++) {
		uint8_t row = 0;
		for (int x = 0; x < 8; x++) {
			// 閾値より小さいピクセルを黒にする。ビット1=白、0=黒。
			if (BAYER8[y][x] >= threshold)
				row |= (1 << (7 - x));
		}
		pat[y] = row;       // パターン行
		pat[8 + y] = 0xFF;  // マスク（全ピクセル不透明）
	}
	pd->graphics->fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (LCDColor)pat);
}

static void draw_stack(void)
{
	if (s_depth == 0)
		return;

	// draw_below が連続する間、描く最下段を下げる
	int base = s_depth - 1;
	while (base > 0 && s_stack[base].draw_below)
		base--;

	for (int i = base; i < s_depth; i++) {
		Scene* s = &s_stack[i];
		if (s->draw)
			s->draw(s);
	}
}

// --- 遷移の進行 ---

static void tick_transition(float dt)
{
	s_phase_t += dt;

	if (s_phase == T_OUT) {
		if (s_phase_t >= TRANS_HALF) {
			// 暗転しきったタイミングで実際に差し替える
			clear_all();
			instantiate(s_op_id, s_op_arg);
			s_op = OP_NONE;
			s_phase = T_IN;
			s_phase_t = 0.0f;
			draw_stack();
			draw_fade(1.0f);
			return;
		}
		draw_stack();
		draw_fade(s_phase_t / TRANS_HALF);          // 0 -> 1（暗くなる）
	} else { // T_IN
		if (s_phase_t >= TRANS_HALF) {
			s_phase = T_IDLE;
			draw_stack();
			return;
		}
		draw_stack();
		draw_fade(1.0f - s_phase_t / TRANS_HALF);    // 1 -> 0（明るくなる）
	}
}

void scene_tick(float dt)
{
	// 遷移中は入力・update を止め、演出だけ進める
	if (s_phase != T_IDLE) {
		tick_transition(dt);
		return;
	}

	// 予約された操作を適用
	switch (s_op) {
		case OP_SWITCH:
			if (s_op_trans == TRANSITION_NONE) {
				clear_all();
				instantiate(s_op_id, s_op_arg);
				s_op = OP_NONE;
			} else {
				// フェード開始（差し替えは中間点で行う）
				s_phase = T_OUT;
				s_phase_t = 0.0f;
				tick_transition(dt);
				return;
			}
			break;

		case OP_PUSH:
			instantiate(s_op_id, s_op_arg);
			s_op = OP_NONE;
			break;

		case OP_POP: {
			void* result = s_op_arg;
			s_op = OP_NONE;
			if (s_depth > 0)
				destroy_top();
			refresh_menu();
			if (s_depth > 0) {
				Scene* top = &s_stack[s_depth - 1];
				if (top->resume)
					top->resume(top, result);
			}
			break;
		}

		case OP_NONE:
			break;
	}

	// トップのシーンだけ update（下のシーンは止まる）
	if (s_depth > 0) {
		Scene* top = &s_stack[s_depth - 1];
		if (top->update)
			top->update(top, dt);
	}

	draw_stack();
}
