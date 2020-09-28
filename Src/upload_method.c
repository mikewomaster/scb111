#include "nb_mqtt.h"
#include <stdio.h>
#include <string.h>

extern gNbMQTT nb_mqtt;
extern char rs485_uart_rcv_buffer[256];

void uploadMQTT(char *buf)
{
		gNbMQTT *pNbMqtt = &nb_mqtt;
		pNbMqtt->mqttOpen();

		mqttConfig type;
		for (type = URL; type < ClientID; type++) {
			pNbMqtt->mqttConfigure(type);
			HAL_Delay(10);
		}

		pNbMqtt->mqttConnect();
	  pNbMqtt->mqttPub(buf, strlen(buf));

		pNbMqtt->mqttDisconnect();
		pNbMqtt->mqttClose();
}

void rs485Mqtt()
{
	if (strlen(rs485_uart_rcv_buffer))
		uploadMQTT(rs485_uart_rcv_buffer);
}
