// Title:     stopwatch.h
// Purpose:   Simple millisecond stopwatch class

#ifndef _CUED_STOPWATCH_INCLUDED
#define _CUED_STOPWATCH_INCLUDED

#ifndef __cplusplus
#error stopwatch.h must be used from C++, not C code
#endif

#include <sys/time.h>


class stopwatch
{
public:
  stopwatch(bool);
  stopwatch();

  void start();
  int  read();
  int  stop();

private:
  struct timeval base_time;
  bool           running;
};

#endif /* _CUED_STOPWATCH_INCLUDED */
