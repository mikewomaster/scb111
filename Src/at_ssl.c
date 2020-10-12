#include "at_command.h"
#include "nbiot.h"
#include "param.h"
#include "at_ssl.h"

#include <stdio.h>
#include <string.h>

flashSSL gSSL;

static void SSLATSend(AtCommand at, char* buf)
{
	int cmdRet = 0;
	at_command_factory(&at);
	cmdRet = Send_AT_Command(at.cmd, buf, 2000, 2000);
}

static void SSLHandleParam(const char *content)
{
	AtCommand at;
	at_command_parament(&at, "CSSLCFG");
	at_command_content(&at, content);
	at.type = writeAT;
	SSLATSend(at, "OK");
}

void setSSLVersionHandle (void)
{
	char buf[64] = {0};
	sprintf(buf, "\"sslversion\",%d,%d", gSSL.index, gSSL.version);
	
	SSLHandleParam(buf);
}

void setSSLCiperHandle(void)
{
	char buf[64] = {0};
	sprintf(buf, "\"ciphersuite\",%d,%d,%d", gSSL.index, gSSL.cipherIndex, gSSL.cipherSuit);
	
	SSLHandleParam(buf);
}

void setSSLRtcHandle(void)
{
	char buf[64] = {0};
	sprintf(buf, "\"ignorertctime\",%d,%d", gSSL.index, gSSL.ignoreRTC);
	
	SSLHandleParam(buf);
}

void setSSLProtocolHandle(void)
{
	char buf[64] = {0};
	sprintf(buf, "\"protocol\",%d,%d", gSSL.index, gSSL.protocol);
	
	SSLHandleParam(buf);
}
void setSSLIndexHandle(void)
{
	char buf[64] = {0};
	sprintf(buf, "\"ctxindex\",%d", gSSL.index);
	
	SSLHandleParam(buf);
}

void setSSLServerHandle(void)
{
	char buf[64] = {0};
	sprintf(buf, "\"sni\",%d,%s", gSSL.index, gSSL.serverName);

	SSLHandleParam(buf);
}

// FIX ME: Check what does it mean
void setSSLConvert(void)
{

}
