#include "include/animation.h"
#include "include/tiva.h"
#include <stdio.h>

// Initialize the 30fps timer for animations
void animation_init() {
}

void animation_add_anim(AnimationState *as, Animation anim) {
    as->animations[as->animations_length++] = anim;
    assert(as->animations_length < 16);
}

void animation_remove_anim(AnimationState *as, uint32_t index) {
    if (as->animations_length == 0)
        return;

    // Swap removal algorithm
    as->animations_length -= 1;
    as->animations[index] = as->animations[as->animations_length];
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
