#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#include <stdint.h>

void  DebugPrintf (char  *p_fmt, ...);
// void exchangeBytes(unsigned char* byte);
int set_value_str(char *set_data, uint16_t num,char *ret_data);
int set_value_uint16(char *set_data, uint16_t num,char *ret_data);
int set_value_uint32(char *set_data, uint16_t num,char *ret_data);
int get_value_uint32(char *set_data, uint16_t num,char *ret_data);
int get_value_str(char *data, uint16_t num,char *ret_data);
unsigned short crc_Compute(unsigned char *PBuf,unsigned char len);

#endif
