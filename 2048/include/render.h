#ifndef _RENDER_H
#define _RENDER_H

#include "stdint.h"
#include "gameplay.h"
#include "matrix.h"

void render_tile(uint8_t x, uint8_t y, uint8_t num);
void render_board(uint8_t board[4][4]);
void render_init_colors();
MatrixColor render_get_color(uint8_t tile_num);

#endif // _RENDER_H
