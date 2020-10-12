#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "at_command.h"

void at_command_parament(AtCommand* at, const char* parament)
{
		memset(at->para, 0, sizeof(at->para));
		strcpy(at->para, parament);
}

void at_command_content(AtCommand* at, const char* context)
{
		memset(at->cnt, 0, sizeof(at->cnt));
		strcpy(at->cnt, context);
}

void at_command_factory(AtCommand *at)
{
	memset(at->cmd, 0, sizeof(at->cmd));

	if (at->type == testAT)
		sprintf(at->cmd, "AT+%s=?", at->para);
	else if (at->type == readAT)
		sprintf(at->cmd, "AT+%s?", at->para);
	else if (at->type == writeAT)
		sprintf(at->cmd, "AT+%s=%s", at->para, at->cnt);
	else if (at->type == Default)
		sprintf(at->cmd, "AT+%s", at->para);
}
