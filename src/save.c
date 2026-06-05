#include "save.h"
#include "globals.h"
#include "i18n.h"
#include <string.h>

// セーブファイル（Data フォルダに作られる）
#define SAVE_PATH "save.dat"
#define SAVE_MAGIC 0x50445459u   // 'PDTY'
#define SAVE_VERSION 1

// ディスク上のレイアウト。項目を増やすときは version を上げ、読み込み側で互換処理する。
typedef struct {
	uint32_t magic;
	uint32_t version;
	int32_t  sound_on;
	int32_t  language;
	uint32_t cleared_mask;   // ステージのクリアビット（最大32ステージ）
} SaveBlob;

void save_load(void)
{
	SDFile* f = pd->file->open(SAVE_PATH, kFileReadData);
	if (f == NULL)
		return;   // セーブ無し → 既定値のまま

	SaveBlob b;
	int n = pd->file->read(f, &b, sizeof(b));
	pd->file->close(f);

	if (n != (int)sizeof(b) || b.magic != SAVE_MAGIC || b.version != SAVE_VERSION)
		return;   // 壊れている/別バージョン → 無視

	g_sound_on = b.sound_on ? 1 : 0;
	g_cleared_stages = b.cleared_mask;
	if (b.language >= 0 && b.language < LANG_COUNT)
		i18n_set_language((Language)b.language);
}

void save_store(void)
{
	SaveBlob b = {
		.magic        = SAVE_MAGIC,
		.version      = SAVE_VERSION,
		.sound_on     = g_sound_on,
		.language     = (int32_t)i18n_get_language(),
		.cleared_mask = g_cleared_stages,
	};

	SDFile* f = pd->file->open(SAVE_PATH, kFileWrite);
	if (f == NULL) {
		pd->system->logToConsole("save_store: cannot open %s: %s", SAVE_PATH, pd->file->geterr());
		return;
	}
	pd->file->write(f, &b, sizeof(b));
	pd->file->close(f);
}

void save_mark_stage_cleared(int stage)
{
	if (stage < 0 || stage >= 32)
		return;
	g_cleared_stages |= (1u << stage);
}

int save_is_stage_cleared(int stage)
{
	if (stage < 0 || stage >= 32)
		return 0;
	return (g_cleared_stages & (1u << stage)) != 0;
}
