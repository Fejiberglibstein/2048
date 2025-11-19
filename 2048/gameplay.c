#include "include/gameplay.h"
#include "include/animation.h"
#include "include/matrix.h"
#include "include/render.h"
#include <stdbool.h>
#include <stdio.h>

struct Coord status[16];

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
    int i, j, start;
    for (i = 0; i < 4; i++) { // Iterate over all the columns
        // First four bits are for each tile in the col, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 0; start < 3; start++) { // Iterate over every row
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j >= 0; j--) { // Iterate for every row
                uint8_t *onto_tile = &gs->board[j][i];
                uint8_t *from_tile = &gs->board[j + 1][i];

                if (*from_tile == 0) {
                    continue;
                }

                if (*onto_tile == 0) {
                    *onto_tile = *from_tile;
                    *from_tile = 0;
                    tile_has_moved = true;
                    continue;
                }

                if (combined_flag & (1 << j)) {
                    // If we cannot move the tile up, but there has already been
                    // a combination, then we shouldn't do anything else in the
                    // loop
                    break;
                }

                if (*onto_tile == *from_tile) {
                    *onto_tile += 1;
                    *from_tile = 0;
                    // Set the onto tile's bitflag to be 1
                    tile_combined_at_position = 1 << j;
                    tile_has_moved = true;
                    j -= 1; // Fix animation
                    break;
                }
                // If the tile wasn't moved at all now, then it won't be moved
                // in the future.
                break;
            }
            j++; // Fix animation
            combined_flag |= tile_combined_at_position;
            if (tile_has_moved) {
                tile_has_moved = false;
                animation_new_moved_anim(&gs->as, i, start + 1, i, j);
            }
        }
    }
}

void game_move_down(GameState *gs) {
    int i, j, start;
    for (i = 0; i < 4; i++) { // Iterate over all the columns
        // First four bits are for each tile in the col, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 3; start > 0; start--) { // Iterate over every row
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j < 4; j++) { // Iterate for every row
                uint8_t *onto_tile = &gs->board[j][i];
                uint8_t *from_tile = &gs->board[j - 1][i];

                if (*from_tile == 0) {
                    continue;
                }

                if (*onto_tile == 0) {
                    *onto_tile = *from_tile;
                    *from_tile = 0;
                    tile_has_moved = true;
                    continue;
                }

                if (combined_flag & (1 << j)) {
                    // If we cannot move the tile up, but there has already been
                    // a combination, then we shouldn't do anything else in the
                    // loop
                    break;
                }

                if (*onto_tile == *from_tile) {
                    *onto_tile += 1;
                    *from_tile = 0;
                    // Set the onto tile's bitflag to be 1
                    tile_combined_at_position = 1 << j;
                    tile_has_moved = true;
                    j += 1; // Fix animation
                    break;
                }
                // If the tile wasn't moved at all now, then it won't be moved
                // in the future.
                break;
            }
            j -= 1; // Fix animation
            combined_flag |= tile_combined_at_position;
            if (tile_has_moved) {
                tile_has_moved = false;
                animation_new_moved_anim(&gs->as, i, start - 1, i, j);
            }
        }
    }
}


void game_move_left(GameState *gs) {
    int i, j, start;
    for (i = 0; i < 4; i++) { // Iterate over all the columns
        // First four bits are for each tile in the col, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 0; start < 3; start++) { // Iterate over every row
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j >= 0; j--) { // Iterate for every row
                uint8_t *onto_tile = &gs->board[i][j];
                uint8_t *from_tile = &gs->board[i][j + 1];

                if (*from_tile == 0) {
                    continue;
                }

                if (*onto_tile == 0) {
                    *onto_tile = *from_tile;
                    *from_tile = 0;
                    tile_has_moved = true;
                    continue;
                }

                if (combined_flag & (1 << j)) {
                    // If we cannot move the tile up, but there has already been
                    // a combination, then we shouldn't do anything else in the
                    // loop
                    break;
                }

                if (*onto_tile == *from_tile) {
                    *onto_tile += 1;
                    *from_tile = 0;
                    // Set the onto tile's bitflag to be 1
                    tile_combined_at_position = 1 << j;
                    tile_has_moved = true;
                    j -= 1; // Fix animation
                    break;
                }
                // If the tile wasn't moved at all now, then it won't be moved
                // in the future.
                break;
            }
            j++; // Fix animation
            combined_flag |= tile_combined_at_position;
            if (tile_has_moved) {
                tile_has_moved = false;
                animation_new_moved_anim(&gs->as, start + 1, i, j, i);
            }
        }
    }
}

void game_move_right(GameState *gs) {
    int i, j, start;
    for (i = 0; i < 4; i++) { // Iterate over all the columns
        // First four bits are for each tile in the col, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 3; start > 0; start--) { // Iterate over every row
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j < 4; j++) { // Iterate for every row
                uint8_t *onto_tile = &gs->board[i][j];
                uint8_t *from_tile = &gs->board[i][j - 1];

                if (*from_tile == 0) {
                    continue;
                }

                if (*onto_tile == 0) {
                    *onto_tile = *from_tile;
                    *from_tile = 0;
                    tile_has_moved = true;
                    continue;
                }

                if (combined_flag & (1 << j)) {
                    // If we cannot move the tile up, but there has already been
                    // a combination, then we shouldn't do anything else in the
                    // loop
                    break;
                }

                if (*onto_tile == *from_tile) {
                    *onto_tile += 1;
                    *from_tile = 0;
                    // Set the onto tile's bitflag to be 1
                    tile_combined_at_position = 1 << j;
                    tile_has_moved = true;
                    j += 1; // Fix animation
                    break;
                }
                // If the tile wasn't moved at all now, then it won't be moved
                // in the future.
                break;
            }
            j -= 1; // Fix animation
            combined_flag |= tile_combined_at_position;
            if (tile_has_moved) {
                tile_has_moved = false;
                animation_new_moved_anim(&gs->as, start - 1, i, j, i);
            }
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

    render_board(gs);
}
