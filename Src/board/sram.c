#include "sram.h"

SRAM_t sram;

void SRAM_IO_Init()
{
	GpioInit( &sram.Spi.Nss, SRAM_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
}

void SRAM_Init()
{
	SpiInit( &sram.Spi, SPI_1, SRAM_MOSI, SRAM_MISO, SRAM_SCLK, NC );
	SRAM_IO_Init();
}

void SRAM_Read( uint32_t address, uint8_t *buffer, uint32_t size )
{
		GpioWrite( &sram.Spi.Nss, 0 );
	
    SpiInOut( &sram.Spi, SRAM_READ_CMD_SLOW_FREQ );    
    SpiInOut( &sram.Spi, address & 0x00FF );
		SpiInOut( &sram.Spi, ( address & 0xFF00 ) >> 8 );
		SpiInOut( &sram.Spi, ( address & 0xFF0000 ) >> 16 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &sram.Spi, 0 );
    }
    GpioWrite( &sram.Spi.Nss, 1 );
}

void SRAM_Write( uint32_t address, uint8_t *buffer, uint32_t size )
{
    GpioWrite( &sram.Spi.Nss, 0 );

    SpiInOut( &sram.Spi, SRAM_WRITE_CMD );
    SpiInOut( &sram.Spi, address & 0x00FF );
		SpiInOut( &sram.Spi, ( address & 0xFF00 ) >> 8 );
		SpiInOut( &sram.Spi, ( address & 0xFF0000 ) >> 16 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &sram.Spi, buffer[i] );
    }
    GpioWrite( &sram.Spi.Nss, 1 );
}

void SRAM_Set_Burst( uint32_t address, uint8_t *buffer, uint32_t size )
{
		GpioWrite( &sram.Spi.Nss, 0 );
    SpiInOut( &sram.Spi, SRAM_SET_BURST_CMD );    
    GpioWrite( &sram.Spi.Nss, 1 );
}

void SRAM_Read_ID( uint32_t address, uint8_t *buffer, uint32_t size )
{
	  GpioWrite( &sram.Spi.Nss, 0 );
	
    SpiInOut( &sram.Spi, SRAM_READ_ID );    
    SpiInOut( &sram.Spi, address & 0x00FF );
		SpiInOut( &sram.Spi, ( address & 0xFF00 ) >> 8 );
		SpiInOut( &sram.Spi, ( address & 0xFF0000 ) >> 16 );
    for( uint16_t i = 0; i < 8; i++ )
    {
        buffer[i] = SpiInOut( &sram.Spi, buffer[i] );
    }
    GpioWrite( &sram.Spi.Nss, 1 );
}

void SRAM_Reset( uint32_t address, uint8_t *buffer, uint32_t size )
{
	GpioWrite( &sram.Spi.Nss, 0 );	
	SpiInOut( &sram.Spi, SRAM_RESET_ENABLE ); 	
	GpioWrite( &sram.Spi.Nss, 1 );
	
	DelayMsMcu(50);
	
	GpioWrite( &sram.Spi.Nss, 0 );	
	SpiInOut( &sram.Spi, SRAM_RESET ); 	
	GpioWrite( &sram.Spi.Nss, 1 );
	
}