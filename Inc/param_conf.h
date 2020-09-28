#ifndef _PARAM_H_
#define _PARAM_H_

#include <stdint.h>

typedef struct
{
	uint16_t base_addr;
	uint16_t num;
	unsigned char part_tbl_n;
}Param_Config_t;
///////////////////////////////////////////////////////////////////////////////////////////
// eeprom configration
#pragma pack (1)
typedef struct
{
  uint16_t baudrate;
  uint16_t wordLength;
  uint16_t stopBits;
  uint16_t parity; 
	uint16_t modbus_addr;
}uart_config_t;

typedef struct
{
  uint16_t freq_band;
  uint16_t tx_power; 
	uint16_t sf;
	uint16_t bandwidth;	
	uint16_t cr;	
	uint16_t lro;
	uint16_t synword;
}lora_config_t;

typedef struct
{
  uint16_t parent;
  uint8_t child[32]; 
}net_config_t;
	
// aes key
typedef struct
{
  uint16_t enable;
  uint8_t key[16]; 
}aes_key_t;
	
typedef struct
{
  uint8_t name[16];
}mod_name_t;

typedef struct
{
  uint8_t data[16];
}serial_number_t;

typedef struct
{
  uint16_t resp_timeout;
}modbus_ctrl_t;

typedef struct
{
	uint16_t rssi;
	uint16_t snr;
}signal_quality_t;

typedef struct
{
	uint8_t data[16];
}software_ver;

typedef struct
{
	uint8_t username[16];
	uint8_t password[16];
}userinfo;	

typedef struct
{
//	int			bEnable;
	uint8_t		szAPN[32];
	uint8_t		szUserName[16];
	uint8_t		szUserPassword[16];
	uint16_t	ulIotMode;
}nbiot_config_t;

typedef struct
{
	uint8_t		szServer[32];
	uint16_t	ulPort;
	uint8_t		szTopic[64];
	uint8_t		szClientID[32];
	uint32_t	ulInterval;
//	uint8_t		szMsg[1024];
}mqtt_config_t;

typedef enum
{
	IOT_MQTT = 0,
	IOT_COAP
}iot_type_t;

typedef struct
{
	uint8_t		szURL[64];
	uint16_t	ulMethod;
	uint32_t	ulInterval;
	uint8_t		szURLGet[80];
	uint16_t	ulDtlsEnabled;
	uint8_t		szDtlsUserID[32];
	uint8_t		szDtlsPSK[64];
}coap_config_t;

#pragma pack ()

#endif