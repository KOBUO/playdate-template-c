#include "globals.h"
#include <string.h>

PlaydateAPI* pd = NULL;
LCDFont* g_font = NULL;
int g_sound_on = 1;
uint32_t g_cleared_stages = 0;

int text_width(const char* text)
{
	return pd->graphics->getTextWidth(g_font, text, strlen(text), kUTF8Encoding, 0);
}

void draw_text(const char* text, int x, int y)
{
	pd->graphics->setFont(g_font);
	pd->graphics->drawText(text, strlen(text), kUTF8Encoding, x, y);
}

void draw_text_right(const char* text, int right_x, int y)
{
	draw_text(text, right_x - text_width(text), y);
}

void draw_text_centered(const char* text, int y)
{
	draw_text(text, (SCREEN_WIDTH - text_width(text)) / 2, y);
}

LCDBitmap* img_load(const char* path)
{
	const char* err;
	LCDBitmap* b = pd->graphics->loadBitmap(path, &err);
	if (b == NULL)
		pd->system->logToConsole("img_load failed: %s (%s)", path, err);
	return b;
}

void img_draw(LCDBitmap* b, int x, int y)
{
	if (b)
		pd->graphics->drawBitmap(b, x, y, kBitmapUnflipped);
}

void img_free(LCDBitmap* b)
{
	if (b)
		pd->graphics->freeBitmap(b);
}

void* mem_alloc(size_t size)
{
	return pd->system->realloc(NULL, size);
}

void mem_free(void* ptr)
{
	if (ptr)
		pd->system->realloc(ptr, 0);
}
