#include "include/gameplay.h"
#include "include/animation.h"
#include "include/matrix.h"
#include "include/render.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void game_spawn_new_tile(GameState *gs);

void game_reset(GameState *gs) {
    int x, y;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            gs->board[y][x] = 0;
        }
    }
}

void game_init(GameState *gs) {
    srand(time(NULL));

    gs->as = (AnimationState) {0};
    game_spawn_new_tile(gs);
    game_spawn_new_tile(gs);

    render_board(gs->board);
    matrix_swap_bufs();
}

void game_move_tiles(GameState *gs, Coord tiles[4]) {
    int start, i;

    // First four bits are for each tile in tiles, if the bit is a 1 then the
    // tile has already had a combination.
    uint8_t combined_flag = 0;
    for (start = 0; start < 3; start++) {
        bool tile_has_moved = false;

        const Coord tile_start = tiles[start + 1];
        const uint8_t original_num = gs->board[tile_start.y][tile_start.x];
        Coord tile_dest;
        // Non-zero if the current tile being moved has been combined with
        // another tile.
        //
        // Will have the value of `1 << n` where n is in the range (0, 3).
        uint8_t tile_combined_at_position = 0;

        for (i = start; i >= 0; i--) {
            const Coord onto_coord = tiles[i];
            const Coord from_coord = tiles[i + 1];
            uint8_t *onto_tile = &gs->board[onto_coord.y][onto_coord.x];
            uint8_t *from_tile = &gs->board[from_coord.y][from_coord.x];

            if (*from_tile == 0) {
                break;
            }

            if (*onto_tile == 0) {
                *onto_tile = *from_tile;
                *from_tile = 0;
                tile_has_moved = true;
                tile_dest = onto_coord;
                continue;
            }

            if (combined_flag & (1 << i)) {
                // If we cannot move the tile up, but there has already been
                // a combination, then we shouldn't do anything else in the
                // loop
                break;
            }

            if (*onto_tile == *from_tile) {
                *onto_tile += 1;
                *from_tile = 0;
                // Set the onto tile's bitflag to be 1
                tile_combined_at_position = 1 << i;
                tile_has_moved = true;
                tile_dest = onto_coord;
                break;
            }
            // If the tile wasn't moved at all now, then it won't be moved
            // in the future.
            break;
        }
        combined_flag |= tile_combined_at_position;
        if (tile_has_moved) {
            // Remove the moved tile from the static list
            gs->as.static_tiles[tile_start.y][tile_start.x] = 0;
            tile_has_moved = false;
            animation_new_moved_anim(
                &gs->as,
                tile_start.y,
                tile_start.x,
                tile_dest.y,
                tile_dest.x,
                original_num,
                gs->board[tile_dest.y][tile_dest.x]
            );
        }
    }
}

void game_move_up(GameState *gs) {
    int i;
    for (i = 0; i < 4; i++) {
        game_move_tiles(gs, (Coord[]) {{i, 0}, {i, 1}, {i, 2}, {i, 3}});
    }
}

void game_move_down(GameState *gs) {
    int i;
    for (i = 0; i < 4; i++) {
        game_move_tiles(gs, (Coord[]) {{i, 3}, {i, 2}, {i, 1}, {i, 0}});
    }
}

void game_move_right(GameState *gs) {
    int i;
    for (i = 0; i < 4; i++) {
        game_move_tiles(gs, (Coord[]) {{0, i}, {1, i}, {2, i}, {3, i}});
    }
}

void game_move_left(GameState *gs) {
    int i;
    for (i = 0; i < 4; i++) {
        game_move_tiles(gs, (Coord[]) {{3, i}, {2, i}, {1, i}, {0, i}});
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

    animation_new_spawn_anim(&gs->as, spawn_loc.x, spawn_loc.y, tile_num);
}

void game_move_dir(GameState *gs, MoveDirection dir) {
    memcpy(&gs->as.static_tiles, &gs->board, 16);
    gs->as.animations_length = 0;

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
    if (memcmp(gs->as.static_tiles, gs->board, 16) != 0) {
        game_spawn_new_tile(gs);
    }

    gs->as.current_direction = dir;
    gs->as.frame_number = 0;
}
