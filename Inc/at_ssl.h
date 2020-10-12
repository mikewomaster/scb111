#ifndef __AT_SSL__
#define __AT_SSL__

#include <stdint.h>

typedef struct _flashSSLConvert_ {
	uint16_t type;
	char nameCertFile[16];
	char nameKeyFile[16];
	char passKey[16];
	uint16_t cnameLen;
	uint16_t keynameLen;
	uint16_t passkeyLen;
} flashSSLConvert;

typedef struct _flashSSH_ {
		uint16_t index;
		uint16_t version;
		uint16_t cipherIndex;
		uint16_t cipherSuit;
		uint16_t ignoreRTC;
		uint16_t protocol;
		uint16_t sni;
		flashSSLConvert convert;
		char serverName[16];
		uint16_t serverNameLen;
} flashSSL;

typedef struct _AtSSLFunc_ {
	void (*checkSSLStatus) (void);
	void (*setSSLVersion) (void);
	void (*setSSLCiper) (void);
	void (*setSSLRtc) (void);
	void (*setSSLProtocol)(void);
	void (*setSSLIndex)(void);
	void (*setSSLConvert)(void);
	void (*setSSLServer)(void);
} ATSSLFunc;

void checkSSLStatusHandle(void);
void setSSLVersionHandle(void);
void setSSLCiperHandle(void);
void setSSLRtcHandle(void);
void setSSLProtocolHandle(void);
void setSSLIndexHandle(void);
void setSSLConvertHandle(void);
void setSSLServerHandle(void);

#endif