#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // 4x4 Matrix of powers of 2 corresponing to the value in that position as
    // 2^(i,j)
    uint8_t game_board[4][4];
    // Score
    uint32_t score;

    // I do not like how this is in the game state but its almost 6am and idgaf
    bool animation_done;
} GameState;

void game_init(GameState *gs);
void game_clear(GameState *gs);

void game_move_up(GameState *gs);
void game_move_down(GameState *gs);
void game_move_left(GameState *gs);
void game_move_right(GameState *gs);
