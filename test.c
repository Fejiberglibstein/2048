#include "2048/include/gameplay.h"
#include <stdio.h>

void print_board(GameState *gs);

int main() {
    GameState gs = {
        .board = {
            {1, 0, 2, 2},
            {1, 1, 1, 1},
            {2, 1, 1, 1},
            {2, 2, 0, 1},
        },

        // .board = {
        //     {0, 0, 0, 0},
        //     {0, 0, 0, 0},
        //     {0, 0, 0, 0},
        //     {0, 0, 0, 1},
        // },
    };

    GameState expected = {
        .board = {
            {5, 4, 3, 2},
            {0, 0, 0, 2},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
    };

    game_move_up(&gs);

    print_board(&gs);
}

void print_board(GameState *gs) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", gs->board[i][j]);
        }
        printf("\n");
    }
}
