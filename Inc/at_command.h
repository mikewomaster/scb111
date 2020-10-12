#ifndef __NBIOT_H__
#define __NBIOT_H__

typedef enum __commandStatus {
	testAT = 0,
	readAT,
	writeAT,
	Default
} commandStatus;

typedef struct __command{
	commandStatus type;
	char para[16];
	char cnt[48];
	char cmd[64];
}AtCommand;

void at_command_factory(AtCommand*);
void at_command_parament(AtCommand* , const char*);
void at_command_content(AtCommand* , const char*);

#endif
