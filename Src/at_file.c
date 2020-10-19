#include "nbiot.h"
#include "param.h"
#include "at_ssl.h"
#include "at_file.h"
#include "at_command.h"

#include <stdio.h>
#include <string.h>

AtFile atFile;
extern flashSSL gSSL;
extern uint8_t usb_rcv_buff[128];

static void FileATSend(AtCommand at, char* buf)
{
	int cmdRet = 0;
	at_command_factory(&at);
	cmdRet = Send_AT_Command(at.cmd, buf, 2000, 2000);
}

void AtFileInitHanle(void)
{
	AtCommand at;
	at_command_parament(&at, "CFSINIT");
	at.type = Default;

  FileATSend(at, "OK");
}

void AtFileWriteHanlde (char* msg, char* fileName, int fileSize)
{
	char buf[16];
	AtCommand at;
	at_command_parament(&at, "CFSWFILE");
	at.type = writeAT;
	sprintf(buf, "%d,%s,%d,%d,%d", 0, fileName, 0, fileSize, 10000);
	at_command_content(&at, buf);
	at_command_factory(&at);

	// FIX ME: Check Function
	int cmdRet = Send_AT_Command(at.cmd, "", 10000, 10000);
  HAL_Delay(20);
	Send_AT_Command(msg, "OK", 10000, 10000);
}

void AtFileReadHandle (char* fileName, int fileSize)
{
	char buf[16];
	AtCommand at;
	at_command_parament(&at, "CFSRFILE");
	at.type = writeAT;
	sprintf(buf, "%d,\"%s\",%d,%d,%d", 0, fileName, 0, fileSize, 0);
	at_command_content(&at, buf);
	at_command_factory(&at);
	
	// FIX ME: Check Function
	int cmdRet = Send_AT_Command(at.cmd, "", 10000, 10000);
  HAL_Delay(20);
	// Send_AT_Command(msg, "OK", 10000, 10000);
}

void AtFileCheckHandle(char *fileName)
{
	char buf[32];
	AtCommand at;
	at_command_parament(&at, "CFSGFIS");
	at.type = writeAT;
	sprintf(buf, "%d,%s", 0, fileName),
	at_command_content(&at, buf);

	FileATSend(at, "OK");
}

void AtFileFreeHandle(void)
{
	AtCommand at;
	at_command_parament(&at, "CFSTREAM");
	at.type = Default;

  FileATSend(at, "OK");
}

void ATFileInit()
{
	atFile.init = AtFileInitHanle;
	atFile.read = AtFileReadHandle;
	atFile.write = AtFileWriteHanlde;
	atFile.check = AtFileCheckHandle;
	atFile.free = AtFileFreeHandle;
}

void ATFileWriteProcess(char *fileName, char* content)
{
		char fileNameBuffer[8];
		strcpy(fileNameBuffer, fileName);

		nbiot_poweroff_on();
		HAL_Delay(100);

		atFile.init();
		HAL_Delay(100);
		
		atFile.write(content, fileNameBuffer, strlen(content) - 8); // without the 8 char at the end 'endwowow'
		HAL_Delay(200);

		atFile.check(fileNameBuffer);
		HAL_Delay(100);

		atFile.free();
		HAL_Delay(100);

		nbiot_poweroff_on();
}
