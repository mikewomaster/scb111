#ifndef _GSMODBUS_H_
#define _GSMODBUS_H_
#include "stdbool.h"

#define	READ_COIL_STA					0x01			// read coil status
#define	READ_INPUT_STA	 			0x02			// read input status
#define	READ_HOLD_REG					0x03			// read hold reg
#define	READ_INPUT_REG	 			0x04			// read input reg
#define	WRITE_COIL_REG	 			0x05			// write single coil 
#define	WRITE_HOLD_REG	 			0x06			// write hold reg 
#define	WRITE_COIL_OR_REG	 		0x15			// write mutil coil
#define	WRITE_HOLD_W_REG	 		0x10			// write mutil hold reg

typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B
} eMBException;


extern unsigned int gs_baudrate[10];

typedef struct _gsModbus_{

	unsigned char		Address;																	// modbus dev addr
	unsigned char		Function_Code;														// modbus fun code
	unsigned short	Regist_Addr; 															// modbus reg addr
	unsigned short	Regist_Size; 															// modbus reg size
	unsigned short	Regist_Byte_Cache[1][100];						// reg table

	unsigned char (*Set_HOLD_Function)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n);		
	void (*Read_HOLD_Function)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n);	

	void (*gsModbus_PutData)(unsigned char *PBuf, unsigned int length);	
	void (*gsModbus_forword)(unsigned char *pBuf, unsigned int size);
	void (*gsModbus_Check)(unsigned char *PBuf, unsigned int length);
}gsModbus;

void gsModbus_Processor(gsModbus *pModbus,unsigned char *pBuf,unsigned int length);

void gsModbus_Set_DirControl(gsModbus *pModbus,void (*fun)(unsigned char dir));

void gsModbus_Set_SendFunction(gsModbus *pModbus,void (*fun)(unsigned char *PBuf, unsigned int length));
 
void gsModbus_Set_ADDR(gsModbus *pModbus,unsigned char addr);

// void gsModbus_Set_Regist(gsModbus *pModbus,unsigned char cmd,unsigned char addr,unsigned short value);

void gsModbus_Dev_Coil_Function(gsModbus *pModbus,void (*fun)(unsigned short addr));

void gsModbus_Write_REG_Function(gsModbus *pModbus,unsigned char (*fun)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n));
void gsModbus_Read_REG_Function(gsModbus *pModbus, void (*fun)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n));

void gsModbus_01_02_Response(gsModbus *pModbus);
void gsModbus_03_04_Response(gsModbus *pModbus);
void gsModbus_05_06_Response(gsModbus *pModbus);
void gsModbus_16_Response(gsModbus *pModbus, unsigned char *pBuf);
void gsModbus_Exception_Response(unsigned char err_code, gsModbus *pModbus);

#endif /*_GSMODBUS_H*/
