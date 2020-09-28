#ifndef __NB_MQTT__
#define __NB_MQTT__

#include <stdint.h>

typedef enum __mqtt_cfg_ {
	URL = 0,
	Topic,
	UserName,
	PassWord,
	ClientID
} mqttConfig;


typedef struct _gNbMQTT_{
	char mqttAddr[32];
	unsigned short mqttPort;
	char mqttTopic[64];
	char mqttClientID[32];
	unsigned int mqttInterval;
	char mqttUsername[16];
	char mqttPassword[16];

	void (*mqttOpen) (void);
	void (*mqttConnect) (void);
	void (*mqttConfigure)(mqttConfig);
	void (*mqttPub) (uint8_t* , uint16_t);
	void (*mqttSub)();
	void (*mqttClose)();
	void (*mqttDisconnect)();
	uint8_t (*mqttCheckStatus)(void);
}gNbMQTT;

void mqttAddrSet(gNbMQTT*);
void mqttPortSet(gNbMQTT*);
void mqttTopicSet(gNbMQTT*);
void mqttClientIdSet(gNbMQTT*);
void mqttIntervalSet(gNbMQTT*);
void mqttUsernameSet(gNbMQTT*);
void mqttPasswordSet(gNbMQTT*);

void mqttOpenSet(gNbMQTT *pNbMqtt, void (*fn)(void));
void mqttConnectSet(gNbMQTT *pNbMqtt, void (*fn)(void));
void mqttConfigureSet(gNbMQTT *pNbMqtt, void (*fn)(mqttConfig));
void mqttPubSet(gNbMQTT *pNbMqtt, void (*fn)(uint8_t* , uint16_t));
void mqttSubSet(gNbMQTT *pNbMqtt, void (*fn)());
void mqttCloseSet(gNbMQTT *pNbMqtt, void (*fn)());
void mqttDisconnectSet(gNbMQTT *pNbMqtt, void (*fn)());
void mqttCheckSet(gNbMQTT *pNbMqtt, uint8_t (*fn)(void));

void mqttOpenHandle(void);
void mqttConnectHandle (void);
void mqttConfigureHandle (mqttConfig);
void mqttPubHandle (uint8_t* , uint16_t);
void mqttSubHandle ();
void mqttCloseHandle ();
void mqttDisconnectHandle ();
uint8_t mqttCheckStatusHandle (void);

#endif
