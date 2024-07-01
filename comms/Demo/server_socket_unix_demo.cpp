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
using namespace std;

int open(char* sock_file){
    int listener;
    struct sockaddr_un addr;

    listener = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("socket"); exit(1);
    }
    int arg = fcntl(listener, F_GETFL);
    long flags = arg & ~O_NONBLOCK;
    flags |= O_NONBLOCK;
    fcntl(listener, F_SETFL, flags);
    
    unlink(sock_file);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, sock_file);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(2);
    }
    listen(listener, 1);
    return listener;
}

int main()
{
    int sock, listener;
    char buf[1024];
    int bytes_read;
    char* sock_name = "sb3_hmi.sock";

    listener = open(sock_name);

    int a=0, cnn=0;
    int rc=0;
    int msec=0, msec_now;
    struct timespec _t_now, _t_start;
    // -- debug
    int msec_dbg=0;
    struct timespec _t_dbg{};
    int cell_n=0;
    int act=1;
    //---------
    int timeout = 0;

    int8_t status[36]{};
    status[0] = act;

    while(1) {
        if(cnn==0){
            sock = accept4(listener, 0, 0, SOCK_NONBLOCK);
            if(sock < 0) {
                if(errno==EAGAIN){ if(!a){ printf("wait for connect\n"); a=1; } }
                else{ perror("accept4"); exit(3); }
            }
            else{ 
                clock_gettime(CLOCK_MONOTONIC_RAW, &_t_start);
                clock_gettime(CLOCK_MONOTONIC_RAW, &_t_dbg);
                cnn=1;
                printf("Connected!!!\n"); 
            }
        }
        else{
            bytes_read = recv(sock, buf, 1024, 0);
            if(errno!=EAGAIN){ printf("errno:%d ", errno); }
            if(bytes_read > 0){
                // обработка полученных данных
                // printf("read %d bytes: ", bytes_read);
                // for(int i=0;i<bytes_read;i++){ printf("%02x ",buf[i]); }
                // printf("\n");
                // отправка ответа
                send(sock, status, sizeof(status), 0);
                clock_gettime(CLOCK_MONOTONIC_RAW, &_t_start);
            }
            clock_gettime(CLOCK_MONOTONIC_RAW, &_t_now);
            msec_now = (_t_now.tv_sec-_t_start.tv_sec)*1000+((_t_now.tv_nsec-_t_start.tv_nsec)/1000000);
            // timeout, reopen socket
            if((timeout) && ( msec_now-msec)>=timeout ){
                printf("timeout, reconnect\n");
                close(sock); 
                cnn=0;a=0;
                listener = open(sock_name);
            }
            msec_dbg = (_t_now.tv_sec-_t_dbg.tv_sec)*1000+((_t_now.tv_nsec-_t_dbg.tv_nsec)/1000000);
            if(msec_dbg>=500){
                clock_gettime(CLOCK_MONOTONIC_RAW, &_t_dbg);
                status[cell_n++] = 0;
                if(cell_n>=sizeof(status)){
                    cell_n=0;
                    act<<=1;
                    if(act>8)act=1;
                }
                status[cell_n] = act;
            }
        }
    }   
    return 0;
}
