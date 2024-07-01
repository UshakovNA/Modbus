#include "socket_unix.h"
#include "defines.h"

using namespace std;

LOG_INIT;

Socket_unix::Socket_unix() {
    listener = open(sock_name);
}
Socket_unix::~Socket_unix() {
    close(sock); 
    unlink(sock_name);
}

int Socket_unix::read(uint8_t* buf){
    if(rd_flag){
        memcpy(buf, rd_buf, rd_bytes);
        rd_flag=false;
        return rd_bytes;
    }
    else{ return -1; }
}
void Socket_unix::write(const uint8_t* buf, int len){
    memcpy(wr_buf, buf, len);
    wr_bytes=len;
    wr_flag=true;
}

int Socket_unix::open(const char* sock_file){
    int listener;
    struct sockaddr_un addr;
     
    // log_time(); puts("socket open");
    LOG_TRACE("socket open: %s", sock_file);

    listener = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listener < 0) {
        // log_time(); perror("socket"); exit(1);
        LOG_CRITICAL("socket"); exit(1);
    }
    int arg = fcntl(listener, F_GETFL);
    long flags = arg & ~O_NONBLOCK;
    flags |= O_NONBLOCK;
    fcntl(listener, F_SETFL, flags);
    
    unlink(sock_file);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, sock_file);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        // log_time(); perror("bind"); exit(2);
        LOG_CRITICAL("bind"); exit(2);
    }
    listen(listener, 1);
    return listener;
}

int Socket_unix::loop()
{
    static int a=0;

    if(!cnn){
        sock = accept4(listener, 0, 0, SOCK_NONBLOCK);
        if(sock < 0) {
            if(errno==EAGAIN){ if(!a){ 
                LOG_TRACE("Socket wait for connect ++++++++\n"); a=1; } }
            else{ 
                LOG_CRITICAL("accept4"); exit(3); }
        }
        else{ 
            cnn=true;
            LOG_TRACE("Socket connected\n"); }
    }
    else{
        rd_bytes= recv(sock, rd_buf, 1024, 0);
        // if(errno!=EAGAIN){ LOG_ERROR("errno:%d ", errno); }
        if(rd_bytes == 0){
            LOG_ERROR("Socket connection lost, reconnect");
            close(sock); 
            cnn=false;a=0;
            listener = open(sock_name);
            return 0;
        }
        if(rd_bytes> 0){
            rd_flag = true;
        }
        if(wr_flag){
            send(sock,wr_buf,wr_bytes,0);
            wr_flag = false;
        }
    }
    return 0;
}