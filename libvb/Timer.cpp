
/// @file Timer.cpp
/// Implementation of the vb::Timer class

#include <vb/Timer.h>

namespace vb {
  Timer::Timer (double p) : sec(0), usec(0), t_sec(0), t_usec(0), period(p) {
    gettimeofday (&start,0);
    current = start;
    last = start;
  }

  void Timer::compute () {
    sec = current.tv_sec - last.tv_sec;
    usec = current.tv_usec - last.tv_usec;
    if (usec<0) { usec += 1000000; sec -= 1; }

    t_sec = current.tv_sec - start.tv_sec;
    t_usec = current.tv_usec - start.tv_usec;
    if (t_usec<0) { t_usec += 1000000; t_sec -= 1; }
  }

  bool Timer::check () {
    gettimeofday (&current,0);
    compute();
    return elapsed() > period;
  }

  void Timer::reset () {
    last = current;
    sec = 0; usec = 0;
  }

  double Timer::elapsed () {
    return double(sec) + .000001 * double(usec);
  }

  double Timer::t_elapsed () {
    return double(t_sec) + .000001 * double(t_usec);
  }
}