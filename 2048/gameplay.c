#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/render.h"
#include <stdlib.h>
#include <time.h>

void game_clear(GameState *gs) {
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            gs->board[i][j] = 0;
        }
    }
}

struct Coord status[16];

void game_new_random_tile(GameState *gs) {
    int y, x;
    uint32_t status_len = 0;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            if (gs->board[y][x] == 0) {
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
    gs->board[new_tile.y][new_tile.x] = ((rand() % 100 < 70) ? 1 : 2);
}

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
    gs->board[r1][c1] = 1; // First block guaranteed to be 2
    int proc = rand() % 100;
    uint8_t value_2;
    if (proc < 80) {
        value_2 = 1; // 80% chance of 2 for second block
    } else {
        value_2 = 2; // 20% chance of 4 for second block
    }
    gs->board[r2][c2] = value_2;
}

void shift_row(
    GameState *gs,
    uint8_t from,
    uint8_t onto,
    uint16_t *smush_flag
) {
    int i;
    for (i = 0; i < 4; i++) {
        uint8_t *onto_tile = &gs->board[onto][i];
        uint8_t *from_tile = &gs->board[from][i];

        if (*smush_flag & 1 << ((i * 4) + onto)) {
            continue;
        }

        if (*onto_tile == 0) {
            *onto_tile = *from_tile;
            *from_tile = 0;
            // *smush_flag |= 1 << ((i * 4) + from);
        } else if (*onto_tile == *from_tile) {
            *onto_tile += 1;
            *from_tile = 0;
            *smush_flag |= 1 << ((i * 4) + onto);
        }
    }
}

void shift_col(
    GameState *gs,
    uint8_t from,
    uint8_t onto,
    uint16_t *smush_flag
) {
    int i;
    for (i = 0; i < 4; i++) {
        uint8_t *onto_tile = &gs->board[i][onto];
        uint8_t *from_tile = &gs->board[i][from];

        if (*smush_flag & 1 << ((onto * 4) + i)) {
            continue;
        }

        if (*onto_tile == 0) {
            *onto_tile = *from_tile;
            *from_tile = 0;
            // *smush_flag |= 1 << ((onto * 4) + i);
        } else if (*onto_tile == *from_tile) {
            *onto_tile += 1;
            *from_tile = 0;
            *smush_flag |= 1 << ((onto * 4) + i);
        }
    }
}

void game_move_up(GameState *gs) {
    int i = 0;
    uint16_t smush_flag = 0;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 2:
            shift_row(gs, 3, 2, &smush_flag);
        case 1:
            shift_row(gs, 2, 1, &smush_flag);
        case 0:
            shift_row(gs, 1, 0, &smush_flag);
        }
    }
}

void game_move_down(GameState *gs) {
    int i = 0;
    uint16_t smush_flag = 0;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 2:
            shift_row(gs, 0, 1, &smush_flag);
        case 1:
            shift_row(gs, 1, 2, &smush_flag);
        case 0:
            shift_row(gs, 2, 3, &smush_flag);
        }
    }
}

void game_move_right(GameState *gs) {
    int i = 0;
    uint16_t smush_flag = 0;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 2:
            shift_col(gs, 3, 2, &smush_flag);
        case 1:
            shift_col(gs, 2, 1, &smush_flag);
        case 0:
            shift_col(gs, 1, 0, &smush_flag);
        }
    }
}

void game_move_left(GameState *gs) {
    int i = 0;
    uint16_t smush_flag = 0;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 2:
            shift_col(gs, 0, 1, &smush_flag);
        case 1:
            shift_col(gs, 1, 2, &smush_flag);
        case 0:
            shift_col(gs, 2, 3, &smush_flag);
        }
    }
}

void game_move_dir(GameState *gs, MoveDirection dir) {
    switch (dir) {
    case MOVE_UP:
        game_move_up(gs);
        break;
    case MOVE_DOWN:
        game_move_down(gs);
        break;
    case MOVE_LEFT:
        game_move_left(gs);
        break;
    case MOVE_RIGHT:
        game_move_right(gs);
        break;
    }

    game_new_random_tile(gs);

    render_board(gs);
}
