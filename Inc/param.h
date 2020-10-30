#ifndef _PARAM_H_
#define _PARAM_H_

#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t unsigned char

enum usb_plug_flag
{
	PlugIn = 1,
	PlugOut
};

typedef enum  
{
	EM_BIT = 0,
	EM_SHORT,
	EM_INT,
	EM_STR,
	EM_MIX
}mb_trans_data_type_t;

typedef struct
{
	uint16_t base_addr;
	uint16_t num;
	uint8_t  part_tbl_n;
	uint8_t  type;
}Param_Config_t;

typedef struct
{
	uint16_t  addr;
	uint8_t		type;
	uint16_t  cnt;
}partition_element_t;

typedef struct
{
	uint8_t   part_n;
	uint8_t   cnt;
	partition_element_t* e;
}partition_datatype_t;

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

typedef struct {
	uint8_t		szServer[32];
	uint16_t	ulPort;
	// uint16_t  szRtuTimeout;
	uint8_t		szTopic[64];
	uint8_t		szClientID[32];
	uint32_t	ulInterval;
	uint8_t		szUserName[16];
	uint8_t		szPassWord[16];
	uint16_t  szRtuTimeout;
} mqtt_config_t;

typedef struct {
	uint8_t		szServer[32];
	uint16_t	ulPort;
}tcp_config_t;

typedef struct _mbus_md_device_cfg 
{
	uint8_t addr_and_mode[2];
	uint8_t secondary_addr[8];
	uint8_t readout_interval[4];
	uint8_t upload_bitmap[4];
	uint8_t timestamp[4];
	uint8_t resv[18];
} mbus_md_device_cfg;

typedef struct __sslResult__ {
	uint16_t sslMode;
	uint16_t rootCARes;
	uint16_t privCARes;
	uint16_t priKeyRes;
} ssl_result_t;

#pragma pack ()

#endif
