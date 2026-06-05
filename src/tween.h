#pragma once

// 補間（トゥイーン）/ イージング ユーティリティ。
// t は 0..1 を想定。アニメや演出に使う。

float tween_clamp01(float t);
float tween_lerp(float a, float b, float t);

float tween_ease_in(float t);       // 加速
float tween_ease_out(float t);      // 減速
float tween_ease_in_out(float t);   // 緩急

// 毎フレーム、現在値 cur を target へ滑らかに近づける（フレームレート非依存）。
// rate が大きいほど速い。例: x = tween_approach(x, targetX, 16.0f, dt);
float tween_approach(float cur, float target, float rate, float dt);
