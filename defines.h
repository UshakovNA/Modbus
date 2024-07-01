#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include "myLog.h"

#define TM_DIF(t2, t1) (t2.tv_sec-t1.tv_sec)*1000+((t2.tv_nsec-t1.tv_nsec)/1000000)
#define TM_IS_0(t) (t.tv_sec==0 && t.tv_nsec==0)
#define TM_NOT_0(t) (t.tv_sec>0 && t.tv_nsec>0)

#define     brd_baud        B19200
#define     brd_port_name   "/dev/ttyUSB1" 

#endif /*DEFINES_H*/
