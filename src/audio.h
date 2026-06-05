#pragma once

// 効果音（SE）。g_sound_on が偽のときは鳴らさない。
// 音源は Source/sounds/*.wav（pdc が取り込む）。BGM を足すなら fileplayer を追加。

typedef enum {
	SFX_SELECT,    // カーソル移動
	SFX_CONFIRM,   // 決定
	SFX_COUNT
} SfxId;

void audio_init(void);        // main の init で1回
void audio_play(SfxId id);    // SE を鳴らす（消音時は無視）
