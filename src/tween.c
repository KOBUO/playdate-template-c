#include "tween.h"
#include <math.h>

float tween_clamp01(float t) { return t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t); }

float tween_lerp(float a, float b, float t) { return a + (b - a) * t; }

float tween_ease_in(float t)  { t = tween_clamp01(t); return t * t; }

float tween_ease_out(float t) { t = tween_clamp01(t); return 1.0f - (1.0f - t) * (1.0f - t); }

float tween_ease_in_out(float t)
{
	t = tween_clamp01(t);
	if (t < 0.5f) return 2.0f * t * t;
	float u = -2.0f * t + 2.0f;
	return 1.0f - u * u / 2.0f;
}

float tween_approach(float cur, float target, float rate, float dt)
{
	float k = 1.0f - expf(-rate * dt);   // フレームレート非依存の指数スムージング
	return cur + (target - cur) * k;
}
