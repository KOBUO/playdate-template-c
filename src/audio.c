#include "audio.h"
#include "globals.h"

// 拡張子なしで指定（pdc が Source/sounds/*.wav を取り込む）
static const char* SFX_PATH[SFX_COUNT] = {
	[SFX_SELECT]  = "sounds/select",
	[SFX_CONFIRM] = "sounds/confirm",
};

static SamplePlayer* s_players[SFX_COUNT];
static FilePlayer*   s_bgm;

#define BGM_VOL 0.5f

void audio_refresh(void)
{
	if (s_bgm) {
		float v = g_sound_on ? BGM_VOL : 0.0f;
		pd->sound->fileplayer->setVolume(s_bgm, v, v);
	}
}

void audio_init(void)
{
	for (int i = 0; i < SFX_COUNT; i++) {
		s_players[i] = pd->sound->sampleplayer->newPlayer();
		AudioSample* smp = pd->sound->sample->load(SFX_PATH[i]);
		if (smp)
			pd->sound->sampleplayer->setSample(s_players[i], smp);
		else
			pd->system->logToConsole("audio: missing %s", SFX_PATH[i]);
	}

	// BGM（ループ再生）。fileplayer はファイルからストリーム再生する。
	s_bgm = pd->sound->fileplayer->newPlayer();
	pd->sound->fileplayer->loadIntoPlayer(s_bgm, "music/bgm");  // 無ければ play は無音
	audio_refresh();
	pd->sound->fileplayer->play(s_bgm, 0);   // 0 = 無限ループ
}

void audio_play(SfxId id)
{
	if (!g_sound_on || id < 0 || id >= SFX_COUNT || s_players[id] == NULL)
		return;
	pd->sound->sampleplayer->play(s_players[id], 1, 1.0f);   // 1回再生
}
