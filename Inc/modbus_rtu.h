#ifndef __MODBUS_RTU__
#define __MODBUS_RTU__
#include <stdint.h>

typedef struct {
	char name[8];
	uint16_t addr;
	uint16_t reg;
	uint16_t num;
}rtuUnit;

typedef struct __rtuModbus {
		rtuUnit rtuModbusTable[20];
} rtuModbus;

#endif
