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
    game_timer_init();
    game_init(&gs);

    for (;;)
        ;
}
