#ifndef __MODBUS_RTU_SENSOR__
#define __MODBUS_RTU_SENSOR__

#include "modbus_rtu.h"

#define TOTALRTUNUM 20
#define MODBUSLENGH 8
#define MODBUSRTUUNITLENGTH 14

void ModbusRtuMqtt();
void ModbusRtuRcv();
void ModbusRtuUnitSndAndRcv();

typedef struct __sensor {
	rtuModbus sensorConf;
	short value[20];
}sensorData;

#endif