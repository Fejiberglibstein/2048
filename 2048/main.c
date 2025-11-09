#include "include/matrix.h"

int main(void) {
    int x, y, i;
    matrix_init();

    // for (y = 0; y < 64; y++) {
    //     for (i = 0; i < 64; i++) {
    //         matrix_draw_pixel(matrix_color(0, i / 4, 0), i, y);
    //     }
    // }

    for (y = 0; y < 64; y += 1) {
        for (x = 0; x < 64; x += 1, i++) {
            matrix_draw_pixel(
                matrix_color(0, x % 16, y % 16), x, y
            );
        }
    }
    matrix_swap_bufs();

    for (;;)
        ;
}
