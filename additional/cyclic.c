
// (c)Arthur Ketels 2008

#include "cyclic.h"

void ec_sync(int64 reftime, int64 cycletime , int64 *offsettime)
{
  static int64 integral = 0;
  int64 delta;
  delta = (reftime - 50000) % cycletime;
  if(delta> (cycletime / 2)) { delta= delta - cycletime; }
  if(delta>0){ integral++; }
  if(delta<0){ integral--; }
  *offsettime = -(delta / 100) - (integral / 20);
  gl_delta = delta;
}



void add_timespec(struct timespec *ts, int64 addtime)
{
  int64 sec, nsec;

  nsec = addtime % NSEC_PER_SEC;
  sec = (addtime - nsec) / NSEC_PER_SEC;
  ts->tv_sec += sec;
  ts->tv_nsec += nsec;
  if ( ts->tv_nsec > NSEC_PER_SEC )
  {
    nsec = ts->tv_nsec % NSEC_PER_SEC;
    ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
    ts->tv_nsec = nsec;
  }
}

void initialise_cyclic_variables(){

  clock_gettime(CLOCK_MONOTONIC, &ts);
  ht = (ts.tv_nsec / 1000000) + 1;
  ts.tv_nsec = ht * 1000000;
  cycletime_ns = *(int*)&cycletime_us * 1000;
  toff = 0;
}

void sleep_cycle(){

  add_timespec(&ts, cycletime_ns + toff);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
}

void calc_offset(){

  if (ec_slave[0].hasdc)
  {
    ec_sync(ec_DCtime, cycletime_ns, &toff);
  }
}
