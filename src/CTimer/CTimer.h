#ifndef CLASS_CTimer
#define CLASS_CTimer

#define USE_CUSTOM_TIMER

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

#ifndef USE_CUSTOM_TIMER
    #include "boost/timer/timer.hpp"
    using boost::timer::cpu_timer;
    using boost::timer::cpu_times;
    using boost::timer::nanosecond_type;
#else
    #include <time.h>
    #include <sys/time.h>
    #include <stdlib.h>

    #ifdef __MACH__
        #include <mach/clock.h>
        #include <mach/mach.h>
    #endif
#endif


class CTimer
{
    
private:
#ifndef USE_CUSTOM_TIMER
    cpu_timer timer;
#else
    timespec t_start;
    timespec t_stop;
#endif
    bool isRunning;
    
protected:
#ifdef USE_CUSTOM_TIMER
    long diff_ns(timespec start, timespec end);
    long diff_sec(timespec start, timespec end);
#endif

public:
    CTimer(bool _start);
    CTimer();
    void start();
    void stop();
    void reset();
    long get_time_ns();
    long get_time_us();
    long get_time_ms();
    long get_time_sec();
};

#endif
