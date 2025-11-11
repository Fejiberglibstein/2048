#include "include/accelerometer.h"
#include "include/gameplay.h"
#include "include/matrix.h"
#include "include/tiva.h"
#include <stdbool.h>

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
    *I2C_MTPR(i2c_1) = (66666666 / (2 * (6 + 4) * 100000)) - 1;
}

uint8_t accel_read(uint8_t reg_addr) {
    matrix_pause();

    *I2C_MSA(i2c_1) = 0x32;            // Write address with transmit
    *I2C_MDR(i2c_1) = reg_addr;        // Write address
    *I2C_MCS(i2c_1) = 0b00111;         // Set run start, & stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    *I2C_MSA(i2c_1) = 0x33;            // Write address with receive
    *I2C_MCS(i2c_1) = 0b00111;         // Set run, start, and stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error
    uint8_t data = *I2C_MDR(i2c_1);

    matrix_resume();
    return data;
}

void accel_write(uint8_t reg_addr, uint8_t data) {
    matrix_pause();

    *I2C_MSA(i2c_1) = 0x32;            // Write address with transmit
    *I2C_MDR(i2c_1) = reg_addr;        // Write address
    *I2C_MCS(i2c_1) = 0b00011;         // Set run & start
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    *I2C_MDR(i2c_1) = data;            // Write data
    *I2C_MCS(i2c_1) = 0b00101;         // Set run & stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    matrix_resume();
}

// void accel_get_acceleration(struct AccelerometerData *data) {
//     uint8_t *buf = (void *)data;
//
//     uint8_t addrs[6] = {
//         ACCEL_OUT_X_L_A,
//         ACCEL_OUT_X_H_A,
//         ACCEL_OUT_Y_L_A,
//         ACCEL_OUT_Y_H_A,
//         ACCEL_OUT_Z_L_A,
//         ACCEL_OUT_Z_H_A,
//     };
//
//     int i, j;
//     for (i = 0; i < 6; i++) {
//         *buf++ = accel_read(addrs[i]);
//         for (j = 0; j < 10000; j++) {
//             __asm__("\tnop");
//         }
//     }
// }

void accel_get_acceleration(struct AccelerometerData *data) {
    matrix_pause();

    *I2C_MSA(i2c_1) = 0x32;                   // Write address with transmit
    *I2C_MDR(i2c_1) = ACCEL_OUT_X_L_A | 0x80; // address with increment
    *I2C_MCS(i2c_1) = 0b00011;                // Set run & start
    while (*I2C_MCS(i2c_1) & (1 << 0))        // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    // This is a 6 byte value
    uint8_t *casted = (void *)data;

    // Read the first byte
    *I2C_MSA(i2c_1) = 0x33;            // Write address with receive
    *I2C_MCS(i2c_1) = 0b01011;         // Set run, start, & ack
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error

    *casted++ = *I2C_MDR(i2c_1);

    // Read the next 4 bytes
    int i = 0;
    for (i = 0; i < 4; i++) {
        *I2C_MCS(i2c_1) = 0b01001;         // Set run & ack
        while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
            ;
        assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error
        *casted++ = *I2C_MDR(i2c_1);
    }

    // Read the last byte
    *I2C_MCS(i2c_1) = 0b00101;         // Set run & stop
    while (*I2C_MCS(i2c_1) & (1 << 0)) // Loop while busy bit is high
        ;
    assert((*I2C_MCS(i2c_1) & 0x0E) == 0); // assert no error
    *casted++ = *I2C_MDR(i2c_1);

    matrix_resume();
}

// Configures the accelerometer to cause xyz interrupts on PA2
void accel_interrupts_init() {
    uint8_t gpio_ports = 0b00000001; // interrupts is on port a
    *SYSCTL_RCGCGPIO |= gpio_ports;
    while ((*SYSCTL_PRGPIO & gpio_ports) != gpio_ports)
        ;

    uint8_t interrupt_pin = (1 << 2);           // Interrupt is on pin 2
    *GPIO_DEN(gpio_port_a) |= interrupt_pin;    // turn it on
    *GPIO_DIR(gpio_port_a) &= ~interrupt_pin;   // Toggle as input
    *GPIO_AFSEL(gpio_port_a) &= ~interrupt_pin; // no afsel

    // 1. Mask the corresponding port by clearing the IME field in the GPIOIM
    // register.
    *GPIO_IM(gpio_port_a) &= ~interrupt_pin; // disable interrupts
    // 2. Configure the IS field in the GPIOIS register and the IBE field in the
    // GPIOIBE register.
    *GPIO_IBE(gpio_port_a) &= ~interrupt_pin; // one edge causes interrupt
    *GPIO_IS(gpio_port_a) &= ~interrupt_pin;  // edge sensitive
    *GPIO_IEV(gpio_port_a) |= interrupt_pin;  // Rising edge triggered
    // 3. Clear the GPIORIS register.
    *GPIO_RIS(gpio_port_a) = 0;
    // 4. Unmask the port by setting the IME field in the GPIOIM register.
    *GPIO_IM(gpio_port_a) |= interrupt_pin;
    *REG(0xE000E100) |= 1 << 0; // GPIO Port a is 0th offset in vectable

    accel_write(ACCEL_CTRL_REG5_A, 0x80); // Reset memory
    accel_write(ACCEL_CTRL_REG1_A, 0x57); // ODR = 5, enable xyz
    accel_write(ACCEL_CTRL_REG2_A, 0x07); // High pass for all interrupts
    accel_write(ACCEL_CTRL_REG3_A, 0x40); // Interrupts for AOI1, AOI2, & click
    accel_write(ACCEL_CTRL_REG4_A, 0x80); // Block data update TODO
    accel_write(ACCEL_CTRL_REG5_A, 0x08); // Latch interrupt on Int1 TODO A
    accel_write(ACCEL_CTRL_REG6_A, 0x02); // interrupts are active low
    accel_write(ACCEL_INT1_CFG_A, 0x01);  // all interrupts
    accel_write(ACCEL_INT1_THS_A, 255);
    accel_write(ACCEL_INT1_DURATION_A, 10); // 100 ms debounce
}

#define ACCEL_INT_Z_HIGH (1 << 5)
#define ACCEL_INT_Z_LOW (1 << 4)
#define ACCEL_INT_Y_HIGH (1 << 3)
#define ACCEL_INT_Y_LOW (1 << 2)
#define ACCEL_INT_X_HIGH (1 << 1)
#define ACCEL_INT_X_LOW (1 << 0)

extern GameState gs;

void accelerometer_interrupt_handler(void) {
    *GPIO_ICR(gpio_port_a) |= (1 << 2); // Clear interrupt on pin 2
    uint8_t int_src = accel_read(ACCEL_INT1_SRC_A);

    if (int_src & ACCEL_INT_Z_LOW) {
    }

    // clang-format off
    if (int_src & ACCEL_INT_Y_HIGH) game_move_dir(&gs, MOVE_UP);
    if (int_src & ACCEL_INT_Y_LOW) game_move_dir(&gs, MOVE_DOWN);
    if (int_src & ACCEL_INT_X_HIGH) game_move_dir(&gs, MOVE_LEFT);
    if (int_src & ACCEL_INT_X_LOW) game_move_dir(&gs, MOVE_RIGHT);
    // clang-format on
}
