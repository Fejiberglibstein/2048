#include "include/animation.h"
#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/render.h"
#include "include/tiva.h"

extern GameState gs; // defined in main.c

// Initialize the 30fps timer for animations
void animation_init() {
    uint8_t timer_port = 1 << 1; // Enable timer 1
    *SYSCTL_RCGCTIMER |= timer_port;
    while ((*SYSCTL_PRTIMER & timer_port) != timer_port)
        ;

    // Enable timer 1 (30fps timer)
    *GPTM_CTL(timer_1) &= ~0x1;           // Clear the Timer A enable bit
    *GPTM_CFG(timer_1) = 0;               // Configure as 32 bit timer
    *GPTM_TAMR(timer_1) |= 0x2;           // Set timer to be periodic
    *GPTM_TAILR(timer_1) = CLOCK_SPEED / 60; // Timer interrupts to 30Hz
    *GPTM_IMR(timer_1) |= 0x01;           // Configure timer to use interrupts
    interrupt_enable(21, 1);    // Enable timer 1 with the 2nd highest priority
    *GPTM_CTL(timer_1) |= 0x01; // Enable the timer
}

void animation_add_anim(AnimationState *as, Animation anim) {
    as->animations[as->animations_length++] = anim;
    assert(as->animations_length < 16);
}

void animation_remove_anim(AnimationState *as, uint32_t index) {
    if (as->animations_length == 0)
        return;

    as->animations_length -= 1;
    const int len = as->animations_length;

    int i;
    for (i = index; i < len; i++) {
        as->animations[i] = as->animations[i + 1];
    }
}

void animation_new_moved_anim(
    AnimationState *as,
    uint8_t old_col,
    uint8_t old_row,
    uint8_t new_col,
    uint8_t new_row,
    uint8_t old_num,
    uint8_t new_num
) {

    animation_add_anim(
        as,
        (Animation) {
            .kind = ANIMATION_MOVE,
            .move = {
                .start_x = old_row * 16,
                .start_y = old_col * 16,
                .dest_x = new_row * 16,
                .dest_y = new_col * 16,
                .old_num = old_num,
                .new_num = new_num,
            },
        }
    );
}

void animation_new_spawn_anim(
    AnimationState *as,
    uint8_t col,
    uint8_t row,
    uint8_t num
) {
    animation_add_anim(
        as,
        (Animation) {
            .kind = ANIMATION_SPAWN,
            .spawn = {
                .x = col * 16,
                .y = row * 16,
                .number = num,
            },
        }
    );
}

#define ANIMATION_DURATION 25

void animation_frame(void) {
    if (gs.as.frame_number > ANIMATION_DURATION)
        return;

    // Wait for the previous frame to be drawn
    while (!matrix_bufs_have_swapped())
        ;

    if (gs.as.frame_number == ANIMATION_DURATION) {
        render_board(gs.board); // Render the actual state of the board
        matrix_swap_bufs();
        // Make it go past 1 so it doesn't rerender the board over and over
        gs.as.frame_number += 1;
        return;
    }

    // Render all of the static tiles first
    render_board(gs.as.static_tiles);

    int i;
    Animation anim;
    for (i = 0; i < gs.as.animations_length; i++) {
        anim = gs.as.animations[i];
        if (anim.kind == ANIMATION_MOVE) {
            int tile_offset = gs.as.frame_number * 4;

            int dx = 0;
            int dy = 0;
            switch (gs.as.current_direction) {
            case MOVE_UP:
                dy = -1 * tile_offset;
                if (anim.move.start_y + dy <= anim.move.dest_y) {
                    render_tile(
                        anim.move.dest_x, anim.move.dest_y, anim.move.new_num
                    );
                    continue;
                }
                break;
            case MOVE_DOWN:
                dy = 1 * tile_offset;
                if (anim.move.start_y + dy >= anim.move.dest_y) {
                    render_tile(
                        anim.move.dest_x, anim.move.dest_y, anim.move.new_num
                    );
                    continue;
                }
                break;
            case MOVE_LEFT:
                dx = 1 * tile_offset;
                if (anim.move.start_x + dx >= anim.move.dest_x) {
                    render_tile(
                        anim.move.dest_x, anim.move.dest_y, anim.move.new_num
                    );
                    continue;
                }
                break;
            case MOVE_RIGHT:
                dx = -1 * tile_offset;
                if (anim.move.start_x + dx <= anim.move.dest_x) {
                    render_tile(
                        anim.move.dest_x, anim.move.dest_y, anim.move.new_num
                    );
                    continue;
                }
                break;
            }

            render_tile(
                anim.move.start_x + dx,
                anim.move.start_y + dy,
                anim.move.old_num
            );
        } else if (anim.kind == ANIMATION_SPAWN) {
            int frame_number = gs.as.frame_number - 8;
            if (frame_number < 0) {
                continue;
            }
            // Spawn animations last 16 frames.
            if (frame_number < 8) {
                int width = frame_number * 2;
                int height = frame_number * 2;
                int x, y;
                MatrixColor colors[2] = {
                    render_get_color(anim.spawn.number),
                    matrix_color(0, 0, 0),
                };
                for (y = 0; y < height; y += 1) {
                    for (x = 0; x < width; x += 1) {
                        MatrixColor c
                            = x % (width - 1) == 0 && y % (height - 1) == 0
                                  ? colors[1]
                                  : colors[0];
                        matrix_draw_pixel(
                            c,
                            anim.spawn.x + x + ((16 - width) / 2),
                            anim.spawn.y + y + ((16 - height) / 2)
                        );
                    }
                }
                continue;
            } else {
                render_tile(anim.spawn.x, anim.spawn.y, anim.spawn.number);
                continue;
            }
        }
    }

    matrix_swap_bufs(); // Done with drawing
    gs.as.frame_number += 1;
}

void animation_timer(void) {
    animation_frame();
    *GPTM_ICR(timer_1) |= 0x1; // Clear TATOCINT interrupt
}
