#include "stdint.h"

typedef struct {
    uint8_t board[4][4];

} GameState;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
} MoveDirection;

void game_move_dir(GameState *gs, MoveDirection dir);
