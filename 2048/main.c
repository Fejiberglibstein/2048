extern int accel_init(void);
#include "include/tiva.h"
#include <stdbool.h>
#include <stdint.h>

#define assert(cond)                                                           \
    if (!(cond))                                                               \
        while (true)                                                           \
            ;

// 0x19 << 1, for the MSA and setting bit 0 to 0 for write
#define LSM303AGR_ADDR_WRITE 0x32
// (0x19 << 1) | 1, for the MSA and setting bit 0 to 1 for read
#define LSM303AGR_ADDR_READ 0x33

bool i2c_read(uint32_t i2c_base, uint8_t address, uint8_t *result);
bool i2c_write(uint32_t i2c_base, uint8_t address, uint8_t data);

void i2c_init() {
    // Enable I2C1 clock
    *SYSCTL_RCGCI2C |= 0x2; // Enable I2C1

    // Wait for I2C to be ready
    while ((*SYSCTL_PRI2C & 0x2) != 0x2)
        ;

    // Enable GPIO Port A clock
    *SYSCTL_RCGCGPIO |= 0x1; // Enable Port A

    // Wait for GPIO to be ready
    while ((*SYSCTL_PRGPIO & 0x1) != 0x1)
        ;

    // Configure PA6 and PA7 for I2C1
    uint32_t i2c_pins = 0xC0; // PA6 and PA7 (bits 6,7)

    *GPIO_DEN(gpio_port_a) |= i2c_pins;   // Digital enable
    *GPIO_DIR(gpio_port_a) |= i2c_pins;   // Enable output
    *GPIO_AFSEL(gpio_port_a) |= i2c_pins; // Enable alternate function
    *GPIO_PUR(gpio_port_a) |= i2c_pins;   // Enable pull-ups
    *GPIO_ODR(gpio_port_a) |= 0x80;       // PA7 (SDA) open drain
    uint32_t port_control = *GPIO_PCTL(gpio_port_a);
    port_control &= ~0xFF000000; // Clear PMC6 and PMC7
    port_control |= 0x33000000;  // Set PMC6=3, PMC7=3
    *GPIO_PCTL(gpio_port_a) = port_control;

    // Configure I2C1 master
    *I2C_MCR(I2C_1) = 0x10; // Enable master
    *I2C_MTPR(I2C_1) = (16 * 1000000 / (2 * (6 + 4) * 100000)) - 1;
}

// write data to accelerometer register
void accel_write_reg(uint8_t reg_addr, uint8_t data) {
    // Set slave address for write
    *I2C_MSA(I2C_1) = LSM303AGR_ADDR_WRITE;

    // Write register address first
    *I2C_MDR(I2C_1) = reg_addr;
    *I2C_MCS(I2C_1) = 0b00011; // START + RUN

    // Wait for completion
    while (*I2C_MCS(I2C_1) & (1 << 6))
        ;
    assert((*I2C_MCS(I2C_1) & 0b10) == 0);

    // Write data byte
    *I2C_MDR(I2C_1) = data;
    *I2C_MCS(I2C_1) = 0b00101; // RUN + STOP

    // Wait for completion
    while (*I2C_MCS(I2C_1) & (1 << 6))
        ;
    assert((*I2C_MCS(I2C_1) & 0b10) == 0);
}

// Read data from accelerometer register
uint8_t accel_read_reg(uint8_t reg_addr) {
    // Write register address (no stop)
    *I2C_MSA(I2C_1) = LSM303AGR_ADDR_WRITE;
    *I2C_MDR(I2C_1) = reg_addr;
    *I2C_MCS(I2C_1) = 0b00011; // START + RUN

    // Wait for completion
    while (*I2C_MCS(I2C_1) & (1 << 6))
        ;
    assert((*I2C_MCS(I2C_1) & 0b10) == 0);

    // Repeated start for read
    *I2C_MSA(I2C_1) = LSM303AGR_ADDR_READ;
    *I2C_MCS(I2C_1) = 0b00111; // START + RUN + STOP

    // Wait for completion
    while (*I2C_MCS(I2C_1) & (1 << 6))
        ;
    assert((*I2C_MCS(I2C_1) & 0b10) == 0);

    return *I2C_MDR(I2C_1);
}

int main() {

    i2c_init(); // gpio, i2c init

    // Test the working i2c functions first
    assert(i2c_write(I2C_1, 0b00110010, 0x0F));
    uint8_t result;
    assert(i2c_read(I2C_1, 0b00110010, &result));

    // Test accel_read_reg with WHO_AM_I register
    uint8_t who_am_i = accel_read_reg(0x0F);
    assert(who_am_i == 0x33);  // Should be 0x33 for LSM303AGR

    // Test accel_write_reg and accel_read_reg with a writable register
    // Use CTRL_REG1_A (0x20) - this register can be written to
    accel_write_reg(0x20, 0x57);              // Write test value
    uint8_t read_back = accel_read_reg(0x20); // Read it back
    // assert(read_back == 0x57);  // Should match what we wrote
}

// Returns true on success, false if i2c failed
bool i2c_read(uint32_t i2c_base, uint8_t address, uint8_t *result) {
    *I2C_MSA(i2c_base) = address | 1; // Write address with receive
    // Set run, start, and stop
    *I2C_MCS(i2c_base) = 0b00111;

    // Loop while busy bit is high
    while (*I2C_MCS(i2c_base) & (1 << 6))
        ;

    if (*I2C_MCS(i2c_base) & 0b10)
        return false;

    *result = *I2C_MDR(i2c_base); // Read data

    return true;
}

bool i2c_write(uint32_t i2c_base, uint8_t address, uint8_t data) {
    *I2C_MSA(i2c_base) = address | 0; // Write address with transmit

    *I2C_MDR(i2c_base) = data; // Write data

    // Set run, start
    *I2C_MCS(i2c_base) = 0b00011;

    // Loop while busy bit is high
    while (*I2C_MCS(i2c_base) & (1 << 6))
        ;

    return (*I2C_MCS(i2c_base) & 0b10) == 0;
}
