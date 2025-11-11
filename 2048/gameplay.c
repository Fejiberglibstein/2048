#include "include/gameplay.h"

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

        if (*onto_tile == *from_tile) {
            *onto_tile += 1;
            *from_tile = 0;
            *smush_flag |= 1 << ((i * 4) + onto);
        } else if (*onto_tile == 0) {
            *onto_tile = *from_tile;
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

        if (*onto_tile == *from_tile) {
            *onto_tile += 1;
            *from_tile = 0;
            *smush_flag |= 1 << ((onto * 4) + i);
        } else if (*onto_tile == 0) {
            *onto_tile = *from_tile;
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
}
