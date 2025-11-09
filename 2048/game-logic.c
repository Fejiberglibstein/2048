#include "include/game-logic.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void game_clear(GameState *gs);

// Initialize game board. This places two blocks on the board, one a 2-block and
// the other a 4-block or a 2-block
void game_init(GameState *gs) {
    srand(time(NULL));

    // Ensure game state is clear
    game_clear(gs);

    // Generate 4 random coordinates
    int r1 = rand() % 4;
    int c1 = rand() % 4;

    int r2, c2;
    do { //  While coordinates match, try generating new ones
        // Generate 4 random coordinates
        r2 = rand() % 4;
        c2 = rand() % 4;
    } while ((r1 == r2) && (c1 == c2));

    // Assign values to the blocks at these coordinates
    gs->game_board[r1][c1] = 1; // First block guaranteed to be 2
    int proc = rand() % 100;
    uint8_t value_2;
    if (proc < 80) {
        value_2 = 1; // 80% chance of 2 for second block
    } else {
        value_2 = 2; // 20% chance of 4 for second block
    }
    gs->game_board[r2][c2] = value_2;
}

// Clears all game_board cells to zero and resets score
void game_clear(GameState *gs) {
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            gs->game_board[i][j] = 0;
        }
    }
    gs->score = 0;
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
                gs->static_mask |= 1 << ((from * 4) + i);                      \
            } else if (*onto_tile == *from_tile) {                             \
                /*If the tiles are the same, add one to the onto and clear     \
                 * from */                                                     \
                *onto_tile += 1;                                               \
                gs->score += 1 << *onto_tile;                                  \
                *from_tile = 0;                                                \
                smush_flag |= 1 << ((onto * 4) + i);                           \
                gs->static_mask |= 1 << ((from * 4) + i);                      \
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
                gs->static_mask |= 1 << ((i * 4) + from);                      \
            } else if (*onto_tile == *from_tile) {                             \
                /*If the tiles are the same, add one to the onto and clear     \
                 * from */                                                     \
                *onto_tile += 1;                                               \
                gs->score += 1 << *onto_tile;                                  \
                *from_tile = 0;                                                \
                smush_flag |= 1 << ((i * 4) + onto);                           \
                gs->static_mask |= 1 << ((i * 4) + from);                      \
            }                                                                  \
        }                                                                      \
    }

// Cascade blocks up, combining equal powers
void game_move_up(GameState *gs) {
    uint16_t smush_flag = 0;
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
    uint16_t smush_flag = 0;
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
    uint16_t smush_flag = 0;
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
    uint16_t smush_flag = 0;
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

struct Coord status[16];

void game_new_random_tile(GameState *gs) {
    int y, x;
    uint32_t status_len = 0;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            if (gs->game_board[y][x] == 0) {
                // set i and j in the status
                status[status_len++] = (struct Coord) {
                    .y = y,
                    .x = x,
                };
            }
        }
    }

    uint8_t index = rand() % status_len;

    struct Coord new_tile = status[index];
    gs->new_tile_loc = new_tile;
    gs->game_board[new_tile.y][new_tile.x] = ((rand() % 100 < 70) ? 1 : 2);
}

void game_move(GameState *gs, enum GameMoveDirection direction) {
    switch (direction) {
    case MOVE_UP:
        game_move_up(gs);
        break;
    case MOVE_LEFT:
        game_move_left(gs);
        break;
    case MOVE_RIGHT:
        game_move_right(gs);
        break;
    case MOVE_DOWN:
        game_move_down(gs);
        break;
    }

    game_new_random_tile(gs);
}
