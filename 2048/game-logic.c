#include <stdlib.h>
#include <stdint.h>



typedef struct {
    uint8_t game_board[4][4]; // 4x4 Matrix of powers of 2 corresponing to the value in that position as 2^(i,j) 
} GameState;

// Initialize game board. This places two blocks on the board, one a 2-block and the other a 4-block or a 2-block 
void game_init(GameState * gs){}

// Cascade blocks up, combining equal powers 
void game_move_up(GameState * gs){


    // 16 bit "Matrix" of bit flags signaling whether the corresponding cell has been combined
    uint16_t smush_flag;    // Each flag bit (row*4)+col corresponds to 
    int j, level;

    for (level = 1; level < 3; level++){                                // Increments level such that the rows are traversed
        switch (level) {
        case 3:
            for (j = 0; j < 3; j++) {                   
                uint8_t *up_cell_value = &gs->game_board[2][j];
                uint8_t *cell_value = &gs->game_board[3][j];
                if (!(smush_flag & (1 << ((3*4)+j)))){          // Mask out corresponding smush bit and continue only if 0
                    if (*up_cell_value == 0){                   // If above cell is zero
                        *up_cell_value = *cell_value;           // Copy cell to above cell   
                        *cell_value = 0;                        // Zero out cell
                    }else if (*up_cell_value == *cell_value){   // If cells match
                        (*up_cell_value)++;                     // Increment above cell
                        *cell_value = 0;                        // Zero cell
                    } 
                    smush_flag = smush_flag | (1 << ((2*4)+j)); // Mark above cell as modified
                }
            }
        case 2:
            for (j = 0; j < 3; j++) {
                uint8_t *up_cell_value = &gs->game_board[1][j];
                uint8_t *cell_value = &gs->game_board[2][j];
                if (!(smush_flag & (1 << ((2*4)+j)))){          // Mask out corresponding smush bit and continue only if 0
                    if (*up_cell_value == 0){                   // If above cell is zero
                        *up_cell_value = *cell_value;           // Copy cell to above cell   
                        *cell_value = 0;                        // Zero out cell
                    }else if (*up_cell_value == *cell_value){   // If cells match
                        (*up_cell_value)++;                     // Increment above cell
                        *cell_value = 0;                        // Zero cell
                    } 
                    smush_flag = smush_flag | (1 << ((1*4)+j)); // Mark above cell as modified
                }
            }
        case 1:
            for (j = 0; j < 3; j++) {
                uint8_t *up_cell_value = &gs->game_board[0][j];
                uint8_t *cell_value = &gs->game_board[1][j];
                if (!(smush_flag & (1 << ((1*4)+j)))){          // Mask out corresponding smush bit and continue only if 0
                    if (*up_cell_value == 0){                   // If above cell is zero
                        *up_cell_value = *cell_value;           // Copy cell to above cell   
                        *cell_value = 0;                        // Zero out cell
                    }else if (*up_cell_value == *cell_value){   // If cells match
                        (*up_cell_value)++;                     // Increment above cell
                        *cell_value = 0;                        // Zero cell
                    } 
                    smush_flag = smush_flag | (1 << ((0*4)+j)); // Mark above cell as modified
                }
            }
        }
    }
}

// Cascade blocks down, combining equal powers 
void game_move_down(GameState *){



}

// Cascade blocks left, combining equal powers 
void game_move_left(GameState *){



}

// Cascade blocks right, combining equal powers 
void game_move_right(GameState *){



}
