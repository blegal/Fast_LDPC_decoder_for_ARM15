/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [bertrand.legal@ims-bordeaux.fr     ]
  [http://legal.vvv.enseirb-matmeca.fr]

  This file is part of Fast_LDPC_C_decoder_for_ARM15.

  Fast_LDPC_C_decoder_for_ARM15 is free software: you can redistribute it and/or modify

  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CTimer.h"

//
// END OF MACOS X SPECIAL DEFINTIION
//

#ifdef USE_CUSTOM_TIMER
long CTimer::diff_ns(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return (temp.tv_nsec);
}
#endif

#ifdef USE_CUSTOM_TIMER
long CTimer::diff_sec(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec  = end.tv_sec-start.tv_sec-1;
    } else {
        temp.tv_sec  = end.tv_sec-start.tv_sec;
    }
    return (temp.tv_sec);
}
#endif

CTimer::CTimer(bool _start){
    if(_start == true){
#ifndef USE_CUSTOM_TIMER
        timer.stop();
        timer.start();
#else
        clock_gettime(CLOCK_MONOTONIC, &t_start);
#endif
        isRunning = true;
    }else{
#ifndef USE_CUSTOM_TIMER
        timer.stop();
#endif
        isRunning = false;
    }
}

CTimer::CTimer(){
#ifndef USE_CUSTOM_TIMER
        timer.stop();
#endif
    isRunning = false;
}

void CTimer::start(){
    if( isRunning == true ){
        cout << "(EE) CTimer :: trying to start a CTimer object that is already running !" << endl;
    }
    isRunning = true;
#ifndef USE_CUSTOM_TIMER
    timer.start();
#else
    clock_gettime(CLOCK_MONOTONIC, &t_start);
#endif
}

void CTimer::stop(){
    if( isRunning == false ){
        cout << "(EE) CTimer :: trying to stop a CTimer object that is not running !" << endl;
    }
#ifndef USE_CUSTOM_TIMER
    timer.stop();
#else
    clock_gettime(CLOCK_MONOTONIC, &t_stop);
#endif
    isRunning = false;
}

void CTimer::reset(){
#ifndef USE_CUSTOM_TIMER
    timer.stop();
    timer.start();
#else
    clock_gettime(CLOCK_MONOTONIC, &t_start);
#endif
}

long CTimer::get_time_ns(){
#ifndef USE_CUSTOM_TIMER
    cpu_times const elapsed_times(timer.elapsed());
    nanosecond_type const elapsed(elapsed_times.system + elapsed_times.user);
    return elapsed;
#else
    if( isRunning == true ){
        clock_gettime(CLOCK_MONOTONIC, &t_stop);
    }
    return diff_ns( t_start, t_stop );
#endif
}

long CTimer::get_time_us(){
    return get_time_ns() / 1000;
}

long CTimer::get_time_ms(){
    return get_time_us() / 1000;
}

long CTimer::get_time_sec(){
#ifndef USE_CUSTOM_TIMER
    return get_time_ms() / 1000;
#else
    if( isRunning == true ){
        clock_gettime(CLOCK_MONOTONIC, &t_stop);
    }
    return diff_sec( t_start, t_stop );
#endif
}
