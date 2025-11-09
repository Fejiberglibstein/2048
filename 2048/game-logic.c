#include <stdlib.h>
#include <stdint.h>

struct GameState {
    uint16_t[4][4] game_board;          // 4x4 Matrix of powers of 2 corresponing to the value in that position as 2^(i,j) 
}

// Initialize game board. This places two blocks on the board, one a 2-block and the other a 4-block or a 2-block 
void game_init(GameState *){}

// Cascade blocks up, combining equal powers 
void game_move_up(GameState *){}

// Cascade blocks down, combining equal powers 
void game_move_down(GameState *){}

// Cascade blocks left, combining equal powers 
void game_move_left(GameState *){}

// Cascade blocks right, combining equal powers 
void game_move_right(GameState *){}
