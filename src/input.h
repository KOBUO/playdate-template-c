#pragma once
#include "pd_api.h"

// 毎フレーム先頭で1回呼ぶ（ボタン状態を更新）
void input_update(void);

int btn_down(PDButtons b);      // 今押されているか（押しっぱなし判定）
int btn_pressed(PDButtons b);   // このフレームで押された瞬間か
int btn_released(PDButtons b);  // このフレームで離された瞬間か

float crank_angle(void);        // クランクの角度（度）
float crank_change(void);       // 前フレームからのクランク変化量（度）
int   crank_docked(void);       // クランクが収納されているか
