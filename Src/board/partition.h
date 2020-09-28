#ifndef _PARTITION_H_
#define _PARTITION_H_


#define PARTITION_MAGIC_WORD (('R' << 0)|('L' << 8))



typedef enum  {
  PARTITION_0 = 0,
  PARTITION_1,
	PARTITION_2,
	PARTITION_3,
	PARTITION_4,
	PARTITION_5,
	PARTITION_6,
	PARTITION_7,	
		// memory
	PARTITION_MEM_RESET,
	PARTITION_MEM_SIGNAL,	
	PARTITION_MEM_CHECKUSER,
}partition_index;


#define p_log(...)    // e_printf("part:"##__VA_ARGS__)
#define PARTITION_UART					  PARTITION_0
#define PARTITION_MODEL_NAME		  PARTITION_1
#define PARTITION_SN						  PARTITION_2
#define PARTITION_SW_VERSION			PARTITION_3 
#define PARTITION_USER_INFO				PARTITION_4	
#define PARTITION_MBUS						PARTITION_5
#define PARTITION_NBIOT						PARTITION_6
#define PARTITION_MQTT						PARTITION_7

int read_partition(partition_index partition, char *out, unsigned short out_len);


#endif
