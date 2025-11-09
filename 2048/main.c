#include "include/animations.h"
#include "include/game-logic.h"
#include "include/matrix.h"
#include "include/rendering.h"
#include "include/tiva.h"

void game_timer_init();

GameState gs;

void interrupt_init() {
    // enable Port C clock
    *SYSCTL_RCGCGPIO |= 0x4; // Enable Port C
    while ((*SYSCTL_PRGPIO & 0x4) == 0)
        ;

    //  PC0-PC3 as inputs for tilt directions
    uint32_t tilt_pins = 0x0F;            // PC0, PC1, PC2, PC3
    *GPIO_DIR(gpio_port_c) &= ~tilt_pins; // set as inputs
    *GPIO_DEN(gpio_port_c) |= tilt_pins;  // digital enable
    *GPIO_PUR(gpio_port_c) |= tilt_pins;  // enable pull-ups

    // configure int on falling edge
    *GPIO_IS(gpio_port_c) &= ~tilt_pins;  // edge-sensitive
    *GPIO_IBE(gpio_port_c) &= ~tilt_pins; // single edge
    *GPIO_IEV(gpio_port_c) |= tilt_pins;  // rising edge (int if HIGH)
    *GPIO_ICR(gpio_port_c) |= tilt_pins;  // flear any pending
    *GPIO_IM(gpio_port_c) |= tilt_pins;   // enable interrupts

    // enable Port C interrupt in NVIC
    *((volatile uint32_t *)0xE000E100) |= (1 << 2);
}

void gpio_port_c_handler() {
    uint32_t interrupt_status = *GPIO_MIS(gpio_port_c);
    uint8_t tilt_direction = 0;

    if (interrupt_status & 0x01)
        tilt_direction = 0; // PC0 - Left
    if (interrupt_status & 0x02)
        tilt_direction = 1; // PC1 - Right
    if (interrupt_status & 0x04)
        tilt_direction = 2; // PC2 - Up
    if (interrupt_status & 0x08)
        tilt_direction = 3; // PC3 - Down

    game_move(&gs, tilt_direction);

    // clear interrupts
    *GPIO_ICR(gpio_port_c) = interrupt_status;
}

int main(void) {
    matrix_init();
    render_init_colors();
    game_init(&gs);
    render_board(&gs);
    render_board(&gs);
    interrupt_init();

    for (;;)
        ;
}
