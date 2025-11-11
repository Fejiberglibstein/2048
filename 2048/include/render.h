#include "stdint.h"
#include "gameplay.h"

#ifndef _RENDER_H
#define _RENDER_H

void render_tile(uint8_t x, uint8_t y, uint8_t num);
void render_board(GameState *gs);

#endif // _RENDER_H
