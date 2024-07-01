
#pragma once
#ifndef SOCKET_UNIX_H
#define SOCKET_UNIX_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <time.h>
#include <iostream>
#include <stdexcept>
#include "defines.h"

class Socket_unix
{
public:
    Socket_unix();
    ~Socket_unix();

    const char* sock_name = HMI_SOCKET;

    int loop();
    int read(uint8_t* buf);
    void write(const uint8_t* buf, int len);
    bool isReady(){ return cnn; }

private:
    char rd_buf[1024]{};
    char wr_buf[1024]{};
    bool rd_flag=false;
    bool wr_flag=false;
    int sock, listener;
    int rd_bytes=0;
    int wr_bytes=0;
    // struct timespec _t_now, _t_start;
    int timeout = 0;
    bool cnn=0;

    int open(const char* sock_file);
};

#endif /*SOCKET_UNIX_H*/