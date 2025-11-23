#include "2048/include/gameplay.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

GameState gs;

void print_board(GameState *gs);
bool board_is_eq(GameState g1, GameState g2);

int main() {
    GameState gs = {
        .board = {
            {0, 0, 2, 1},
            {2, 1, 1, 1},
            {1, 1, 1, 2},
            {1, 2, 0, 0},
        },
    };
    game_move_up(&gs);
    assert(board_is_eq(
        gs,
        (GameState) {
            .board = {
                {2, 2, 2, 2},
                {2, 2, 2, 2},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            },
        }
    ));

    gs = (GameState) {
        .board = {
            {0, 2, 1, 1},
            {0, 0, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
        },
    };
    game_move_left(&gs);
    assert(board_is_eq(
        gs,
        (GameState) {
            .board = {
                {2, 2, 2, 2},
                {2, 2, 2, 2},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            },
        }
    ));

    gs = (GameState) {
        .board = {
            {1, 1, 2, 2},
            {0, 1, 1, 0},
            {1, 0, 0, 1},
            {2, 2, 1, 1},
        },
    };
    game_move_up(&gs);
    assert(board_is_eq(
        gs,
        (GameState) {
            .board = {
                {2, 2, 2, 2},
                {2, 2, 2, 2},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            },
        }
    ));
}

void print_board(GameState *gs) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", gs->board[i][j]);
        }
        printf("\n");
    }
}

bool board_is_eq(GameState g1, GameState g2) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (g1.board[i][j] != g2.board[i][j]) {
                print_board(&g1);
                return false;
            }
        }
    }
    return true;
}
