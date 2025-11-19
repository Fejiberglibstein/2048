#include <stddef.h>
#include <stdint.h>

#ifndef _ANIMATION_H
#define _ANIMATION_H

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
} MoveDirection;

typedef struct {
    union {
        struct {
            uint8_t x;
            uint8_t y;
        } spawn;

        struct {
            uint8_t start_x;
            uint8_t start_y;
            uint8_t dest_x;
            uint8_t dest_y;
        } move;
    };

    enum {
        ANIMATION_SPAWN,
        ANIMATION_MOVE,
    } kind;
} Animation;

typedef struct {
    /// The list of animations that are being played
    Animation animations[16];
    uint32_t animations_length;

    /// The direction the tiles are moving in.
    MoveDirection current_direction;
} AnimationState;


void animation_init();
void animation_new_moved_anim(
        AnimationState *as,
    uint8_t old_col,
    uint8_t old_row,
    uint8_t new_col,
    uint8_t new_row
);
void animation_new_spawn_anim(AnimationState *as, uint8_t col, uint8_t row);

#endif // _ANIMATION_H
