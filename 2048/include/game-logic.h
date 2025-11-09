#include <stdbool.h>
#include <stdint.h>

struct Coord {
    uint8_t x;
    uint8_t y;
};

typedef struct {
    // 4x4 Matrix of powers of 2 corresponing to the
    // value in that position as 2^(i,j)
    uint8_t game_board[4][4];
    // Score
    uint32_t score;
    // the game board that is displayed
    uint32_t game_board_display[4][4];
    // Mask of the tiles that did not move between animations AHHHH im tired
    // 1 is it did move, 0 is no movement
    uint16_t static_mask;
    struct Coord new_tile_loc;

    // I do not like how this is in the game state but its almost 6am and idgaf
    bool animation_done;
} GameState;

void game_init(GameState *gs);
void game_clear(GameState *gs);

void game_move_up(GameState *gs);
void game_move_down(GameState *gs);
void game_move_left(GameState *gs);
void game_move_right(GameState *gs);

enum GameMoveDirection {
    MOVE_UP,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
};

void game_move(GameState *gs, enum GameMoveDirection direction);

void render_board(GameState *gs);
