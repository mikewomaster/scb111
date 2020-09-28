#ifndef __NBIOT_H
#define __NBIOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <time.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define CMD_LENGTH	1024
#define CMD_TIMEOUT	2000

typedef enum 
{
	CEL_ERROR = -1,
	CEL_NOT_READY = 0,
	CEL_PORT_READY,
	CEL_MODULE_READY,
	CEL_SIM_NOT_READY,
	CEL_PIN_REQUEST,
	CEL_SIM_READY,
	CEL_DISCONNECTED,
	CEL_CONNECTED
} Cel_Status_t;

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
int Send_AT_Command(char *cmd, char *result, uint32_t wait_time, uint32_t wait_time2);
int Send_AT_Command_rcv(char *cmd, char *result, uint32_t wait_time, uint32_t wait_time2);
int nbiot_init(void);
uint16_t nbiot_getStatus(void);
void nbiot_setStatus(Cel_Status_t status);
void nbiot_checkSIMStatus(void);
void nbiot_disconnect(void);
void nbiot_checkNetStatus(void);
void nbiot_tcp_cnt(void);
int8_t nbiot_checkAttachStatus(void);	
void nbiot_checkNetworkInfo(void);	
void nbiot_activeNetwork(void);	
void nbiot_checkIsActive(void);

void nbiot_updateSettings(void);
int nbiot_write_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
int nbiot_read_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
int nbiot_read_status(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
void nbiot_sleep(void);

void nbiot_process(void);
void nbiot_poweroff_on();
void mqtt_disconnect(void);
void nbiot_updateMQTT(void);
void nbiot_sendMQTT(void);
void nbiot_sendMQTT2(void);
void nbiot_updateRTC(void);

void mqtt_count_down_proc(void);
int mqtt_write_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
int mqtt_read_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
int mqtt_read_status(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);

extern void BoardResetMcu(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif
