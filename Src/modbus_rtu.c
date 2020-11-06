#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "function.h"
#include "modbus_rtu_sensor.h"
#include "upload_method.h"
#include "param.h"
#include "main.h"
#include "nb_tcp.h"

extern gNbTCP nb_tcp;
extern int nb_tcp_rcv_flag;
extern uint8_t nb_tcp_rcv[256];
extern int nbiot_init(void);

extern UART_HandleTypeDef huart2;
extern uint16_t irqNum_idle_uart;
extern char rs485_uart_rcv_buffer[256];
extern uint16_t rs485_uart_rcv_len;

extern mqtt_config_t mqtt_config;

sensorData g_sensorData;
uint8_t ModbusRtuSendFlag = 0;
uint8_t ModbusRtuSeq = 0;
uint8_t ModbusRtuValueFlag = 0;

static int modbusRtuGetFlag;

void ModbusRtuMqtt()
{
	if (ModbusRtuValueFlag) {
		char buf[512];
		memset(buf, 0, sizeof(buf));

		buf[0] = '{';
		for (int i = 0; i < TOTALRTUNUM; i++) {
			if (g_sensorData.sensorConf.rtuModbusTable[i].name[1] == '\0')
				continue;

			char modbusBuf[16] = {0};
			get_value_str(g_sensorData.sensorConf.rtuModbusTable[i].name, 16, modbusBuf);

			sprintf(buf+strlen(buf), "%s: %d,", modbusBuf, g_sensorData.value[i]);
		}

		int len =  strlen(buf) - 1;
		if (len) {
			buf[len] = '}';
			// FIX ME: upload method
		  uploadMQTT(buf);
			// uploadMQTTDowner(buf);
		} else
			return;

		ModbusRtuValueFlag = 0;
	}
}

static void ModbusRtuRcv()
{
	if (irqNum_idle_uart)
	{
			if (rs485_uart_rcv_buffer[1] == 0x03) {
				g_sensorData.value[ModbusRtuSeq] =  rs485_uart_rcv_buffer[3];
				g_sensorData.value[ModbusRtuSeq] =  (g_sensorData.value[ModbusRtuSeq] << 8) + rs485_uart_rcv_buffer[4];
				modbusRtuGetFlag = 1;
			}
			else {
				g_sensorData.value[ModbusRtuSeq] =  -1;
				modbusRtuGetFlag = 0;
			}
				
			rs485_uart_rcv_len = 0;
			irqNum_idle_uart = 0;
			memset(rs485_uart_rcv_buffer, 0, sizeof(rs485_uart_rcv_buffer));
	} else {
		g_sensorData.value[ModbusRtuSeq] =  -1;
		modbusRtuGetFlag = 0;
	}
}

static void ModbusRtuSend(uint8_t *data)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart2, data, MODBUSLENGH, 10000);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void ModbusRtuUnitSndAndRcv()
{
	uint8_t sensorRead[8];

	for (int i = 0; i < TOTALRTUNUM; i++) {
		if (g_sensorData.sensorConf.rtuModbusTable[i].name[1] == '\0')
			continue;

		memset(sensorRead, 0, sizeof(sensorRead));
		sensorRead[0] = g_sensorData.sensorConf.rtuModbusTable[i].addr;
		sensorRead[1] = 0x03;
		sensorRead[2] = 0x00;
		sensorRead[3] = g_sensorData.sensorConf.rtuModbusTable[i].reg - 1;
		sensorRead[4] = 0x00;
		sensorRead[5] = 0x01;
		unsigned short res = crc_Compute(sensorRead, 6);
		sensorRead[6] = res & 0x00ff;
		sensorRead[7] = res >> 8;

		ModbusRtuSeq = i;
		HAL_Delay(30);

		for(int j = 0; j < 3; j++) {
			if (modbusRtuGetFlag) {
				modbusRtuGetFlag = 0;
				break;
			}

			ModbusRtuSend(sensorRead);
			// HAL_Delay(100);
			HAL_Delay(mqtt_config.szRtuTimeout);
			ModbusRtuRcv();
		}
		HAL_Delay(1000);
	}
		ModbusRtuValueFlag = 1;
}

void tcpRcvAndSnd()
{
		// interval time 5s for each product
		// FIX ME: Timer Used Required?
		// FIX ME: Interval Time Set Required
		HAL_Delay(15000);
		gNbTCP *pNbTcp = &nb_tcp;

		nbiot_init();
		pNbTcp->tcpConnect();

		// handle data from nb by tcp server to products
		pNbTcp->tcpReceive();
		HAL_Delay(10);

		if (nb_tcp_rcv_flag) {
			// DebugPrintf("=>%s-%d<=\r\n", g_nbiot_RxBuffer, g_nbiot_rx_len);
			nb_tcp_rcv_flag = 0;
			// FIX ME: len required if necessary
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
			HAL_UART_Transmit(&huart2, nb_tcp_rcv, strlen(nb_tcp_rcv), 10000);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		}

		// handle data from rs485 which is the response to TCP server
		if (irqNum_idle_uart) {
			DebugPrintf("\r\n");
			DebugPrintf((char *) rs485_uart_rcv_buffer);
			pNbTcp->tcpSend((unsigned char*)rs485_uart_rcv_buffer, rs485_uart_rcv_len);

			memset(rs485_uart_rcv_buffer, 0, sizeof(rs485_uart_rcv_buffer));
			rs485_uart_rcv_len = 0;
			irqNum_idle_uart = 0;
		}
}
