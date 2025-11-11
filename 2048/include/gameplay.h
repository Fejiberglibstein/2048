#include "stdint.h"

#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

typedef struct {
    uint8_t board[4][4];
} GameState;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
} MoveDirection;

typedef struct Coord {
    uint8_t x;
    uint8_t y;
} Coord;


void game_clear(GameState *gs);
void game_init(GameState *gs);
void game_move_dir(GameState *gs, MoveDirection dir);

void game_move_up(GameState *gs);
void game_move_down(GameState *gs);
void game_move_right(GameState *gs);
void game_move_left(GameState *gs);

#endif // _GAMEPLAY_H
