#include <stdio.h>
#include <stdint.h>
#include <time.h> 

#include "defines.h"
#include "Modbus.h"
#include "App.h" 

LOG_INIT; 

// global time
long t_sec{}, t_ms{}, t_ns{};

int main(){
    LOG_TRACE("Start");

    srand(time(NULL));

    long _t_sec{}, _t_ms{};
    int _cnt_h=0, _cnt_100ms=0;
    struct timespec _t_now{};
    clock_gettime(CLOCK_MONOTONIC_RAW, &_t_now);
    _t_sec  = _t_now.tv_sec;
    _t_ms   = _t_now.tv_nsec/1000000;

    App app;
    app.init();  

    while (1) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &_t_now);
        t_sec   = _t_now.tv_sec;
        t_ms    = _t_now.tv_nsec/1000000;
        t_ns    = _t_now.tv_nsec;
        usleep(100);

        if(t_ms == _t_ms) continue; 
        _t_ms = t_ms; _cnt_100ms++;
        /* ---------------- task 1ms ---------------- */
        app.task();
        if(_cnt_100ms>=100){
            _cnt_100ms = 0;
        /* ---------------- task 100ms ---------------- */
            app.task100ms();
        }
        if(t_sec != _t_sec){
            _t_sec = t_sec;
        /* ---------------- task 1s ---------------- */
            app.task1s();
            // LOG_TRACE("1s");
            // _cnt_h++;
            if(_cnt_h>=3600){_cnt_h=0;  
                // app.task1h(); 
            }
        }
    }
}  