#include "sysmenu.h"
#include "globals.h"
#include "scene.h"

static void cb_settings(void* ud) { (void)ud; scene_push(SCENE_SETTINGS, NULL); }
static void cb_credits(void* ud)  { (void)ud; scene_push(SCENE_CREDITS, NULL); }
static void cb_demos(void* ud)    { (void)ud; scene_push(SCENE_DEMOS, NULL); }

void sysmenu_add_settings(void)
{
	pd->system->addMenuItem("Settings", cb_settings, NULL);
}

void sysmenu_add_credits(void)
{
	pd->system->addMenuItem("Credits", cb_credits, NULL);
}

void sysmenu_add_demos(void)
{
	pd->system->addMenuItem("Demos", cb_demos, NULL);
}
