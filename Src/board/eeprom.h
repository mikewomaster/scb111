#ifndef _EEPROM_H_
#define _EEPROM_H_
 
#include "main.h" 
 
 
#define EE_DEV_ADDR		0xa0		
#define EE_PAGE_NUM		256			
#define EE_PAGE_SIZE	32
 
 
uint8_t eeprom_CheckOk(void);
uint8_t eeprom_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t eeprom_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
void eeprom_Erase(void);
void eeprom_Test(void);
 
 
 
#endif
