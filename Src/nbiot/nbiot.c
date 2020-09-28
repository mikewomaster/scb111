/* Includes ------------------------------------------------------------------*/
#include "nbiot.h"
#include "string.h"
#include "stdio.h"
#include "function.h"
#include "delay.h"
#include "param.h"
#include "at_command.h"
// #include "timer.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE 255
#define MAX_MBUS_DEV_NUM 1
#define MAX_MBUS_DEV_VALUE_NUM 20
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

Cel_Status_t nbiot_status = CEL_NOT_READY;
uint16_t mqtt_status = 0;
static char szIpaddr[16];
uint16_t irqNum_idle;
static int mqtt_idx = 1;

unsigned int mqtt_count_down_timer_on = 0; // on/off
uint32_t mqtt_count_down_timer = 0; // unit sec
int mqtt_count_down_timer_timeout = 0;

int nb_tcp_rcv_flag = 0;
uint8_t nb_tcp_rcv[256];

extern void MX_NB_Init(void);
//TimerEvent_t MqttCountDownTimer;

/* Private function prototypes -----------------------------------------------*/
/* External variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
char g_nbiot_RxBuffer[512];
uint32_t g_nbiot_rx_len;

extern nbiot_config_t nbiot_config;
extern mqtt_config_t mqtt_config;
/* Private user code ---------------------------------------------------------*/
void CLR_NBIOT_RxBuf(void)
{
	memset(g_nbiot_RxBuffer,'\0',sizeof(g_nbiot_RxBuffer));
	g_nbiot_rx_len = 0;
}

uint8_t Find(char *a)
{
	if(a == NULL)
		return 0;
	
	if(strstr(g_nbiot_RxBuffer, a) != NULL)
		return 1;
	else
		return 0;
}

void MqttCountDownIrq( void )
{
	if(mqtt_count_down_timer_on)
		{
		if(mqtt_count_down_timer)
			{
			mqtt_count_down_timer--;
			}
		else
			{
			mqtt_count_down_timer_timeout = 1;
			mqtt_count_down_timer_on = 0;
			}
		}
	// TimerStart( &MqttCountDownTimer );
}

/* Exported user code ---------------------------------------------------------*/
int detachFlag = 0;
static int rebootCount = 0;

// #define ATCOMMANDCHECK

int Send_AT_Command(char *cmd, char *result, uint32_t wait_time, uint32_t wait_time2)
{
#ifdef ATCOMMANDCHECK
	DebugPrintf(cmd);
	DebugPrintf("\r\n");
#else	
	int ret = 0;
	uint8_t cmdRet = 0;
	uint8_t at_cmd[CMD_LENGTH]={'\0'};
	uint32_t at_cmd_length = 0;
	uint32_t u32StartTime = 0;
	uint8_t SendCmd = 0;
	int index1 = 0,index2 = 0;
	char *pch1 = NULL,*pch2 = NULL;
	int i = 0;
	int length = 0;
	
	if(cmd == NULL || result == NULL)
		return 0;

	if(strlen(cmd) == 0 || strlen(cmd) > CMD_LENGTH)
		return 0;
		
	irqNum_idle=0;
	at_cmd_length = strlen(cmd);
	memcpy(at_cmd, cmd, strlen(cmd));
	if(memcmp(at_cmd,"{",1) != 0)
		{
		if(cmd[(at_cmd_length-1)] != '\r')
			{
			if(at_cmd_length < CMD_LENGTH)
				{
				at_cmd_length = at_cmd_length+1;
				at_cmd[at_cmd_length-1] = '\r';
				}
			else
				return -2;
			}
		}
	CLR_NBIOT_RxBuf();
	ret = 0;
	
	int times = 0;
	do
	 {			
			while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
			if(SendCmd == 0 || ((HAL_GetTick()-u32StartTime) > wait_time2))
			{
				u32StartTime = HAL_GetTick();	
				cmdRet = HAL_UART_Transmit(&huart1, at_cmd, at_cmd_length, wait_time);

				if(cmdRet != HAL_OK)
					ret = -1;
				else
					SendCmd = 1;
			}
		
			if(strlen(result) > 0 && Find(result)) {
				ret = 1;
				rebootCount = 0;
			}
			else if(strlen(result) == 0 && irqNum_idle > 1) {
				ret = 1;
			}
			else if(Find("ERROR")) {
				ret = -1;
				++ rebootCount;
				if (rebootCount > 4) {
					rebootCount = 0;
					BoardResetMcu();
				}
			}

			DelayMs(20);
	  } while(ret == 0 && times++ < 200);

		DebugPrintf("value = %s\r\n", g_nbiot_RxBuffer);
		
		// CHECK
		if (times == 200)
			BoardResetMcu();
		else
      nb_tcp_rcv_flag = 1;

	/*	
	  if (times == 200 || (strstr(g_nbiot_RxBuffer, "PDP") && strstr(g_nbiot_RxBuffer,"DEACTIVE")))
			BoardResetMcu();
	*/
		
	 if (ret == 1 && strlen(result) > 0)
	 {
		 pch1 = strstr(g_nbiot_RxBuffer, cmd);		
		 if(pch1 != NULL)
		 {
			 index1 = (pch1 - g_nbiot_RxBuffer) + strlen(cmd);
			 do
			 {
			   if(index1 >= BUFFER_SIZE)
			  	 break;
			  	
			   if(g_nbiot_RxBuffer[index1] == '\r' || g_nbiot_RxBuffer[index1] == '\n')
			  	 index1++;
			   else
			  	 break;
			  } while(1);
				
			 pch2 = strstr(&g_nbiot_RxBuffer[index1], result);
			 if(pch2 != NULL)
			 {
				 index2 = pch2 - &g_nbiot_RxBuffer[index1];
				 length = index2;
				 for(i = (index1+index2-1); i > 0; i--)
				 {
					 if(g_nbiot_RxBuffer[i] == '\r' || g_nbiot_RxBuffer[i] == '\n')
						 length--;
					 else
						 break;
				 }

				 if(length > 0)
				 {
					 memcpy(g_nbiot_RxBuffer, &g_nbiot_RxBuffer[index1], length);
					 g_nbiot_rx_len = length;
					 memset(&g_nbiot_RxBuffer[g_nbiot_rx_len], '\0', (BUFFER_SIZE-g_nbiot_rx_len));
				 }
			 }
		 }
	 }

	return ret;
#endif
}

int Send_AT_Command_2(char *cmd, char *result, uint32_t wait_time, uint32_t wait_time2, uint32_t waitResponseTime, char* msg)
{
	int at_cmd_length = strlen(cmd);
	char at_cmd[64];
	memcpy(at_cmd, cmd, strlen(cmd));
	if(memcmp(at_cmd,"{",1) != 0)
		{
		if(cmd[(at_cmd_length-1)] != '\r')
			{
			if(at_cmd_length < CMD_LENGTH)
				{
				at_cmd_length = at_cmd_length+1;
				at_cmd[at_cmd_length-1] = '\r';
				}
			else
				return -2;
			}
		}
	
		CLR_NBIOT_RxBuf();
		HAL_UART_Transmit(&huart1, at_cmd, at_cmd_length, wait_time);

		DebugPrintf(g_nbiot_RxBuffer);
		CLR_NBIOT_RxBuf();
		HAL_UART_Transmit(&huart1, msg, strlen(msg), wait_time);
		HAL_Delay(20000);
		DebugPrintf("\r\n");
		DebugPrintf("\r\n");
		DebugPrintf(g_nbiot_RxBuffer);
		CLR_NBIOT_RxBuf();
	#if 0
#ifdef ATCOMMANDCHECK
	DebugPrintf(cmd);
	DebugPrintf("\r\n");
#else	
	int ret = 0;
	uint8_t cmdRet = 0;
	uint8_t at_cmd[CMD_LENGTH]={'\0'};
	uint32_t at_cmd_length = 0;
	uint32_t u32StartTime = 0;
	uint8_t SendCmd = 0;
	int index1 = 0,index2 = 0;
	char *pch1 = NULL,*pch2 = NULL;
	int i = 0;
	int length = 0;	
	irqNum_idle=0;
	ret = 0;
	int times = 0;
	
	CLR_NBIOT_RxBuf();
	
	do
	 {			
			while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
			if(SendCmd == 0 || ((HAL_GetTick()-u32StartTime) > wait_time2))
			{
				u32StartTime = HAL_GetTick();	
				cmdRet = HAL_UART_Transmit(&huart1, at_cmd, at_cmd_length, wait_time);

				if(cmdRet != HAL_OK)
					ret = -1;
				else
					SendCmd = 1;
			}
		
			if(strlen(result) > 0 && Find(result)) {
				ret = 1;
				rebootCount = 0;
			}
			else if(strlen(result) == 0 && irqNum_idle > 1) {
				ret = 1;
			}
			else if(Find("ERROR")) {
				ret = -1;
				++ rebootCount;
				if (rebootCount > 4) {
					rebootCount = 0;
					BoardResetMcu();
				}
			}

			DelayMs(20);
	  } while(ret == 0 && times++ < 200);

		DebugPrintf("value = %s\r\n", g_nbiot_RxBuffer);

	

	return ret;
#endif
#endif
}

void nbiot_poweroff_on()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(2*1000);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}
	
int nbiot_init(void)
{
	char cmd[CMD_LENGTH];
	int ret = -1;
	int retry_count = 0;
	uint8_t cmdRet = 0;
	uint8_t PS_attach = 0;
	
	do
	{
		memset(cmd,'\0',sizeof(cmd));
		memcpy(cmd,"AT\r",3);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
		if(cmdRet == 1)
		{
			nbiot_status = CEL_MODULE_READY;
			ret = 0;
			break;
		}
		else
		{
			DelayMs(1000);
			retry_count++;
		}
	}while(retry_count <= 10);
	
	// nbiot_checkSIMStatus();
	// nbiot_checkNetStatus();
	retry_count = 0;
	do {
		 DelayMs(2000);
		 PS_attach = nbiot_checkAttachStatus();
		 retry_count ++;
	} while( PS_attach!=1 && retry_count<1000);
	
	if (retry_count == 1000)
		BoardResetMcu();

	nbiot_checkNetworkInfo(); 
	nbiot_activeNetwork();
	nbiot_checkIsActive();
}

void nbiot_tcp_cnt(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	char szBuf[20];	
	int openValue = 1, openTimes = 0;
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CASSLCFG=0,\"SSL\",0");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+CASSLCFG: %s",szBuf);
		if(item > 0)
		{
		}
	}

	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd, "AT+CAOPEN=0,0,\"TCP\",\"%s\",%d", mqtt_config.szServer, mqtt_config.ulPort);

	do {
		cmdRet = Send_AT_Command(cmd, "OK", CMD_TIMEOUT, 10000);
		if(g_nbiot_rx_len > 0) {
			sscanf(g_nbiot_RxBuffer, "+CAOPEN: 0,%d", &openValue);
		}
		
		openTimes ++;
		HAL_Delay(2000);
	} while(openValue && openTimes < 6);
	
	return;
}

void nbiot_checkNetStatus(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	char szBuf[20];

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CSQ");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+CSQ: %s",szBuf);
		if(item > 0)
		{
		}
	}
	return;
}

int8_t nbiot_checkAttachStatus(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	char szBuf[20];
	uint8_t ret;
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CGATT?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+CGATT: %d",&ret);
		if(item > 0)
		{
			return ret;
		}
	}
	return -1;
}

void nbiot_activeNetwork(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0, ret = 0;
	char szBuf[20];

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CNACT=0,1");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+APP PDP: %s",szBuf);
		if(item > 0)
		{
		}
	}
	return;
}

void nbiot_checkIsActive(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CNACT?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szIpaddr,'\0',16);
		int activeID = 0;
		item = sscanf(g_nbiot_RxBuffer,"+CNACT: %d,1,%s", &activeID, szIpaddr);
		
		if(item > 0)
		{
			if (strlen(szIpaddr))
				nbiot_status = CEL_CONNECTED;
		}

	}
	return;
}

void nbiot_checkNetworkInfo(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	char szBuf[20];

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+COPS?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+COPS: %s",szBuf);
		if(item > 0)
		{
		}
	}
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CGNAPN");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+CGNAPN: %s",szBuf);
		if(item > 0)
		{
		}
	}
	return;
}

void nbiot_setStatus(Cel_Status_t status)
{
	nbiot_status = status;
}

void nbiot_checkSIMStatus(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	char szBuf[20];

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CPIN?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT, 2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		memset(szBuf,'\0',sizeof(szBuf));
		item = sscanf(g_nbiot_RxBuffer,"+CPIN: %s",szBuf);
		if(item > 0)
		{
			if(strcmp(szBuf,"READY") == 0)
				nbiot_status = CEL_SIM_READY;
			else if(strcmp(szBuf,"SIM PIN") == 0)
				nbiot_status = CEL_PIN_REQUEST;
			else
				nbiot_status = CEL_SIM_NOT_READY;
		}
	}		
}

uint16_t nbiot_getStatus(void)
{
	return 	nbiot_status;
}

int nbiot_reconnect(void)
{
	char cmd[CMD_LENGTH];
	int ret = -1;
	int retry_count = 0;
	uint8_t cmdRet = 0;

	do
	{
		memset(cmd,'\0',sizeof(cmd));
		memcpy(cmd,"AT\r",3);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
		if(cmdRet == 1)
		{
			nbiot_status = CEL_MODULE_READY;
			ret = 0;
			break;
		}
		else
		{
			DelayMs(1000);
			retry_count++;
		}
	}while(retry_count <= 10);

	if(nbiot_status == CEL_MODULE_READY)
		nbiot_checkSIMStatus();
		
	return ret;
}

void nbiot_disconnect(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
//	uint8_t cid = 1;
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CGACT=0");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+QIDEACT");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	
	nbiot_status = CEL_DISCONNECTED;
}

void nbiot_updateSettings(void)
{
	char cmd[CMD_LENGTH];
	int cmdRet = 0;
	uint8_t cid = 1;
	int item = 0;
	char szBuf[20];
	uint8_t cid_temp = 0;
	uint8_t PS_attach = 0;
	
	if(nbiot_status < CEL_SIM_READY)
		return;
	
	DebugPrintf("nbiot_status:%d. \n", nbiot_status);
	
	if(nbiot_status == CEL_CONNECTED)
		nbiot_disconnect();
	
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CSQ");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{

	}		
	
	// DelayMs(8000);	
	do
	{
		DelayMs(2000);
		memset(cmd,'\0',sizeof(cmd));
		strcpy(cmd,"AT+CGREG?");
		cmdRet = Send_AT_Command(cmd,"OK",2000,5000);
		if(cmdRet == 1 && g_nbiot_rx_len > 0)
		{
			item = sscanf(g_nbiot_RxBuffer,"+CGREG: 0,%d",&PS_attach);
		}
	}while( PS_attach!=1 );

	// query network service operator?
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+COPS?");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	
	// pdp network status
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CGCONTRDP");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		item = sscanf(g_nbiot_RxBuffer,"+CGPADDR: %d,\"%[^\"]\"",&cid_temp,szIpaddr);
		if(item > 0)
		{
			if(strlen(szIpaddr) != 0)
				nbiot_status = CEL_CONNECTED;
		}
	}	
	
	memset(szIpaddr, '\0', sizeof(szIpaddr));
	// ip address
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CGPADDR=%d",cid);
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		item = sscanf(g_nbiot_RxBuffer,"+CGPADDR: %d,\"%[^\"]\"",&cid_temp,szIpaddr);
		if(item > 0)
		{
			if(strlen(szIpaddr) != 0)
				nbiot_status = CEL_CONNECTED;
		}
	}
	
	DebugPrintf("nbiot_status:%d. PS_attach:%d.\n", nbiot_status, PS_attach);
	

	if(PS_attach == 1)
	{
		// TCP/UDP
	}
	
}

int nbiot_read_status(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{

	uint8_t *p = (uint8_t *)data;
	int ret = -1;
	uint16_t value = 0;
	uint16_t status = nbiot_getStatus();

	switch(offset)
	{
		case 0:
		{
			value = status;
			memcpy(p, (uint16_t *)&value, sizeof(uint16_t));
		}
			break;
		case 2:
		{
			ret = get_value_str(szIpaddr, sizeof(szIpaddr), p);
		}
			break;
	}
	return ret;
}

void mqtt_disconnect(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CZMQTTDISCONN=%d",mqtt_idx);
	cmdRet = Send_AT_Command(cmd,"",2000,2000);
}

void nbiot_sendMQTT(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	int idx = 0,status = 0;
	int i = 0,j = 0;
	char szMsg[1024]={'\0'};
	int ulRetryCount = 0;
	unsigned int bitmap;
	char str[64];

	if(nbiot_status != CEL_CONNECTED)
		return;

Again:
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CZMQTTSTATE=%d",mqtt_idx);
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len != 0)
		{
		item = sscanf(g_nbiot_RxBuffer,"+CZMQTTSTATE: %d,%d",&idx,&status);
		if(status == 1)
			{
			for(i = 0; i < MAX_MBUS_DEV_NUM; i++)
				{
				memset(szMsg, '\0',sizeof(szMsg));
					//if(get_mbus_dev_addr(i) != 0xff)
					{
					// dev info
					//snprintf(szMsg,sizeof(szMsg),"{");
					//get_mbus_dev_addr_str(i, str, sizeof(str));
					//snprintf(szMsg,sizeof(szMsg),"%s\"addr\":\"%s\",", szMsg, str);
					//get_mbus_dev_id_str(i, str, sizeof(str));
					//snprintf(szMsg,sizeof(szMsg),"%s\"serial\":\"%s\",", szMsg, str);
					//get_mbus_dev_manufacturer_str(i, str, sizeof(str));
					//snprintf(szMsg,sizeof(szMsg),"%s\"manufacturer\":\"%s\",", szMsg, str);
					//get_mbus_dev_version_str(i, str, sizeof(str));
					//snprintf(szMsg,sizeof(szMsg),"%s\"version\":\"%s\",", szMsg, str);
					//get_mbus_dev_medium_str(i, str, sizeof(str));
					//snprintf(szMsg,sizeof(szMsg),"%s\"medium\":\"%s\"", szMsg, str);

					// dev value
					//bitmap = get_mbus_dev_status_bitmap(i);
					if(bitmap)
						{
						for(j=0;j<MAX_MBUS_DEV_VALUE_NUM;j++)
							{
							memset(str,'\0',sizeof(str));
							if(bitmap & (0x1 << j))
								{
								snprintf(szMsg,sizeof(szMsg),"%s,", szMsg);
						//		get_mbus_value_mqtt_str(i, j, str, sizeof(str));
								snprintf(szMsg,sizeof(szMsg),"%s%s", szMsg, str);
								}
							}
						}
		
					snprintf(szMsg,sizeof(szMsg),"%s}",szMsg);

#if 1
					if(strlen(szMsg) > 0)
						{
						memset(cmd,'\0',sizeof(cmd));
						sprintf(cmd,"AT+CZMQTTPUB=%d,\"%s\",0,%d",mqtt_idx,mqtt_config.szTopic,strlen(szMsg));
						cmdRet = Send_AT_Command(cmd,">",2000,2000);
						cmdRet = Send_AT_Command(szMsg,"OK",5000,5000);
						}
#else
					memset(cmd,'\0',sizeof(cmd));
					sprintf(cmd,"AT+CZMQTTPUB=%d,\"%s\",0,\"%s\",%d",mqtt_idx,mqtt_config.szTopic,szMsg);
					cmdRet = Send_AT_Command(cmd,"",2000);
#endif
					}
				}

			}
		else
			{
			if(ulRetryCount < 3)
				{
				DelayMs(2000);
				ulRetryCount++;
				goto Again;
				}
			}
		}
		mqtt_disconnect();
}

void nbiot_updateRTC(void)
{
	int item = 0;
	int rtc_year = 0,rtc_month = 0,rtc_day = 0;
	int rtc_hour = 0,rtc_minute = 0,rtc_sec = 0,rtc_tz=0;
	int ulRetryCount = 0;
	unsigned int timestamp;
	struct tm stm;
	char cmd[32];
	char buf[64] = {0x00};
	uint8_t cmdRet = 0;
	char zone[10];

	if(nbiot_status != CEL_CONNECTED)
		return;

Again:
	memset(cmd,'\0',sizeof(cmd));
	memset(zone, 0, sizeof(zone));
	strcpy(cmd,"AT+CCLK?");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		item = sscanf(g_nbiot_RxBuffer, "+CCLK: \"%[^\"]\"", buf);
		if(strlen(buf) > 0)
		{
			item = sscanf(buf,"%d/%d/%d,%d:%d:%d%s",
												&rtc_year, &rtc_month, &rtc_day, &rtc_hour, &rtc_minute, &rtc_sec, zone);

			if (rtc_year == 80)
			{
				return ;
			}
			
			if(item == 7 || item == 6)
			{
				stm.tm_year = (rtc_year + 2000) - 1900;
				stm.tm_mon = rtc_month - 1;
				stm.tm_mday = rtc_day;
				if (item == 7)
				{
					stm.tm_hour = rtc_hour + atoi(zone)/4;
				}
				else
				{
					stm.tm_hour = rtc_hour;
				}
				stm.tm_min = rtc_minute;
				stm.tm_sec = rtc_sec;
				timestamp = mktime(&stm);
				
				// set to RTC
				// timestamp_to_time(timestamp);
			}
		}	
	}
	else
	{
		if(ulRetryCount < 3)
		{
			DelayMs(1000);
			ulRetryCount++;
			goto Again;
		}
	}
}

void nbiot_updateMQTT(void)
{
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	int PS_attach = 0;
	int idx,status = 0;
	int i=0,j=0;
	
	if(nbiot_status != CEL_CONNECTED)
		return;
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CGATT?");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		item = sscanf(g_nbiot_RxBuffer,"+CGATT: %d",&PS_attach);
	}

	if(PS_attach == 1)
	{
		if(strlen(mqtt_config.szServer) == 0)
			return;

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTSTATE=%d",mqtt_idx);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
		if(cmdRet == 1 && g_nbiot_rx_len != 0)
		{
			item = sscanf(g_nbiot_RxBuffer,"+CZMQTTSTATE: %d,%d",&idx,&status);
			if(status == 1)
				mqtt_disconnect();
		}

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTSERV=%d,\"%s\"",mqtt_idx,mqtt_config.szServer);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTPORT=%d,%d",mqtt_idx,mqtt_config.ulPort);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTID=%d,\"%s\"",mqtt_idx,mqtt_config.szClientID);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTPARA=%d,\"RECONNECT\",1",mqtt_idx);
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);

		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CZMQTTCONN=%d",mqtt_idx);
		cmdRet = Send_AT_Command(cmd,"",2000,2000);

		nbiot_sendMQTT();

		mqtt_count_down_timer = (mqtt_config.ulInterval);
/*
		if(mqtt_count_down_timer <= 0)
			TimerSetValue( &MqttCountDownTimer, 100000 ); // 100 second
		else
			TimerSetValue( &MqttCountDownTimer, mqtt_count_down_timer );
*/
		mqtt_count_down_timer_timeout = 0;
		mqtt_count_down_timer_on = 1;
//		TimerStart( &MqttCountDownTimer );
	}
}

int mqtt_read_status(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t *p = (uint8_t *)data;
	int ret = -1;
	uint16_t value = 0;
	char cmd[CMD_LENGTH];
	uint8_t cmdRet = 0;
	int item = 0;
	int idx = -1;

	switch(offset)
	{
		case 0:
		{
			/*
				memset(cmd,'\0',sizeof(cmd));
				sprintf(cmd,"AT+CZMQTTSTATE=%d",mqtt_idx);
				cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
				if(cmdRet == 1 && g_nbiot_rx_len != 0)
				{
					item = sscanf(g_nbiot_RxBuffer,"+CZMQTTSTATE: %d,%d",&idx,&value);
				}
			*/
			value = mqtt_status;
			memcpy(p,(uint16_t *)&value, sizeof(uint16_t));
		}
			break;
		case 2:
		{
			ret = get_value_str(szIpaddr, sizeof(szIpaddr), p);
		}
			break;
	}

	return ret;
}

void nbiot_process(void)
{
	if(mqtt_count_down_timer_timeout == 1)
	{
		mqtt_count_down_timer_timeout = 0;
		//nbiot_reconnect();
		//nbiot_updateSettings();
		nbiot_updateRTC();
		nbiot_sendMQTT();
		//nbiot_disconnect();
		mqtt_count_down_timer = (mqtt_config.ulInterval);
		mqtt_count_down_timer_on = 1;
	}
}

void mqtt_count_down_proc(void)
{
	if(mqtt_count_down_timer_on == 1)
	{
		if(mqtt_count_down_timer > 0)
		{
			mqtt_count_down_timer--;
		}
		else
		{
			mqtt_count_down_timer_timeout = 1;
			mqtt_count_down_timer_on = 0;
		}
	}
}

void nbiot_sleep()
{
	char cmd[CMD_LENGTH];
	char szMsg[200]={'\0'};
	uint16_t szlen;
	uint8_t cmdRet = 0;
	int item = 0;
	int PS_attach = 0;
	int idx,status = 0;
	int i=0,j=0;
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CPSMSTATUS=1");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}	
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+IPR=9600");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}		
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CEREG=4");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}		
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CEREG?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}		

	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CPSMS=1,,,\"01011111\",\"00000001\"");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}	
	
	memset(cmd,'\0',sizeof(cmd));
	strcpy(cmd,"AT+CEREG?");
	cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	if(cmdRet == 1 && g_nbiot_rx_len > 0)
	{
		
	}		
	RtcEnterLowPowerStopMode( );
}

