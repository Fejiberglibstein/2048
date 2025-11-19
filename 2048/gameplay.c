#include "include/gameplay.h"
#include "include/animation.h"
#include "include/matrix.h"
#include "include/render.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
    for (i = 0; i < 4; i++) { // Iterate over all the rows
        // First four bits are for each tile in the row, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 0; start < 3; start++) { // Iterate over every col
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j >= 0; j--) { // Iterate for every col
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
    for (i = 0; i < 4; i++) { // Iterate over all the rows
        // First four bits are for each tile in the row, if it is a 1 then the
        // tile has already had a combination.
        uint8_t combined_flag = 0;
        bool tile_has_moved = false;

        for (start = 3; start > 0; start--) { // Iterate over every col
            // Non-zero if the current tile being moved has been combined with
            // another tile.
            //
            // Will have the value of `1 << n` where n is in the range (0, 3).
            uint8_t tile_combined_at_position = 0;
            for (j = start; j < 4; j++) { // Iterate for every col
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

struct Coord available_spawn_locations[16];
void game_spawn_new_tile(GameState *gs) {
    int x, y;
    int avail_len = 0;

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            if (gs->board[y][x] == 0) {
                available_spawn_locations[avail_len++] = (Coord) {
                    .x = x,
                    .y = y,
                };
            }
        }
    }

    Coord spawn_loc = available_spawn_locations[rand() % avail_len];
    // 75% chance to get a 1
    uint8_t tile_num = ((rand() % 0xFF) < 0xBF) ? 1 : 2;
    gs->board[spawn_loc.y][spawn_loc.x] = tile_num;

    animation_new_spawn_anim(&gs->as, x, y);
}

void game_move_dir(GameState *gs, MoveDirection dir) {
    uint8_t dup[4][4];
    memcpy(&dup, &gs->board, 16);

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

    // If the gameboard didn't change, then we can spawn a new tile
    if (memcmp(dup, gs->board, 16) != 0) {
        game_spawn_new_tile(gs);
    }

    render_board(gs);
}
