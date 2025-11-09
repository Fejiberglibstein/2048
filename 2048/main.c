#include "include/matrix.h"
#include "include/rendering.h"

int main(void) {
    int x, y, i;
    matrix_init();
    render_init_colors();

    // for (y = 0; y < 64; y++) {
    //     for (i = 0; i < 64; i++) {
    //         matrix_draw_pixel(matrix_color(0, 0, i / 4), i, y);
    //     }
    // }

    for (x = 0, i = 1; x < 64; x += 16) {
        for (y = 0; y < 64; y += 16, i++) {
            render_tile(x, y, i);
        }
    }

    matrix_swap_bufs();

    for (;;)
        ;
}
