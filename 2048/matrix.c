#include "include/tiva.h"
#include "stdint.h"

typedef volatile struct {
    void *src_buffer;
    void *dest_buffer;
    uint32_t control_word;
    uint32_t unused;
} DmaChannel;

DmaChannel DmaControlTable[15] __attribute__((aligned(1024)));
uint8_t MatrixData[64 * 32 * 4] __attribute__((aligned(32)));
uint8_t MatrixClockPulseByte = 0b10000000; // Clock bit high
uint8_t MatrixResetByte = 0b01110111;      // RGB high and clock low

void matrix_init() {
    //                                                                        //
    //                        Initialize GPIO                                 //
    //                                                                        //
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

    //                                                                        //
    //                        Initialize uDMA                                 //
    //                                                                        //
    DmaChannel *chan;
    uint32_t channel_number;
    const uint8_t map_select = 1;

    *SYSCTL_RCGCDMA = 0x00000001;
    while (*SYSCTL_PRDMA != 0x00000001)
        ;

    *DMA_CFG = 0x00000001;
    *DMA_CTLBASE = (uint32_t)DmaControlTable;

    // Initialize dma channel 4
    channel_number = 4;
    chan = &DmaControlTable[channel_number];
    *DMA_ALTCLR |= 1 << channel_number;
    *DMA_CHMAP0 |= map_select << (channel_number * 4);
    chan->src_buffer = &MatrixData;
    chan->dest_buffer = (void *)(gpio_port_b | (clk_and_rgb_pins_mask << 2));
    chan->control_word = (
          0b11 << 30   // No destination increment
        | 0b00 << 28   // 1 byte destination data size
        | 0b00 << 26   // 1 byte source increment
        | 0b00 << 24   // 1 byte source data size
        | 0b0000 << 14 // 1 item arbitration size
        | 63 << 4      // 64 byte transfer size
        | 0b0 << 3     // Next useburst disabled
        | 0b01 << 0    // Basic transfer mode
    );

    // Initialize dma 6
    channel_number = 6;
    chan = &DmaControlTable[channel_number];
    *DMA_ALTCLR |= 1 << channel_number;
    *DMA_CHMAP0 |= map_select << (channel_number * 4);
    chan->src_buffer = &MatrixClockPulseByte;
    chan->dest_buffer = (void *)(gpio_port_b | (0x80 << 2));
    chan->control_word = (
          0b11 << 30   // No destination increment
        | 0b00 << 28   // 1 byte destination data size
        | 0b11 << 26   // No source increment
        | 0b00 << 24   // 1 byte source data size
        | 0b0000 << 14 // 1 item arbitration size
        | 63 << 4      // 64 byte transfer size
        | 0b0 << 3     // Next useburst disabled
        | 0b01 << 0    // Basic transfer mode
    );

    // Initialize dma 14
    channel_number = 14;
    chan = &DmaControlTable[channel_number];
    *DMA_ALTCLR |= 1 << channel_number;
    *DMA_CHMAP1 |= map_select << 24; // 24 is channel14 select start
    chan->src_buffer = &MatrixClockPulseByte;
    chan->dest_buffer = (void *)(gpio_port_b | (clk_and_rgb_pins_mask << 2));
    chan->control_word = (
          0b11 << 30   // No destination increment
        | 0b00 << 28   // 1 byte destination data size
        | 0b11 << 26   // No source increment
        | 0b00 << 24   // 1 byte source data size
        | 0b0000 << 14 // 1 item arbitration size
        | 63 << 4      // 64 byte transfer size
        | 0b0 << 3     // Next useburst disabled
        | 0b01 << 0    // Basic transfer mode
    );

    *DMA_ENASET |= (1 << 4) | (1 << 6) | (1 << 14);

}
