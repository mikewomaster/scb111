#include "at_command.h"
#include "nb_mqtt.h"
#include "nbiot.h"
#include "param.h"

#include <stdio.h>
#include <string.h>

extern Cel_Status_t nbiot_status;
extern mqtt_config_t mqtt_config;
extern gNbMQTT nb_mqtt;
char *p_mqtt_t = (char *) &mqtt_config;
extern uint16_t mqtt_status;

extern unsigned int gs_baudrate[10];
extern uart_config_t uart_config;
extern UART_HandleTypeDef huart2;

extern nbiot_config_t nbiot_config;

void mqttAddrSet(gNbMQTT* pNbMqtt)
{
	memcpy(pNbMqtt->mqttAddr, p_mqtt_t + 0, 32);
}

void mqttPortSet(gNbMQTT* pNbMqtt)
{
	pNbMqtt->mqttPort = *(p_mqtt_t + 33);
	pNbMqtt->mqttPort = ((pNbMqtt->mqttPort) << 8) + *(p_mqtt_t + 32);
}

void mqttTopicSet(gNbMQTT* pNbMqtt)
{
	memcpy(pNbMqtt->mqttTopic, p_mqtt_t + 34, 64);
}

void mqttClientIdSet(gNbMQTT*pNbMqtt)
{
	memcpy(pNbMqtt->mqttClientID, p_mqtt_t + 98, 16);
}

void mqttIntervalSet(gNbMQTT*pNbMqtt)
{
	pNbMqtt->mqttInterval = *(p_mqtt_t + 133);
	pNbMqtt->mqttInterval = (pNbMqtt->mqttInterval << 8) + *(p_mqtt_t + 132);
	pNbMqtt->mqttInterval = (pNbMqtt->mqttInterval << 8) + *(p_mqtt_t + 131);
  pNbMqtt->mqttInterval = (pNbMqtt->mqttInterval << 8) + *(p_mqtt_t + 130);
}

void mqttUsernameSet(gNbMQTT* pNbMqtt) 
{
	memcpy(pNbMqtt->mqttUsername, p_mqtt_t + 134, 16);
}

void mqttPasswordSet(gNbMQTT* pNbMqtt)
{
	memcpy(pNbMqtt->mqttPassword, p_mqtt_t + 150, 16);
}

void mqttOpenSet(gNbMQTT *pNbMqtt, void (*fn)(void))
{
	pNbMqtt->mqttOpen = fn;
}

void mqttConnectSet(gNbMQTT *pNbMqtt, void (*fn)(void))
{
	pNbMqtt->mqttConnect = fn;
}

void mqttConfigureSet(gNbMQTT *pNbMqtt, void (*fn)(mqttConfig))
{
	pNbMqtt->mqttConfigure = fn;
}

void mqttPubSet(gNbMQTT *pNbMqtt, void (*fn)(uint8_t* , uint16_t))
{
	pNbMqtt->mqttPub = fn;
}

void mqttSubSet(gNbMQTT *pNbMqtt, void (*fn)())
{
	pNbMqtt->mqttSub = fn;
}
	
void mqttCloseSet(gNbMQTT *pNbMqtt, void (*fn)())
{
	pNbMqtt->mqttClose = fn;
}
	
void mqttDisconnectSet(gNbMQTT *pNbMqtt, void (*fn)())
{
	pNbMqtt->mqttDisconnect = fn;
}
	
void mqttCheckSet(gNbMQTT *pNbMqtt, uint8_t (*fn)(void))
{
	pNbMqtt->mqttCheckStatus = fn;
}

static void SetMqttConf(mqttConfig type, char* buf)
{
	if (type == URL) {
		if (strlen((char *)mqtt_config.szServer))
			sprintf(buf, "\"URL\",%s,%d", mqtt_config.szServer, mqtt_config.ulPort);
	}
	else if (type == Topic) {
		if (strlen((char *)mqtt_config.szTopic))
			sprintf(buf, "\"TOPIC\",%s", mqtt_config.szTopic);
	}
	else if (type == UserName) {
		if (strlen((char *)mqtt_config.szUserName))
			sprintf(buf, "\"USERNAME\",%s", mqtt_config.szUserName);
	}
	else if (type == PassWord) {
		if (strlen((char *)mqtt_config.szPassWord))
		sprintf(buf, "\"PASSWORD\",%s", mqtt_config.szPassWord);
	}
	else if (type == ClientID) {
		if (strlen((char *)mqtt_config.szClientID))
		sprintf(buf, "\"CLIENTID\",%s", mqtt_config.szClientID);
	}
}

static void SendMqttConf(AtCommand at, mqttConfig type)
{
	int cmdRet;
	char buf[64] = {0};
	SetMqttConf(type, buf);
	at_command_content(&at, buf);

	at.type = writeAT;
	at_command_factory(&at);

	if (strlen(buf))
		cmdRet = Send_AT_Command(at.cmd, "OK", 2000, 2000);
}

void mqttConfigureHandle (mqttConfig type)
{
	int cmdRet = 0;
	AtCommand at;
	at_command_parament(&at, "SMCONF");

	SendMqttConf(at, type);
}

static void mqttATSend(AtCommand at, char* buf)
{
	int cmdRet = 0;
	at_command_factory(&at);
	cmdRet = Send_AT_Command(at.cmd, buf, 2000, 2000);
}

void mqttPubHandle(uint8_t* msg, uint16_t len)
{
	char cmd[64] = {0};
	int cmdRet;
	int retry_count = 0;

	char buf[64] = {0};
	AtCommand at;
	at_command_parament(&at, "SMPUB");

	if (strlen((char *)mqtt_config.szTopic))
		sprintf(buf, "\"%s\",\"%d\",1,1", mqtt_config.szTopic, len);

	at_command_content(&at, buf);
	at.type = writeAT;

	at_command_factory(&at);

	Send_AT_Command(at.cmd, ">", 10000, 7000);
	Send_AT_Command(msg, "OK", 10000, 7000);

	// Send_AT_Command_2(at.cmd, ">", 10000, 7000, 5000, msg);
}

extern uint32_t g_nbiot_rx_len;
extern char g_nbiot_RxBuffer[512];

void mqttCheckAPN()
{
	int cmdRet = 0;

	AtCommand at;
	at_command_parament(&at, "CNCFG");
	
	at.type = readAT;
	mqttATSend(at, "OK");
	
	if(g_nbiot_rx_len > 0)
	{
		int  activeID;
		char apnBuffer[32];

		int item = sscanf(g_nbiot_RxBuffer,"+CGNAPN: %d, 1,\"%s\"", &activeID, apnBuffer);
		if(item > 0)
			strcpy(nbiot_config.szAPN, apnBuffer);

		DebugPrintf("TestAPN:%s\r\n", nbiot_config.szAPN);
	}
}

void mqttNetworkConfigHandle()
{
	char cmd[64] = {0};
	int cmdRet;
	int retry_count = 0;

	char buf[64] = {0};
	AtCommand at;
	at_command_parament(&at, "CNCFG");

	sprintf(buf, "%d,%d,%s,\"%s\",\"%s\", 0", 0, 0, nbiot_config.szAPN, nbiot_config.szUserName, nbiot_config.szUserPassword);

	at_command_content(&at, buf);
	at.type = writeAT;

	mqttATSend(at, "OK");
}

void mqttSubHandle()
{
	int cmdRet = 0;
	char buf[64] = {0};
	AtCommand at;
	at_command_parament(&at, "SMSUB");
	sprintf(buf, "\"%s\",1", nb_mqtt.mqttTopic );
	at_command_content(&at, buf);
	at.type = writeAT;

	mqttATSend(at, "OK");	
}
static void mqttCheckOperator()
{
	AtCommand at;
	at_command_parament(&at, "CPSI");
	at.type = readAT;
	mqttATSend(at, "OK");
}

static void mqttCheckSoftware()
{
	AtCommand at;
	at_command_parament(&at, "SIMCOMATI"); // SIMCOMATI
	at.type = Default;
	mqttATSend(at, "OK");
}

void mqttSetService()
{
	int cmdRet = 0;
	char buf[64] = {0};
	AtCommand at;
	at_command_parament(&at, "CNMP");
	sprintf(buf, "%d", 13);
	at_command_content(&at, buf);
	at.type = writeAT;
	mqttATSend(at, "OK");

	HAL_Delay(20);

	memset(buf, 0, sizeof(buf));
	at_command_parament(&at, "CMNB");
	sprintf(buf, "%d", 2);
	at_command_content(&at, buf);
	at.type = writeAT;
	mqttATSend(at, "OK");
}

void mqttOpenHandle(void)
{
	// HAL_UART_DeInit(&huart2);
	// mqttSetService();
	// HAL_Delay(20);
	mqttNetworkConfigHandle();
	HAL_Delay(20);
	nbiot_init();
	// mqttCheckAPN();
	// HAL_Delay(100);
	// mqttCheckSoftware();
	HAL_Delay(100);
	// uartConfigure(gs_baudrate[uart_config.baudrate], uart_config.parity, uart_config.stopBits);
}

void mqttConnectHandle (void)
{
	int cmdRet = 0;
	int retry_count = 0;

	AtCommand at;
	at_command_parament(&at, "SMCONN");
	at.type = Default;

	at_command_factory(&at);
	do
	{
		cmdRet = Send_AT_Command(at.cmd, "OK", 7000, 7000);
		if(cmdRet == 1)
		{
			break;
		}
		else
		{
			DelayMs(1000);
			retry_count++;
		}
	} while(retry_count <= 7);

	if (retry_count > 7)
		mqtt_status = 0;
	else
		mqtt_status = 1;

}

void mqttDisconnectHandle ()
{
	int cmdRet = 0;

	AtCommand at;
	at_command_parament(&at, "SMDISC");
	at.type = Default;

  mqttATSend(at, "OK");
	// mqtt_status = 0;
}

void mqttCloseHandle ()
{
  int cmdRet = 0;

	AtCommand at;
	at_command_parament(&at, "CNACT");
	at_command_content(&at, "0,0");
	at.type = writeAT;

	mqttATSend(at, "OK");
	nbiot_status = CEL_DISCONNECTED;
}

uint8_t mqttCheckStatusHandle (void)
{
 // stub here
}
