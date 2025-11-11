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

int main(void) {
    clock_init();
    // matrix_init();
}
