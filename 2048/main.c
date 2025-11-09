#include "include/animations.h"
#include "include/matrix.h"
#include "include/rendering.h"
#include "include/game-logic.h"
#include "include/tiva.h"

void game_timer_init();

GameState gs;

int main(void) {
    matrix_init();
    render_init_colors();
    game_init(&gs);
    render_board(&gs);
    render_board(&gs);

    int i;

    for (i = 0; i < 6666666; i++)  {
        __asm__("\n\tnop");
    }
    game_move(&gs, MOVE_UP);

    // for (i = 0; i < 6666666; i++)  {
    //     __asm__("\n\tnop");
    // }
    // game_move(&gs, MOVE_LEFT);
    //
    // for (i = 0; i < 6666666; i++)  {
    //     __asm__("\n\tnop");
    // }
    // game_move(&gs, MOVE_DOWN);
    //
    // for (i = 0; i < 6666666; i++)  {
    //     __asm__("\n\tnop");
    // }
    // game_move(&gs, MOVE_RIGHT);
    //

    for (;;)
        ;
}
