#include "stdio.h"
#include "time.h"
#include "myLog.h"

void LOG_time(){
    struct timespec _t{};
    time_t t = time(0);   // get time now
    tm* now = localtime(&t);
    clock_gettime(CLOCK_MONOTONIC_RAW, &_t);
    printf("[%02d.%02d.%02d][%02d:%02d:%02d.%09ld]", 
        now->tm_mday, now->tm_mon+1, now->tm_year%100,
        now->tm_hour, now->tm_min, now->tm_sec, _t.tv_nsec);
}