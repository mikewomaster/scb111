#ifndef __SRAM_H__
#define __SRAM_H__

#include "spi.h"

typedef struct SX126x_s
{
    Gpio_t        CE;
    Gpio_t        SCLK;
    Gpio_t        MOSI;
    Gpio_t        MISO;
    Spi_t         Spi;
}SRAM_t;


#define SRAM_MOSI                                  PB_5
#define SRAM_MISO                                  PB_4
#define SRAM_SCLK                                  PB_3
#define SRAM_NSS																	 PA_15

// cmd
#define SRAM_SET_BURST_CMD  											0xC0
#define SRAM_READ_CMD_SLOW_FREQ										0x03
#define SRAM_READ_CMD_FAST_FREQ										0x0B

#define SRAM_WRITE_CMD														0x02

#define SRAM_READ_ID															0x9F
#define SRAM_RESET_ENABLE													0x66
#define SRAM_RESET																0x99

#endif