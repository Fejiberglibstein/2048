#include "2048/include/game-logic.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool check_board(GameState *exp, GameState *act);
void print_game_board(GameState *gs);

int main() {
    // Test for move_up() and Score update
    GameState game = (GameState) {
        .game_board = {
            {2, 1, 0, 4},
            {2, 1, 0, 4},
            {0, 2, 0, 4},
            {0, 0, 2, 4},
        },
        .score = 0,
    };
    GameState expected = (GameState) {
        .game_board = {
            {3, 2, 2, 5},
            {0, 2, 0, 5},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        .score = 76,
    };

    game_move_up(&game);
    assert(check_board(&game, &expected));
    print_game_board(&game);
    printf("Score: %0d\n", game.score);

    // Test for game_board init
    game_init(&game);           // Should also clear previous case
    print_game_board(&game); 
    printf("Score: %0d\n", game.score);

}

// Non lazy test function that prints the backend game_board
void print_game_board(GameState *gs){
    int x, y;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            printf("%d ", gs->game_board[y][x]);
        }
        printf("\n");
    }
}

// Checks equivalence between two game_boards
bool check_board(GameState *act, GameState *exp) {
    int i;
    for (i = 0; i < 4; i++) {
        if (*((uint32_t *)exp->game_board[i]) != *((uint32_t *)act->game_board[i])) {
            return false;
        }
    }

    return true;
}
