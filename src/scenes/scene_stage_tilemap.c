#include "scenes.h"
#include "globals.h"
#include "input.h"
#include "i18n.h"
#include "save.h"
#include "audio.h"
#include "sysmenu.h"
#include "dialogue.h"

// ステージ「タイルマップ（迷路）」：2次元配列のマップを描画し、十字キーで
// マス移動（壁には進めない）。出口 'E' に着いたらクリア。
// 壁は塗りつぶし矩形、プレイヤーは歩行スプライト(images/anim)を流用。

#define COLS 12
#define ROWS 7
#define TILE 32
#define OX   ((SCREEN_WIDTH  - COLS * TILE) / 2)   // 8
#define OY   ((SCREEN_HEIGHT - ROWS * TILE) / 2)   // 8

static const char* MAP[ROWS] = {
	"############",
	"#S.........#",
	"#..####....#",
	"#..####....#",
	"#.........E#",
	"#..........#",
	"############",
};

typedef struct {
	LCDBitmapTable* tbl;
	int cx, cy;       // プレイヤーのマス
	int won;
	float wt;
	int stage;
} St;

static const char*    s_intro[1];
static DialogueScript s_dlg;

static char tile_at(int c, int r)
{
	if (c < 0 || c >= COLS || r < 0 || r >= ROWS) return '#';
	return MAP[r][c];
}

static void enter(Scene* self, void* arg)
{
	St* s = mem_alloc(sizeof(St));
	const char* err;
	s->tbl = pd->graphics->loadBitmapTable("images/anim", &err);
	s->won = 0; s->wt = 0; s->stage = ARG_TO_INT(arg);
	// スタート 'S' を探す
	s->cx = 1; s->cy = 1;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (MAP[r][c] == 'S') { s->cx = c; s->cy = r; }
	self->state = s;
	s_intro[0] = tr(STR_HINT_TILEMAP);
	s_dlg.lines = s_intro; s_dlg.count = 1;
	s_dlg.name = tr(STR_NAME_GUIDE);
	scene_push(SCENE_DIALOGUE, &s_dlg);
}

static void try_move(St* s, int dc, int dr)
{
	int nc = s->cx + dc, nr = s->cy + dr;
	if (tile_at(nc, nr) != '#') { s->cx = nc; s->cy = nr; audio_play(SFX_SELECT); }
}

static void update(Scene* self, float dt)
{
	St* s = self->state;
	if (s->won) {
		s->wt += dt;
		if (s->wt >= 1.0f) scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
		return;
	}

	if (btn_pressed(kButtonLeft))  try_move(s, -1, 0);
	if (btn_pressed(kButtonRight)) try_move(s, +1, 0);
	if (btn_pressed(kButtonUp))    try_move(s, 0, -1);
	if (btn_pressed(kButtonDown))  try_move(s, 0, +1);

	if (tile_at(s->cx, s->cy) == 'E') {
		s->won = 1; s->wt = 0;
		save_mark_stage_cleared(s->stage); save_store();
		audio_play(SFX_CONFIRM);
	}

	if (btn_pressed(kButtonB))
		scene_switch(SCENE_STAGE_SELECT, NULL, TRANSITION_FADE);
}

static void draw(Scene* self)
{
	St* s = self->state;
	pd->graphics->clear(kColorWhite);

	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			int x = OX + c * TILE, y = OY + r * TILE;
			char t = MAP[r][c];
			if (t == '#')
				pd->graphics->fillRect(x, y, TILE, TILE, kColorBlack);   // 壁
			else if (t == 'E') {
				pd->graphics->drawRect(x + 3, y + 3, TILE - 6, TILE - 6, kColorBlack);
				draw_text("E", x + TILE / 2 - 4, y + TILE / 2 - 6);      // 出口
			}
		}
	}

	LCDBitmap* b = s->tbl ? pd->graphics->getTableBitmap(s->tbl, 0) : NULL;
	if (b) pd->graphics->drawBitmap(b, OX + s->cx * TILE, OY + s->cy * TILE, kBitmapUnflipped);

	if (s->won) draw_text_centered(tr(STR_CLEAR), 16);
}

static void leave(Scene* self)
{
	St* s = self->state;
	if (s->tbl) pd->graphics->freeBitmapTable(s->tbl);
	mem_free(s);
}

static void build_menu(Scene* self) { (void)self; sysmenu_add_settings(); sysmenu_add_quit_to_title(); }

Scene scene_stage_tilemap(void)
{
	return (Scene){ .enter = enter, .update = update, .draw = draw, .leave = leave, .build_menu = build_menu };
}
