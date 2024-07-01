#pragma once
#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H
// Serial port for Linux comp

#include <stdio.h>   /* Стандартные объявления ввода/вывода */
#include <stdint.h>
#include <string.h>  /* Объявления строковых функций */
#include <unistd.h>  /* Объявления стандартных функций UNIX */
#include <fcntl.h>   /* Объявления управления файлами */
#include <errno.h>   /* Объявления кодов ошибок */
#include <termios.h> /* Объявления управления POSIX-терминалом */

// #define BAUDRATE B115200
// #define MODEMDEVICE "/dev/ttyUSB0"
// #define _POSIX_SOURCE 1 /* POSIX compliant source */

class SerialPort
{
private:
    int fd;
    struct termios options, old_opts;
    // char buf[255];

public:
    int8_t      open_port(const char *file, tcflag_t baud);
    void        stop();
    ssize_t     receive(uint8_t* buf, int8_t size=50);
    ssize_t     send(const uint8_t* buf, uint16_t len);

    uint8_t     ready=0;
};

#endif /*SERIAL_PORT_H*/