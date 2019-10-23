// (c)Arthur Ketels 2008

#ifndef C_CYCLIC_H
#define C_CYCLIC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#include "ethercat.h"

#define NSEC_PER_SEC 1000000000

struct timespec   ts, tleft;
int ht, cycletime_us;
int64 cycletime_ns, toff, gl_delta;

void initialise_cyclic_variables();
void sleep_cycle();
void calc_offset();

#endif
