#include "stdint.h"

#ifndef _MATRIX_H
#define _MATRIX_H

typedef uint32_t MatrixColor;

void matrix_init();
MatrixColor matrix_color(uint32_t r, uint32_t g, uint32_t b);
void matrix_draw_pixel(MatrixColor color, uint8_t x, uint8_t y);
void matrix_swap_bufs();

#endif // _MATRIX_H
