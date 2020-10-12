#include "nbiot.h"
#include "param.h"
#include "at_file.h"
#include "at_command.h"

#include <stdio.h>
#include <string.h>

void AtFileInitHanle(void);
void AtFileReadHandle(char*, int);
void AtFileWriteHanlde(char*, int);
void AtFileCheckHandle(void);
void AtFileFreeHandle(void);
