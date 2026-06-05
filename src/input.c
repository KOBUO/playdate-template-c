#include "input.h"
#include "globals.h"

static PDButtons s_current  = 0; // 現在押されている
static PDButtons s_pushed   = 0; // このフレームで押された
static PDButtons s_released = 0; // このフレームで離された

void input_update(void)
{
	pd->system->getButtonState(&s_current, &s_pushed, &s_released);
}

int btn_down(PDButtons b)     { return (s_current  & b) != 0; }
int btn_pressed(PDButtons b)  { return (s_pushed   & b) != 0; }
int btn_released(PDButtons b) { return (s_released & b) != 0; }

float crank_angle(void)  { return pd->system->getCrankAngle();  }
float crank_change(void) { return pd->system->getCrankChange(); }
int   crank_docked(void) { return pd->system->isCrankDocked();  }
