#include "2048/include/game-logic.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool check_board(GameState *exp, GameState *act);

int main() {

    GameState game = (GameState) {
        .game_board = {
            {2, 1, 0, 4},
            {2, 1, 0, 4},
            {0, 2, 0, 4},
            {0, 0, 2, 4},
        },
    };
    GameState expected = (GameState) {
        .game_board = {
            {3, 2, 2, 5},
            {0, 2, 0, 5},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
    };

    game_move_up(&game);
    assert(check_board(&game, &expected));
}

bool check_board(GameState *act, GameState *exp) {
    int i;
    int x, y;

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            printf("%d ", act->game_board[y][x]);
        }
        printf("\n");
    }


    for (i = 0; i < 4; i++) {
        if (*((uint32_t *)exp->game_board[i]) != *((uint32_t *)act->game_board[i])) {
            return false;
        }
    }

    return true;
}
