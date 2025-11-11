#include "include/accelerometer.h"
#include "include/tiva.h"

void accel_init() {
    uint8_t i2c_ports = 0b0010;
    *SYSCTL_RCGCI2C |= i2c_ports;
    while ((*SYSCTL_PRI2C & i2c_ports) != i2c_ports)
        ;

    uint8_t gpio_ports = 0b00000001; // spi is on port a
    *SYSCTL_RCGCGPIO |= gpio_ports;
    while ((*SYSCTL_PRGPIO & gpio_ports) != gpio_ports)
        ;

    uint8_t i2c_pins = 0b11000000; // Use pins 6 & 7
    *GPIO_DIR(gpio_port_a) |= i2c_pins;
    *GPIO_DEN(gpio_port_a) |= i2c_pins;
    *GPIO_AFSEL(gpio_port_a) |= i2c_pins;
    uint32_t pctl = *GPIO_PCTL(gpio_port_a);
    pctl &= 0x00FFFFFF;
    pctl |= 0x33000000;
    *GPIO_PCTL(gpio_port_a) = pctl;
    *GPIO_PUR(gpio_port_a) |= i2c_pins; // PUR for clock
    *GPIO_ODR(gpio_port_a) |= 1 << 7;

    *I2C_MCR(i2c_1) = 0x00000010; // Enable i2c as master
    // (16MHz/(2*(6+4)*100000))-1;
    *I2C_MTPR(i2c_1) = (66666666 / (2 * (6 + 4) * 100000)) - 1;

}

uint8_t accel_read(uint8_t reg_addr) {
    *I2C_MSA(i2c_1) = 0x32;            // Write address with transmit
    *I2C_MDR(i2c_1) = reg_addr;        // Write data
    *I2C_MCS(i2c_1) = 0b00111;         // Set run & start
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    *I2C_MSA(i2c_1) = 0x33;            // Write address with receive
    *I2C_MCS(i2c_1) = 0b00111;         // Set run, start, and stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    return *I2C_MDR(i2c_1);
}

void accel_write(uint8_t reg_addr, uint8_t data) {
    *I2C_MSA(i2c_1) = 0x32;            // Write address with transmit
    *I2C_MDR(i2c_1) = reg_addr;        // Write data
    *I2C_MCS(i2c_1) = 0b00111;         // Set run & start
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    *I2C_MSA(i2c_1) = 0x32;            // Write address with receive
    *I2C_MDR(i2c_1) = data;            // Write data
    *I2C_MCS(i2c_1) = 0b00111;         // Set run, start, and stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error
}
