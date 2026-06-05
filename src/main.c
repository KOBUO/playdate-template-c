#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "audio.h"
#include "save.h"
#include "scene.h"
#include "scenes.h"

static int update(void* userdata);

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	(void)arg;

	if (event == kEventInit) {
		pd = playdate;

		// 多言語システムを初期化（言語別フォントの読込もここで）
		i18n_init();
		audio_init();
		save_load();      // 保存済みの設定/進行を読み込む（言語もここで反映）

		pd->display->setRefreshRate(30);

		// 全シーンを登録（新しい画面を足したらここにも1行）
		scene_register(SCENE_TITLE, scene_title);
		scene_register(SCENE_STAGE_SELECT, scene_stage_select);
		scene_register(SCENE_GAME,  scene_game);
		scene_register(SCENE_PAUSE, scene_pause);
		scene_register(SCENE_SETTINGS, scene_settings);
		scene_register(SCENE_CREDITS, scene_credits);
		scene_register(SCENE_DIALOGUE, scene_dialogue);
		scene_register(SCENE_DEMOS, scene_demos);
		scene_register(SCENE_DEMO_CRANK, scene_demo_crank);
		scene_register(SCENE_DEMO_ACCEL, scene_demo_accel);

		// 最初に表示する画面（初回は演出なし）
		scene_switch(SCENE_TITLE, NULL, TRANSITION_NONE);

		pd->system->setUpdateCallback(update, pd);
	}

	// 終了・スリープ時に取りこぼし防止で保存（設定変更時も都度保存している）
	if (event == kEventTerminate || event == kEventLock)
		save_store();

	return 0;
}

// 毎フレーム呼ばれるメインループ
static int update(void* userdata)
{
	(void)userdata;

	// 経過時間（秒）を計算してシーンに渡す
	float dt = pd->system->getElapsedTime();
	pd->system->resetElapsedTime();

	input_update();
	pd->graphics->clear(kColorWhite);

	scene_tick(dt);

	pd->system->drawFPS(0, 0);
	return 1;
}
