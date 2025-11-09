#include <stdint.h>

typedef struct {
    uint8_t game_board[4][4];           // 4x4 Matrix of powers of 2 corresponing to the value in that position as 2^(i,j) 
} GameState; 

void game_init(GameState * gs);

void game_move_up(GameState * gs);
void game_move_down(GameState * gs);
void game_move_left(GameState * gs);
void game_move_right(GameState * gs);

