#include "stdint.h"

void accel_init();

void accel_write(uint8_t address, uint8_t data);
uint8_t accel_read(uint8_t address);
