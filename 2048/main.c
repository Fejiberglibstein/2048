#include "include/accelerometer.h"
#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/render.h"
#include "include/tiva.h"
#include <stdbool.h>

char printf_arena[256];

void clock_init() {
    // Clear usesys and set byprass
    *SYSCTL_RCC = (*SYSCTL_RCC & ~(1 << 22)) | (1 << 11);

    uint32_t rcc = *SYSCTL_RCC;
    rcc = (rcc & ~(0b11 << 4)) | 0b01 << 4; // select oscillator source (OSCSRC)
    rcc = (rcc & ~(0b11111 << 6)) | 0x15 << 6; // set crystal value to 16MHz
    rcc = (rcc & ~(0b1 << 13));                // Clear pwrdwn bit
    rcc = (rcc & ~(0b1111 << 23)) | 0x2 << 23; // Set sysdiv to be 2
    rcc |= 1 << 22;                            // Set the usesysdiv bit
    *SYSCTL_RCC = rcc;

    while (!(*SYSCTL_PLLSTAT)) // Wait until the status is 1
        ;

    *SYSCTL_RCC &= ~(1 << 11);
}

GameState gs;

// void polling_timer_init() {
//     uint32_t timer_ports = 1 << 1;
//     *SYSCTL_RCGCTIMER |= timer_ports;
//     while ((*SYSCTL_PRTIMER & timer_ports) != timer_ports)
//         ;
//
//     // Enable timer 1 (poll)
//     *GPTM_CTL(timer_1) &= ~0x1;          // Clear the Timer A enable bit
//     *GPTM_CFG(timer_1) = 0;              // Configure as 32 bit timer
//     *GPTM_TAMR(timer_1) |= 0x2;          // Set timer to be periodic
//     *GPTM_TAILR(timer_1) = 66666666 / 1; // Timer interrupts every 16 cycles
//     *GPTM_IMR(timer_1) |= 0x01;          // Configure timer to use interrupts
//     *REG(0xE000E100) |= 1 << 21; // Timer 1 is 23rd offset in interrupt
//     vtable *GPTM_CTL(timer_1) |= 0x01;  // Enable the timer
// }

int main(void) {
    clock_init();
    matrix_init();
    accel_init();
    accel_interrupts_init();
    game_init(&gs);
    // polling_timer_init();

    int i, x, y;
    render_board(&gs);

    bool has_reset;

    for (;;) {
        // struct AccelerometerData d;
        // MoveDirection dir;
        // accel_get_acceleration(&d);
        //
        // if (d.z < -15000) {
        //     has_reset = true;
        //     continue;
        // }
        //
        // if (has_reset == false) {
        //     continue;
        // }
        //
        // if (d.x < -10000) {
        //     game_move_dir(&gs, MOVE_DOWN);
        //     has_reset = false;
        // }
        // if (d.x > 10000) {
        //     game_move_dir(&gs, MOVE_UP);
        //     has_reset = false;
        // }
        // if (d.y < -10000) {
        //     game_move_dir(&gs, MOVE_LEFT);
        //     has_reset = false;
        // }
        // if (d.y > 10000) {
        //     game_move_dir(&gs, MOVE_RIGHT);
        //     has_reset = false;
        // }
    }
}
