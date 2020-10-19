#include "nb_mqtt.h"
#include <stdio.h>
#include <string.h>

extern gNbMQTT nb_mqtt;
extern char rs485_uart_rcv_buffer[256];

void uploadMQTT(char *buf)
{
		gNbMQTT *pNbMqtt = &nb_mqtt;
		DebugPrintf("open\r\n");
		HAL_Delay(100);
		pNbMqtt->mqttOpen();

		mqttConfig type;
		DebugPrintf("configure\r\n");
		HAL_Delay(100);
		for (type = URL; type <= ClientID; type++) {
			pNbMqtt->mqttConfigure(type);
			HAL_Delay(10);
		}

		DebugPrintf("connect\r\n");
		HAL_Delay(100);
		pNbMqtt->mqttConnect();
		DebugPrintf("pub\r\n");
		HAL_Delay(100);
	  pNbMqtt->mqttPub(buf, strlen(buf));

		DebugPrintf("disconnect\r\n");
		HAL_Delay(100);
		pNbMqtt->mqttDisconnect();
		DebugPrintf("close\r\n");
		HAL_Delay(100);
		pNbMqtt->mqttClose();
}

void rs485Mqtt()
{
	if (strlen(rs485_uart_rcv_buffer))
		uploadMQTT(rs485_uart_rcv_buffer);
}
