
//(c)Arthur Ketels 2010-2012

#ifndef MODIFIED_SOEM_FUNCTIONS_H
#define MODIFIED_SOEM_FUNCTIONS_H

#include <unistd.h>
#include <signal.h>
#include <ethercat.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>


#define MAXBUF 524288
#define MINBUF 128
#define EC_TIMEOUTMON 500
#define EC_TIMEOUTREC 5

int eeprom_read(int slave, int start, int length, uint8_t *ebuf);


#endif
