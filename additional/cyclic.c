
// (c)Arthur Ketels 2008

#include "cyclic.h"

struct timespec   ts, tleft;
int ht;
int64 toff;

static void ec_sync(int64 reftime, int64 cycletime , int64 *offsettime)
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



static void add_timespec(struct timespec *ts_, int64 addtime)
{
  int64 sec, nsec;

  nsec = addtime % NSEC_PER_SEC;
  sec = (addtime - nsec) / NSEC_PER_SEC;
  ts_->tv_sec += sec;
  ts_->tv_nsec += nsec;
  if ( ts_->tv_nsec > NSEC_PER_SEC )
  {
    nsec = ts_->tv_nsec % NSEC_PER_SEC;
    ts_->tv_sec += (ts_->tv_nsec - nsec) / NSEC_PER_SEC;
    ts_->tv_nsec = nsec;
  }
}

void initialise_cyclic_variables(){

  clock_gettime(CLOCK_MONOTONIC, &ts);
  ht = (ts.tv_nsec / 1000000) + 1;
  ts.tv_nsec = ht * 1000000;
  toff = 0;
}

void sleep_cycle_with_offset(int64_t cycletime){

  add_timespec(&ts, cycletime + toff);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
}

void sleep_cycle(int64_t sleep){

  add_timespec(&ts, sleep);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
}

void calc_offset(int64_t cycletime){

  if (ec_slave[0].hasdc)
  {
    ec_sync(ec_DCtime, cycletime, &toff);
  }
}
