#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "2048/include/game-logic.h"

int main(){

    struct GameState game;
    game.game_board = {
                        {2, 0, 0, 4}, 
                        {2, 2, 0, 4}, 
                        {0, 2, 2, 4}, 
                        {0, 0, 2, 4}};
    uint8_t expected[4][4]  = {
                                {4, 4, 4, 8}, 
                                {0, 0, 0, 8}, 
                                {0, 0, 0, 0}, 
                                {0, 0, 0, 0}};
    struct GameState exp;
    exp.game_board = expected;

    game_move_up(&game);
    assert(check_board(game, exp));


}

bool check_board(struct GameState * exp, struct GameState * act){
    int i;
    for (i = 0; i < 4; i++;){
        if (!((uint32_t *)exp.game_board[i]) == ((uint32_t *)act.game_board[i])){
            return false;   
        }
    }
}
