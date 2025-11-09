#include "include/game-logic.h"
#include "include/matrix.h"
#include "include/rendering.h"
#include "include/tiva.h"
#include "stdint.h"

extern GameState gs;

struct {
    struct AnimatedTile {
        uint8_t x;
        uint8_t y;
        int8_t vx;
        int8_t vy;
        uint8_t dest_x;
        uint8_t dest_y;
        uint8_t number;
    } animated_tiles[16];
    uint8_t len;
} AnimationState;

// 30fps clock interrupt handler
void timer_3_handler() {
    int x, y, i;
    *GPTM_ICR(timer_1) |= 0x1; // Clear TATOCINT interrupt
    if (!gs.animation_done) {
        // Render the static tiles
        for (x = 0; x < 4; x++) {
            for (y = 0; y < 4; y++) {
                if (gs.game_board[y][x] != 0) {
                    render_tile(x * 16, y * 16, gs.game_board[y][x]);
                } else {
                    render_clear_tile(x * 16, y * 16);
                }
            }
        }

        gs.animation_done = AnimationState.len == 0;

        for (i = 0; i < AnimationState.len; i++) {
            struct AnimatedTile *tile = &AnimationState.animated_tiles[i];

            render_tile(tile->x, tile->y, tile->number);

            tile->x += tile->vx;
            // If the x exceeds destination, remove from list
            if ((tile->x > tile->dest_x && tile->vx > 0)
                || (tile->x < tile->dest_x && tile->vx < 0)) {
                // Swap remove
                AnimationState.len -= 1;
                *tile = AnimationState.animated_tiles[AnimationState.len];
            }

            tile->y += tile->vy;
            // If the y exceeds destination, remove from list
            if ((tile->y > tile->dest_y && tile->vy > 0)
                || (tile->y < tile->dest_y && tile->vy < 0)) {
                // Swap remove
                AnimationState.len -= 1;
                *tile = AnimationState.animated_tiles[AnimationState.len];
            }
        }

        // Finish drawing to the screen
        matrix_swap_bufs();
    }
}

void game_timer_init() {
    AnimationState.len = 1;
    AnimationState.animated_tiles[0] = (struct AnimatedTile) {
        .number = 3,
        .dest_y = 0,
        .x = 0,
        .dest_x = 0,
        .y = 63 - 16,
        .vy = -6,
        .vx = 0,
    };

    uint32_t timer_ports = 1 << 1;
    *SYSCTL_RCGCTIMER |= timer_ports;
    while ((*SYSCTL_PRTIMER & timer_ports) != timer_ports)
        ;

    // Enable timer 3 (30 fps clock)
    *GPTM_CTL(timer_1) &= ~0x1;           // Clear the Timer A enable bit
    *GPTM_CFG(timer_1) = 0;               // Configure as 32 bit timer
    *GPTM_TAMR(timer_1) |= 0x2;           // Set timer to be periodic
    *GPTM_TAILR(timer_1) = 66666666 / 30; // 30Hz clock
    *GPTM_IMR(timer_1) |= 0x01;           // Configure timer to use interrupts
    *REG(0xE000E100) |= 1 << 21; // Timer 3 is 35th offset in interrupt vtable
    *GPTM_CTL(timer_1) |= 0x01;  // Enable the timer
}
