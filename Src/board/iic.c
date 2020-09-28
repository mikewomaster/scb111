#include "iic.h"
 	
#define I2C_SCL_PIN		GPIO_PIN_8			
#define I2C_SDA_PIN		GPIO_PIN_9			
 

#define I2C_SCL_1()  HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN, GPIO_PIN_SET)		/* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN, GPIO_PIN_RESET)		/* SCL = 0 */

#define I2C_SDA_1()  HAL_GPIO_WritePin(GPIOB, I2C_SDA_PIN, GPIO_PIN_SET)		/* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(GPIOB, I2C_SDA_PIN, GPIO_PIN_RESET)		/* SDA = 0 */

#define I2C_SDA_READ()  HAL_GPIO_ReadPin(GPIOB, I2C_SDA_PIN)	


void I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;   
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); 
}
 
static void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; 
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
 
static void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	// ?
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; 
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/**************************************
**************************************/
void I2C_Start()
{
	SDA_OUT();   
  I2C_SDA_1(); 
  I2C_SCL_1();  
  DelayMsMcu(10);  
  I2C_SDA_0(); 
  DelayMsMcu(10);
  I2C_SCL_0(); 	// ?
}
 
/**************************************
**************************************/
void I2C_Stop()
{
	SDA_OUT();    
	I2C_SDA_0();  
	I2C_SCL_1();  
	DelayMsMcu(10); 
  I2C_SDA_1();  
  DelayMsMcu(10);
}

/**************************************
**************************************/
uint8_t I2C_SendByte(uint8_t data)
{
	uint8_t i; 
	
	SDA_OUT(); 	    
  I2C_SCL_0();
  for( i=0;i<8;i++)
  {              
		if(data&0x80)
			I2C_SDA_1();    
		else 
			I2C_SDA_0();
    data<<=1; 	  
		DelayMsMcu(10);
		I2C_SCL_1();        
		DelayMsMcu(10); 
		I2C_SCL_0();
		DelayMsMcu(10);
	}
	return I2C_Wait_ACK();
}

/**************************************
**************************************/	
 
uint8_t I2C_RecvByte(void)
{
	uint8_t i = 0;
	uint8_t byte = 0;
	
	SDA_IN();
	for(i = 0;i < 8;i++)
	{
		DelayMsMcu(10);
		I2C_SCL_1();	
		DelayMsMcu(10);
		byte <<= 1;      
		if(I2C_SDA_READ())   
		{
			byte |= 0x01;
		}
		I2C_SCL_0();
		DelayMsMcu(10);
	}
	return byte;
}

/**************************************
**************************************/
void I2C_SendACK(uint8_t ack)
{
	SDA_OUT();
	I2C_SCL_0();
	DelayMsMcu(10);
	if(ack)
		I2C_SDA_1();
	else 
		I2C_SDA_0();
	I2C_SCL_1(); 
	DelayMsMcu(10);
	I2C_SCL_0();
	DelayMsMcu(10);
}
 
/**************************************
**************************************/
uint8_t I2C_Wait_ACK(void)
{
	uint8_t ucErrTime=0;
 
 	SDA_IN();     	   
	I2C_SCL_1();   
	DelayMsMcu(10);   
	
	while(I2C_SDA_READ())  
	{
		ucErrTime++;
		if(ucErrTime>250)   
		{
			I2C_Stop();     
			return 1;
		}
	}
	I2C_SCL_0();            //   
	return 0;  
}
 

 

 
 
/****************************************************************************
*
*
*
*
****************************************************************************/
uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
 
	I2C_Start();		
 
	
	I2C_SendByte(_Address | I2C_WR);
	ucAck = I2C_Wait_ACK();	
 
	I2C_Stop();		
 
	return ucAck;
}
 
