#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t game_board[4][4]; // 4x4 Matrix of powers of 2 corresponing to the
                              // value in that position as 2^(i,j)
} GameState;

// Initialize game board. This places two blocks on the board, one a 2-block and
// the other a 4-block or a 2-block
void game_init(GameState *gs) {
}

#define move_vertical(from, onto)                                              \
    for (i = 0; i < 4; i++) {                                                  \
        uint8_t *onto_tile = &gs->game_board[onto][i];                         \
        uint8_t *from_tile = &gs->game_board[from][i];                         \
        if (!(smush_flag & (1 << ((onto * 4) + i)))) {                         \
            if (*onto_tile == 0) {                                             \
                /*If there is empty space on the space being moved onto, fill  \
                 * that spot with the tile that it came from and clear the     \
                 * from tile */                                                \
                *onto_tile = *from_tile;                                       \
                *from_tile = 0;                                                \
            } else if (*onto_tile == *from_tile) {                             \
                /*If the tiles are the same, add one to the onto and clear     \
                 * from */                                                     \
                *onto_tile += 1;                                               \
                *from_tile = 0;                                                \
                smush_flag |= 1 << ((onto * 4) + i);                           \
            }                                                                  \
        }                                                                      \
    }

#define move_horizontal(from, onto)                                            \
    for (i = 0; i < 4; i++) {                                                  \
        uint8_t *onto_tile = &gs->game_board[i][onto];                         \
        uint8_t *from_tile = &gs->game_board[i][from];                         \
        if (!(smush_flag & (1 << ((i * 4) + onto)))) {                         \
            if (*onto_tile == 0) {                                             \
                /*If there is empty space on the space being moved onto, fill  \
                 * that spot with the tile that it came from and clear the     \
                 * from tile */                                                \
                *onto_tile = *from_tile;                                       \
                *from_tile = 0;                                                \
            } else if (*onto_tile == *from_tile) {                             \
                /*If the tiles are the same, add one to the onto and clear     \
                 * from */                                                     \
                *onto_tile += 1;                                               \
                *from_tile = 0;                                                \
                smush_flag |= 1 << ((i * 4) + onto);                           \
            }                                                                  \
        }                                                                      \
    }

// Cascade blocks up, combining equal powers
void game_move_up(GameState *gs) {
    uint16_t smush_flag;
    int i, level;
    for (level = 0; level < 3; level++) {
        switch (level) {
        case 2:
            move_vertical(3, 2);
        case 1:
            move_vertical(2, 1);
        case 0:
            move_vertical(1, 0);
        }
    }
}

// Cascade blocks down, combining equal powers
void game_move_down(GameState *gs) {
    uint16_t smush_flag;
    int i, level;
    for (level = 0; level < 3; level++) {
        switch (level) {
        case 2:
            move_vertical(0, 1);
        case 1:
            move_vertical(1, 2);
        case 0:
            move_vertical(2, 3);
        }
    }
}

// Cascade blocks left, combining equal powers
void game_move_left(GameState *gs) {
    uint16_t smush_flag;
    int i, level;
    for (level = 0; level < 3; level++) {
        switch (level) {
        case 2:
            move_horizontal(0, 1);
        case 1:
            move_horizontal(1, 2);
        case 0:
            move_horizontal(2, 3);
        }
    }
}

// Cascade blocks right, combining equal powers
void game_move_right(GameState *gs) {
    uint16_t smush_flag;
    int i, level;
    for (level = 0; level < 3; level++) {
        switch (level) {
        case 2:
            move_horizontal(3, 2);
        case 1:
            move_horizontal(2, 1);
        case 0:
            move_horizontal(1, 0);
        }
    }
}
