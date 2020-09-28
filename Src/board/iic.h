#ifndef _IIC_H_
#define _IIC_H_

#include "main.h"

#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t unsigned char

#define I2C_WR	0	
#define I2C_RD	1	
 
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(uint8_t ack);
uint8_t I2C_RecvByte(void);
uint8_t I2C_RecvACK(void);
 
uint8_t I2C_SendByte(uint8_t data);
uint8_t I2C_Wait_ACK(void);
 
uint8_t I2C_CheckDevice(uint8_t _Address);

#endif
