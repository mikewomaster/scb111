#ifndef __AT_FILE_H__
#define __AT_FILE_H__

#include "at_ssl.h"
#include <stdint.h>

typedef struct _AT_File_ {
	void (*init) (void);
	void (*write) (char*, int);
	void (*read) (char*, int);
	void (*check) (void);
	void (*free) (void);
} AtFile;

void AtFileInitHanle(void);
void AtFileReadHandle(char*, int);
void AtFileWriteHanlde(char*, int);
void AtFileCheckHandle(void);
void AtFileFreeHandle(void);

#endif