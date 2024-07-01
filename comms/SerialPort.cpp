#include "SerialPort.h"

int8_t SerialPort::open_port(const char *file, tcflag_t baud){
    // fd = open(file, O_RDWR | O_NOCTTY | O_NDELAY );
    fd = open(file, O_RDWR);
    if (fd <0) { return -1; }
    fcntl(fd, F_SETFL, FNDELAY);
    
    tcgetattr(fd,&options);
    tcgetattr(fd,&old_opts);

    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    // set input mode (non-canonical, no echo,...)
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // parity check
    // options.c_iflag |= (INPCK | ISTRIP);     // отрезает старший бит
    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN]     = 0;
    options.c_cc[VTIME]    = 10;
    
    tcsetattr(fd,TCSANOW,&options);
    tcflush(fd, TCIFLUSH);

    ready = 1;
    return 0;
}

ssize_t SerialPort::receive(uint8_t* buf, int8_t size){
    ssize_t res;
    res = read(fd,buf,size);  
    return res;
}

ssize_t SerialPort::send(const uint8_t* buf, uint16_t len){
    ssize_t res;
    res = write(fd, buf, len);
    tcdrain(fd);
    return res;
}

void SerialPort::stop(){
    tcsetattr(fd,TCSANOW,&old_opts);
    ready = false;
}
