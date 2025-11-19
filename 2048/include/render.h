#include "stdint.h"
#include "gameplay.h"

#ifndef _RENDER_H
#define _RENDER_H

void render_tile(uint8_t x, uint8_t y, uint8_t num);
void render_board(uint8_t board[4][4]);
void render_init_colors();

#endif // _RENDER_H
