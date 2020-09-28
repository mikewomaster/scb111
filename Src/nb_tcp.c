#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nbiot.h"
#include "nb_tcp.h"
#include "at_command.h"

void nbTcp_Set_Addr(gNbTCP *pNbTcp, uint8_t *addr)
{
	memset(pNbTcp->tcpAddr, 0, sizeof(pNbTcp->tcpAddr));
	strcpy((char *)pNbTcp->tcpAddr, (const char*)addr);
}

void nbTcp_Set_Ip(gNbTCP *pNbTcp, uint8_t *port)
{
	memset(pNbTcp->tcpPort, 0, sizeof(pNbTcp->tcpPort));
	strcpy((char *)pNbTcp->tcpPort, (const char*)port);
}

void tcpSetConnectHandle(gNbTCP *pTcp, void(*fun)())
{
	pTcp->tcpConnect = fun;
}

void tcpSetSendHandle(gNbTCP *pTcp, void(*fun)(uint8_t *, uint16_t))
{
	pTcp->tcpSend = fun;
}

void tcpSetRcvHandle(gNbTCP *pTcp, void (*fun)(uint8_t *))
{
	pTcp->tcpReceive = fun;
}

void tcpSetCheckHandle(gNbTCP *pTcp, uint8_t (*fun)())
{
	pTcp->tcp_chk_staus = fun;
}

void tcpSetCloseHandle(gNbTCP *pTcp, void (*fun)(void))
{
	pTcp->tcpClose = fun;
}

void tcpSetDisConnectHandle(gNbTCP* pTcp, void(*fun)())
{
  pTcp->tcpDisconnect = fun;
}

// handle
void TCPConnectHandle(void)
{
	nbiot_tcp_cnt();
}

void TCPSendHandle(uint8_t* szMsg, uint16_t len)
{
	char cmd[64] = {0};
	int cmdRet;

	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CASEND=0,%d", len);
	cmdRet = Send_AT_Command(cmd, ">", 2000, 2000);
	cmdRet = Send_AT_Command(szMsg, "OK", 5000, 5000);
}

static AtCommand at;

void TCPReceiveHandle()
{
	char cmd[64] = {0};
	int cmdRet, len;

	AtCommand* p_at = &at;
	at_command_parament(p_at, "CARECV");
	at_command_content(p_at, "0,100");

	p_at->type = writeAT;
	at_command_factory(p_at);
	cmdRet = Send_AT_Command(p_at->cmd, "OK", 2000, 2000);

	// memset(cmd,'\0',sizeof(cmd));
	// sprintf(cmd,"AT+CARECV=0,100");
}

void TCPDisConnectHandle()
{
	char cmd[64] = {0};
	int cmdRet;

	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CNACT=0,0");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
}

void TCPCloseHandle()
{
	char cmd[64] = {0};
	int cmdRet;

	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CACLOSE=0");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	
	/*
		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"AT+CACLOSE=1");
		cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	*/
	
}
