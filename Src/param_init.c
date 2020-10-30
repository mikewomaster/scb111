#include "param_init.h"
#include "param.h"
#include "stm32l072xx.h"
#include "main.h"
#include "modbus_rtu.h"

#define STM_EPROM_ADDR_BASE									((uint32_t)DATA_EEPROM_BASE)
#define STM_EPROM_ADDR_END									((uint32_t)DATA_EEPROM_BASE+0x1800-1)

#define UR_EPROM_ADDR_BASE									STM_EPROM_ADDR_BASE
#define LORA_EPROM_ADDR_BASE								UR_EPROM_ADDR_BASE+sizeof(uart_config_t)+6
#define NET_EPROM_ADDR_BASE									LORA_EPROM_ADDR_BASE+sizeof(lora_config_t)+6
#define AES_EPROM_ADDR_BASE									NET_EPROM_ADDR_BASE+sizeof(net_config_t)+6
#define MODELNAME_EPROM_ADDR_BASE						AES_EPROM_ADDR_BASE+sizeof(aes_key_t)+6
#define SN_EPROM_ADDR_BASE									MODELNAME_EPROM_ADDR_BASE+sizeof(mod_name_t)+6
#define RESP_TIMEOUT_EPROM_ADDR_BASE				SN_EPROM_ADDR_BASE+sizeof(serial_number_t)+6
#define SW_VERSION_ADDR_BASE								RESP_TIMEOUT_EPROM_ADDR_BASE+sizeof(modbus_ctrl_t)+6
#define USER_INFO_ADDR_BASE									SW_VERSION_ADDR_BASE+sizeof(software_ver )+6

#define MODBUS_UNIT_ADDR_BASE								USER_INFO_ADDR_BASE + sizeof(userinfo) + 6

/*
	#define MBUS_EPROM_ADDR_BASE								MODBUS_UNIT_ADDR_BASE + sizeof(rtuModbus)+6
	#define NBIOT_EPROM_ADDR_BASE								MBUS_EPROM_ADDR_BASE+sizeof(mbus_md_device_cfg)+6
	#define MQTT_EPROM_ADDR_BASE								NBIOT_EPROM_ADDR_BASE+sizeof(nbiot_config_t)+6
*/

#define NBIOT_EPROM_ADDR_BASE								MODBUS_UNIT_ADDR_BASE+sizeof(rtuModbus)+6
#define MQTT_EPROM_ADDR_BASE								NBIOT_EPROM_ADDR_BASE+sizeof(nbiot_config_t)+6
#define SSL_EPROM_ADDR_BASE									MQTT_EPROM_ADDR_BASE+sizeof(mqtt_config_t)+6
#define TCP_EPROM_ADDR_BASE									SSL_EPROM_ADDR_BASE+sizeof(tcp_config_t)+6

#define COAP_EPROM_ADDR_BASE								MQTT_EPROM_ADDR_BASE+sizeof(mqtt_config_t)+6

#if (SUPPORT_DLMS == 1)
#define DLMS_EPROM_ADDR_BASE								COAP_EPROM_ADDR_BASE+sizeof(coap_config_t)+6
#endif

struct partition_map_ {
    uint16_t part_size;
    uint32_t part_addr;   
};

#pragma pack (1)
typedef struct {
    uint16_t magic;
    uint16_t length;
    uint16_t crc;
}table_header_t;
#pragma pack ()

Param_Config_t param_config_tbl[]={{1, 10, PARTITION_UART}, // uart
																		{22, 14, PARTITION_LOAR}, // lora transparent
																		{101, 34, PARTITION_NET}, // net struct
																		{118, 18, PARTITION_AES}, // aes
																		{134, 16, PARTITION_MODEL_NAME}, // model name
																		{142, 16, PARTITION_SN}, // sn
																		{150, 2, PARTITION_MODBUS_CTRL}, // modbus_ctrl
																		{171, 16, PARTITION_SW_VERSION}, // sw version
																		{181, 32, PARTITION_USER_INFO}, // user name & password
																		{179, 2, PARTITION_MEM_RESET}, // reset default
																		{161, 4, PARTITION_MEM_SIGNAL}, // rssi, snr
																		{200, 32, PARTITION_MEM_CHECKUSER}
																	};
#define PARAM_TBL_N sizeof(param_config_tbl)/sizeof(param_config_tbl[0])

static const struct partition_map_ partition_map[] = 
{
	6 + sizeof(uart_config_t),	UR_EPROM_ADDR_BASE,								// PARTITION_0,  uart
	6 + sizeof(lora_config_t),	LORA_EPROM_ADDR_BASE,							// PARTITION_1,  lora transparent
	6 + sizeof(net_config_t),		NET_EPROM_ADDR_BASE,							// PARTITION_2,  net struct
	6 + sizeof(aes_key_t),		AES_EPROM_ADDR_BASE,								// PARTITION_3,  aes
	6 + sizeof(mod_name_t),		MODELNAME_EPROM_ADDR_BASE,					// PARTITION_4,  model name
	6 + sizeof(serial_number_t),	SN_EPROM_ADDR_BASE,							// PARTITION_5,  sn
	6 + sizeof(modbus_ctrl_t),	RESP_TIMEOUT_EPROM_ADDR_BASE,			// PARTITION_6,  modbus_ctrl
	6 + sizeof(software_ver), 	SW_VERSION_ADDR_BASE,							// PARTITION_7,  software_version
	6 + sizeof(userinfo), 			USER_INFO_ADDR_BASE,							// PARTITION_8,  userinfo
	6 + sizeof(rtuModbus),				MODBUS_UNIT_ADDR_BASE,					// PARTITION_9,  mbus
	6 + sizeof(nbiot_config_t),	NBIOT_EPROM_ADDR_BASE,						// PARTITION_10, nbiot
	6 + sizeof(mqtt_config_t),	MQTT_EPROM_ADDR_BASE,							// PARTITION_11, mqtt
	6 + sizeof(ssl_result_t), SSL_EPROM_ADDR_BASE,								// PARTITION_12, ssl
	6 + sizeof(tcp_config_t), TCP_EPROM_ADDR_BASE									// PARTITION_13, tcp
	// 6 + sizeof(mbus_md_device_cfg),	MBUS_EPROM_ADDR_BASE,
#if (SUPPORT_DLMS == 1)
	6+sizeof(coap_config_t),	COAP_EPROM_ADDR_BASE,			// PARTITION_12, coap configuration
	6+sizeof(dlms_config_t),	DLMS_EPROM_ADDR_BASE			// PARTITION_13, coap configuration
#endif
};

uint16_t crc_calc(uint16_t crc, uint8_t *start, uint8_t *end)
{
  uint8_t  *data;

  for (data = start; data < end; data++)
  {
    crc  = (crc >> 8) | (crc << 8);
    crc ^= *data;
    crc ^= (crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0xff) << 5;
  }
  return crc;
}

uint8_t StmL0xxEpromUnlock(void)
{
	return (uint8_t)HAL_FLASHEx_DATAEEPROM_Unlock();
}

uint8_t StmL0xxEpromLock(void)
{
	return (uint8_t)HAL_FLASHEx_DATAEEPROM_Lock();
}

uint8_t StmL0xxEpromWordErase(uint32_t addr)
{
	if( (STM_EPROM_ADDR_BASE>addr) || (STM_EPROM_ADDR_END<addr))
	{
		return 0;
	}
	return HAL_FLASHEx_DATAEEPROM_Erase(addr);
}

uint8_t StmL0xxEpromSingleWrite(uint8_t type, uint32_t addr, uint32_t data)
{
	uint8_t ret = 0;
	uint8_t cnt = 3;
	
	while( cnt != 0 )
	{	
		if( (STM_EPROM_ADDR_BASE>addr) || (STM_EPROM_ADDR_END<addr))
		{
			return 0;
		}
		StmL0xxEpromUnlock();
		ret = (uint8_t)HAL_FLASHEx_DATAEEPROM_Program((uint32_t)type, addr, data);
		StmL0xxEpromLock();
		
		if( ret !=0 )
			cnt --;
		else 
			return 0;
	}	
	return ret;
}

void StmL0xxEpromSingleByteRead(uint32_t addr, uint8_t *data)
{
	if( (STM_EPROM_ADDR_BASE>addr) || (STM_EPROM_ADDR_END<addr))
	{
		return ;
	}
	StmL0xxEpromUnlock();
	*data = *(volatile uint8_t*)addr;
	StmL0xxEpromLock();
}

void StmL0xxEpromSingleHalfWordRead(uint32_t addr, uint16_t *data)
{
	if( (STM_EPROM_ADDR_BASE>addr) || (STM_EPROM_ADDR_END<addr))
	{
		return ;
	}
	StmL0xxEpromUnlock();
	*data = *(volatile uint16_t*)addr;
	StmL0xxEpromLock();
}

void StmL0xxEpromSingleWordRead(uint32_t addr, uint32_t *data)
{
	if( (STM_EPROM_ADDR_BASE>addr) || (STM_EPROM_ADDR_END<addr))
	{
		return ;
	}
	StmL0xxEpromUnlock();
	*data = *(volatile uint32_t*)addr;
	StmL0xxEpromLock();
}

uint32_t get_partition(partition_index index)
{
    uint32_t addr = partition_map[index].part_addr;
    return addr;	
}

int read_partition_init(partition_index partition, char *out, uint16_t out_len)
{
  uint16_t i, crc;
  table_header_t table;
  uint32_t base_addr;

	base_addr = get_partition(partition);
	
	for(i=0; i < out_len / 2; i++)
	{
		StmL0xxEpromSingleHalfWordRead(base_addr+6+i*2, (uint16_t*)(out+i*2));
	}

	return 0;
}

int read_partition(partition_index partition, char *out, uint16_t out_len)
{
  uint16_t i, crc;
  table_header_t table;
  uint32_t base_addr;

	base_addr = get_partition(partition);
  StmL0xxEpromSingleHalfWordRead(base_addr, (uint16_t*)&(table.magic));
	StmL0xxEpromSingleHalfWordRead(base_addr+2, (uint16_t*)&(table.length));
	StmL0xxEpromSingleHalfWordRead(base_addr+4, (uint16_t*)&(table.crc));

	if (table.magic != PARTITION_MAGIC_WORD) {
			return -1;
	}

	if (table.length != out_len) {
			return -1;
	}
	
	for(i=0; i<table.length/2; i++)
	{
		StmL0xxEpromSingleHalfWordRead(base_addr+6+i*2, (uint16_t*)(out+i*2));
	}


	crc = crc_calc(0, (unsigned char*)out, (unsigned char*)(out + table.length));
	if (crc != table.crc) {
			return -1;
	}

	return 0;
}

int write_partition(partition_index partition, char *in, uint16_t in_len)
{
	uint16_t i;
  uint16_t crc;
  table_header_t table;
  uint32_t base_addr;
  
  base_addr = get_partition(partition);
  
  table.magic = PARTITION_MAGIC_WORD;
  table.length = in_len;
  crc = crc_calc(0, (unsigned char*)in, (unsigned char*)(in + in_len));
  table.crc = crc;
	
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr, table.magic);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr, table.magic);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr + 2, table.length);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr + 4, table.crc);

  for(i = 0; i < in_len/2; i++)
	{
		if( 0 != StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, (uint32_t)(base_addr+6+i*2), *(uint16_t*)(in+i*2)))
		{
			return -1;
		}
	}  	      
  return 0;
}

// for more sequences
int read_partition_modbus_rtu(partition_index partition, char *out, uint16_t out_len, int offset)
{
  uint16_t i = 0;
  table_header_t table;
  uint32_t base_addr;

	base_addr = get_partition(partition);
	base_addr = base_addr + offset;

	for(i=0; i<70; i++)	
		StmL0xxEpromSingleHalfWordRead(base_addr+6+i*2, (uint16_t*)(out+i*2));

	return 0;
}

int write_partition_more_seq(partition_index partition, char *in, uint16_t in_len, int seq)
{
	uint16_t i;
  uint16_t crc;
  table_header_t table;
  uint32_t base_addr;

  base_addr = get_partition(partition);
	int offset_addr = base_addr + seq*14;

  table.magic = PARTITION_MAGIC_WORD;
  table.length = 140;
  crc = crc_calc(0, (unsigned char*)in, (unsigned char*)(in + in_len));
  table.crc = crc;

	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr, table.magic);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr, table.magic);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr + 2, table.length);
	StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, base_addr + 4, table.crc);

  for(i = 0; i < in_len/2; i++)
	{
		if( 0 != StmL0xxEpromSingleWrite(EPROM_WR_TYPE_HALF_WORD, (uint32_t)(offset_addr+6+i*2), *(uint16_t*)(in+i*2)))
		{
			return -1;
		}
	}

  return 0;
}
