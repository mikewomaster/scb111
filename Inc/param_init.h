#ifndef __PARAM_INIT_H__
#define __PARAM_INIT_H__

#include <stdint.h>
#include "param.h"

#define EPROM_WR_TYPE_BYTE 									((uint8_t)0x00)
#define EPROM_WR_TYPE_HALF_WORD 						((uint8_t)0x01)
#define EPROM_WR_TYPE_WORD 									((uint8_t)0x02)

#define PARTITION_MAGIC_WORD (('R' << 0)|('L' << 8))

typedef enum  {
		// in flash
    PARTITION_0 = 0,
    PARTITION_1,
		PARTITION_2,
		PARTITION_3,
		PARTITION_4,
		PARTITION_5,
		PARTITION_6,
		PARTITION_7,
		PARTITION_8,
		PARTITION_9,
		PARTITION_10,
		PARTITION_11,
		PARTITION_12,
		PARTITION_13,
		PARTITION_14,
		// in memory
		PARTITION_MEM_RESET,
		PARTITION_MEM_SIGNAL,
		PARTITION_MEM_CHECKUSER,
}partition_index;

/* mapping with partition_map[] */
#define PARTITION_UART					PARTITION_0
#define PARTITION_LOAR					PARTITION_1
#define PARTITION_NET						PARTITION_2
#define PARTITION_AES						PARTITION_3
#define PARTITION_MODEL_NAME		PARTITION_4
#define PARTITION_SN						PARTITION_5
#define PARTITION_MODBUS_CTRL		PARTITION_6
#define PARTITION_SW_VERSION		PARTITION_7
#define PARTITION_USER_INFO			PARTITION_8
// #define PARTITION_MBUS					PARTITION_9
#define PARTITION_MODBUS_RTU		PARTITION_9
#define PARTITION_NBIOT					PARTITION_10
#define PARTITION_MQTT					PARTITION_11
#define PARTITION_SSL						PARTITION_12
#define PARTITION_COAP					PARTITION_13
#define PARTITION_DLMS					PARTITION_14

#define p_log(...)    // e_printf("part:"##__VA_ARGS__)

int read_partition(partition_index partition, char *out, unsigned short out_len);
int write_partition(partition_index partition, char *in, uint16_t in_len);
int write_partition_more_seq(partition_index partition, char *in, uint16_t in_len, int seq);
int read_partition_modbus_rtu(partition_index partition, char *out, uint16_t out_len, int offset);
#endif
