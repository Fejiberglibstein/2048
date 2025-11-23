#include "include/accelerometer.h"
#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/render.h"
#include "include/tiva.h"
#include <stdbool.h>

void clock_init() {
    *SYSCTL_RCC2 |= 1 << 31; // Enable RCC2
    *SYSCTL_RCC2 |= 1 << 11; // Bypass PLL
    uint32_t rcc = *SYSCTL_RCC;
    rcc = (rcc & ~(0b11111 << 6)) | 0x15 << 6; // set crystal value to 16MHz
    *SYSCTL_RCC = rcc;
    *SYSCTL_RCC2 &= ~(0b111 << 4); // Set oscillator to be Main Oscillator
    *SYSCTL_RCC2 &= ~(1 << 13);    // Clear the pwrdwn bit
    *SYSCTL_RCC2 |= 1 << 30; // Set the div400 bit: divide by 400 instead of 200
    // Set SYSDIV to be 4: 400MHz/(4 + 1) = 80MHz
    *SYSCTL_RCC2 = (*SYSCTL_RCC2 & ~(0b1111111 << 22)) | 0x04 << 22;

    while (!(*SYSCTL_PLLSTAT)) // Wait until the status is 1
        ;

    *SYSCTL_RCC2 &= ~(1 << 11); // Set bypass to 0
}

GameState gs;

int main(void) {
    clock_init();
    matrix_init();
    accel_init();
    accel_interrupts_init();
    game_init(&gs);
    render_init_colors();
    animation_init();

    // int y, x, i;
    // for (y = 0; y < 64; y += 1) {
    //     for (x = 0; x < 64; x += 1, i++) {
    //         matrix_draw_pixel(matrix_color(x % 16, 0, y % 16), x, y);
    //     }
    // }

    matrix_swap_bufs();

    for (;;)
        ;
}
