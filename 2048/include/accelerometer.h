#include "stdint.h"

#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

struct AccelerometerData {
    int16_t x;
    int16_t y;
    int16_t z;
};

void accel_init();
void accel_interrupts_init();

void accel_write(uint8_t address, uint8_t data);
uint8_t accel_read(uint8_t address);
void accel_get_acceleration(struct AccelerometerData *data);

#define ACCEL_STATUS_REG_AUX_A 0x07
#define ACCEL_OUT_TEMP_L_A 0x0C
#define ACCEL_OUT_TEMP_H_A 0x0D
#define ACCEL_INT_COUNTER_REG_A 0x0E
#define ACCEL_WHO_AM_I_A 0x0F
#define ACCEL_TEMP_CFG_REG_A 0x1F
#define ACCEL_CTRL_REG1_A 0x20
#define ACCEL_CTRL_REG2_A 0x21
#define ACCEL_CTRL_REG3_A 0x22
#define ACCEL_CTRL_REG4_A 0x23
#define ACCEL_CTRL_REG5_A 0x24
#define ACCEL_CTRL_REG6_A 0x25
#define ACCEL_DATACAPTURE_A 0x26
#define ACCEL_STATUS_REG_A 0x27
#define ACCEL_OUT_X_L_A 0x28
#define ACCEL_OUT_X_H_A 0x29
#define ACCEL_OUT_Y_L_A 0x2A
#define ACCEL_OUT_Y_H_A 0x2B
#define ACCEL_OUT_Z_L_A 0x2C
#define ACCEL_OUT_Z_H_A 0x2D
#define ACCEL_FIFO_CTRL_REG_A 0x2E
#define ACCEL_FIFO_SRC_REG_A 0x2F
#define ACCEL_INT1_CFG_A 0x30
#define ACCEL_INT1_SRC_A 0x31
#define ACCEL_INT1_THS_A 0x32
#define ACCEL_INT1_DURATION_A 0x33
#define ACCEL_INT2_CFG_A 0x34
#define ACCEL_INT2_SRC_A 0x35
#define ACCEL_INT2_THS_A 0x36
#define ACCEL_INT2_DURATION_A 0x37
#define ACCEL_CLICK_CFG_A 0x38
#define ACCEL_CLICK_SRC_A 0x39
#define ACCEL_CLICK_THS_A 0x3A
#define ACCEL_TIME_LIMIT_A 0x3B
#define ACCEL_TIME_LATENCY_A 0x3C
#define ACCEL_TIME_WINDOW_A 0x3D
#define ACCEL_ACT_THS_A 0x3E
#define ACCEL_ACT_DUR_A 0x3F
#define _MAGN_OFFSET_X_REG_L_M 0x45
#define _MAGN_OFFSET_X_REG_H_M 0x46
#define _MAGN_OFFSET_Y_REG_L_M 0x47
#define _MAGN_OFFSET_Y_REG_H_M 0x48
#define _MAGN_OFFSET_Z_REG_L_M 0x49
#define _MAGN_OFFSET_Z_REG_H_M 0x4A
#define _MAGN_WHO_AM_I_M 0x4F
#define _MAGN_CFG_REG_A_M 0x60
#define _MAGN_CFG_REG_B_M 0x61
#define _MAGN_CFG_REG_C_M 0x62
#define _MAGN_INT_CRTL_REG_M 0x63
#define _MAGN_INT_SOURCE_REG_M 0x64
#define _MAGN_INT_THS_L_REG_M 0x65
#define _MAGN_INT_THS_H_REG_M 0x66
#define _MAGN_STATUS_REG_M 0x67
#define _MAGN_OUTX_L_REG_M 0x68
#define _MAGN_OUTX_H_REG_M 0x69
#define _MAGN_OUTY_L_REG_M 0x6A
#define _MAGN_OUTY_H_REG_M 0x6B
#define _MAGN_OUTZ_L_REG_M 0x6C
#define _MAGN_OUTZ_H_REG_M 0x6D

#endif // _ACCELEROMETER_H
