#include <stdint.h>
#define NBIOT_VALUE_START			4001
#define NBIOT_VALUE_END				4500
#define MQTT_VALUE_START			4501
#define MQTT_VALUE_END				4700
#define COAP_VALUE_START			4701
#define COAP_VALUE_END				4800

#define MODBUSRTU_VALUE_START 501
#define MODBUSRTU_VALUE_END		641

#define NBIOT_STATUS_VALUE_START	5001
#define NBIOT_STATUS_VALUE_END		5100
#define MQTT_STATUS_VALUE_START		5501
#define MQTT_STATUS_VALUE_END		5700

#define SSL_VALUE_START 651 
#define SSL_VALUE_END  655

#define TCP_VALUE_START 661
#define TCP_VALUE_END 	679

void Modbus_Slave_USBPutData(uint8_t *buf, uint32_t len);
uint8_t Do_W_HOLD_REG_Function(uint16_t addr, uint16_t *data, uint16_t num, uint8_t n);
void Do_R_HOLD_REG_Function(unsigned short addr, unsigned short* data, uint16_t num, uint8_t n);