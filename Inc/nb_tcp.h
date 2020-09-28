#ifndef __NB_TCP_
#define __NB_TCP_

#include <stdint.h>

typedef struct _gNbTCP_{
	uint8_t tcpAddr[64];
	uint8_t tcpPort[16];

	void (*tcpConnect) (void);
	void (*tcpSend) (uint8_t* str, uint16_t len);
	void (*tcpReceive)();
	void (*tcpClose)();
	void (*tcpDisconnect)();
	uint8_t (*tcp_chk_staus)(void);
}gNbTCP;

typedef enum _gsnbTcpStatus_ {
	tcpUnconnected = 0,
	tcpconnected = 1,
}nbTcpStatus;

void tcpSetConnectHandle(gNbTCP *pTcp, void(*fun)());
void tcpSetSendHandle(gNbTCP *pTcp, void(*fun)(uint8_t *, uint16_t));
void tcpSetRcvHandle(gNbTCP *pTcp, void (*fun)());
void tcpSetCheckHandle(gNbTCP *pTcp, uint8_t (*fun)());
void tcpSetCloseHandle(gNbTCP* pTcp, void (*fun)(void));
void tcpSetDisConnectHandle(gNbTCP* pTcp, void(*fun)());

void TCPConnectHandle(void);
void TCPSendHandle(uint8_t* szMsg, uint16_t len);
void TCPReceiveHandle();
void TCPCloseHandle();
void TCPDisConnectHandle();

#endif
