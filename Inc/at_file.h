#ifndef __AT_FILE_H__
#define __AT_FILE_H__

#include "at_ssl.h"
#include <stdint.h>

typedef struct _AT_File_ {
	void (*init) (void);
	void (*write) (char*, char*, int);
	void (*read) (char*, int);
	void (*check) (char*);
	void (*free) (void);
} AtFile;

void AtFileInitHanle(void);
void AtFileReadHandle(char*, int);
void AtFileWriteHanlde(char*, char*, int);
void AtFileCheckHandle(char*);
void AtFileFreeHandle(void);

#endif