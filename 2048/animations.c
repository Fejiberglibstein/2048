#include "include/game-logic.h"
#include "include/matrix.h"
#include "include/rendering.h"
#include "include/tiva.h"
#include "stdint.h"

#define sign(x) ((x > 0) ? 1 : -1)

extern GameState gs;

struct {
    struct AnimatedTile {
        uint8_t x;
        uint8_t y;
        uint8_t vx;
        uint8_t vy;
        uint8_t dest_x;
        uint8_t dest_y;
        uint8_t number;
    } animated_tiles[16];
    uint8_t len;
} AnimationState;

// 30fps clock interrupt handler
void timer_3_handler() {
    int x, y, i;
    *GPTM_ICR(timer_3) |= 0x1; // Clear TATOCINT interrupt

    // Render the static tiles
    for (x = 0; x < 4; x++) {
        for (y = 0; y < 4; y++) {
            render_tile(x * 16, y * 16, gs.game_board[y][x]);
        }
    }

    gs.animation_done = AnimationState.len == 0;

    for (int i = 0; i < AnimationState.len; i++) {
        struct AnimatedTile *tile = &AnimationState.animated_tiles[i];

        tile->x += tile->vx;
        tile->vx *= 2;
        // If the x exceeds destination, remove from list
        if (sign(tile->vx) * tile->x >= tile->dest_x) {
            // Swap remove
            AnimationState.len -= 1;
            *tile = AnimationState.animated_tiles[AnimationState.len];
        }

        tile->y += tile->vy;
        tile->vy *= 2;
        // If the y exceeds destination, remove from list
        if (sign(tile->vy) * tile->y >= tile->dest_y) {
            // Swap remove
            AnimationState.len -= 1;
            *tile = AnimationState.animated_tiles[AnimationState.len];
        }

        render_tile(tile->x, tile->y, tile->number);
    }

    // Finish drawing to the screen
    matrix_swap_bufs();
}

void game_timer_init() {
    // Enable timer 3 (30 fps clock)
    *GPTM_CTL(timer_3) &= ~0x1;           // Clear the Timer A enable bit
    *GPTM_CFG(timer_3) = 0;               // Configure as 32 bit timer
    *GPTM_TAMR(timer_3) |= 0x2;           // Set timer to be periodic
    *GPTM_TAILR(timer_3) = 66666666 / 30; // 30Hz clock
    *GPTM_IMR(timer_3) |= 0x01;           // Configure timer to use interrupts
    *REG(0xE000E100) |= 1 << 23; // Timer 2 is 23rd offset in interrupt vtable
    *GPTM_CTL(timer_3) |= 0x01;  // Enable the timer
}
