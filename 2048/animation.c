#include "include/animation.h"
#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/render.h"
#include "include/tiva.h"

extern GameState gs; // defined in main.c

// Initialize the 30fps timer for animations
void animation_init() {
    // Enable timer 1 (30fps timer)
    *GPTM_CTL(timer_1) &= ~0x1;           // Clear the Timer A enable bit
    *GPTM_CFG(timer_1) = 0;               // Configure as 32 bit timer
    *GPTM_TAMR(timer_1) |= 0x2;           // Set timer to be periodic
    *GPTM_TAILR(timer_1) = 66666666 / 30; // Timer interrupts to 30Hz
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
                .start_x = old_col * 16,
                .start_y = old_row * 16,
                .dest_x = new_col * 16,
                .dest_y = new_row * 16,
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
                .x = col,
                .y = row,
                .number = num,
            },
        }
    );
}

void animation_timer(void) {
    *GPTM_ICR(timer_1) |= 0x1; // Clear TATOCINT interrupt

    // 24 is the computed number that determines how long the animations for a
    //    single movement will last
    if (gs.as.frame_number > 24) {
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
                dx = -1 * tile_offset;
                if (anim.move.start_x + dx <= anim.move.dest_x) {
                    render_tile(
                        anim.move.dest_x, anim.move.dest_y, anim.move.new_num
                    );
                    continue;
                }
                break;
            case MOVE_RIGHT:
                dx = 1 * tile_offset;
                if (anim.move.start_x + dx >= anim.move.dest_x) {
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
            // Spawn animations last 16 frames.
            if (gs.as.frame_number > 15) {
                continue;
            }

        }
    }

    gs.as.frame_number += 1;
}
