// (c)Arthur Ketels 2008

#ifndef C_CYCLIC_H
#define C_CYCLIC_H

#include <stdio.h>
#include <stdbool.h>
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


int64 gl_delta;

void initialise_cyclic_variables();
void sleep_cycle_with_offset(int64_t cycletime);
void sleep_cycle(int64_t sleep);
void calc_offset(int64_t cycletime);


#endif
