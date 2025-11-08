#include "include/tiva.h"
#include "stdint.h"

typedef volatile struct {
	void *src_buffer;
	void *dest_buffer;
	uint32_t control_word;
	uint32_t unused;
} DmaChannel;


void matrix_init() {
    uint32_t gpio_ports = 0b010011;
    *SYSCTL_RCGCGPIO |= gpio_ports;
    while (*SYSCTL_PRGPIO != gpio_ports)
        ;

    uint32_t output_enable_pin_mask = 1 << 5;
    *GPIO_DIR(gpio_port_a) |= output_enable_pin_mask;    // enable output
    *GPIO_AFSEL(gpio_port_a) &= ~output_enable_pin_mask; // disable afsel
    *GPIO_DEN(gpio_port_a) |= output_enable_pin_mask;    // digital enable

    uint32_t clk_and_rgb_pins_mask = 0b11110111;        // All pins but pb3
    *GPIO_DIR(gpio_port_b) |= clk_and_rgb_pins_mask;    // enable output
    *GPIO_AFSEL(gpio_port_b) &= ~clk_and_rgb_pins_mask; // disable afsel
    *GPIO_DR8R(gpio_port_b) |= clk_and_rgb_pins_mask;   // drive with 8mA
    *GPIO_DEN(gpio_port_b) |= clk_and_rgb_pins_mask;    // digital enable

    uint32_t address_pins = 0b01111111;     // Use all pins but PE8
    *GPIO_DIR(gpio_port_e) |= address_pins; // enable output
    *GPIO_DEN(gpio_port_e) |= address_pins; // digital enable
}
