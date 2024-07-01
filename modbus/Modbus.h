#pragma once
#ifndef MODBUS_H
#define MODBUS_H

#include <stdio.h>
#include <stdint.h>
#include <time.h> 

#include "defines.h"
#include "myLog.h"
#include "SerialPort.h"

// #define SEND_DIR_PIN        32 

#define REQ_N       3
#define RD_BUF_LEN  255
#define RD_DELAY    20

class Modbus {
public:
    Modbus(SerialPort& serial) : port{serial} { }
    void        init();
    void        task();             // call every cycle 
    void        task100ms();
    void        task1s();

    enum class CmmErrorType{
        None,
        Timeout,
        Slave,
        FuncID,
        RegN,
        MbErr,
        CRC
    };

    uint16_t        timeout                 = 500;              // ms
    CmmErrorType    last_err{};


private:
    SerialPort&     port;

    struct Req_1 {
        uint8_t     slave;                  // slave address
        uint8_t     func;                   // Modbus function code
        uint16_t    addr;                   // register address
        uint16_t    param;                  // num of regs or write value
        uint16_t    crc;
        uint8_t     en;
        uint16_t*   regs;
        int8_t      mb_err;
        uint16_t    cnt_fails;              // num of fails
        int16_t     tm_last;                // ms, time of request
    }; 
    Req_1       req_1{};
    Req_1       req_2{};
    Req_1       req_3{};
    Req_1*      p_req_que[REQ_N]{&req_1, &req_2, &req_3};
    int8_t      cur_req_n   = 0;
    Req_1*      p_cur_req   = p_req_que[cur_req_n];

    uint16_t    crc(uint8_t *data, uint8_t size); 

    bool        wait        = false;
    int8_t      send(); 
    int8_t      receive(); 
    void        reset_alr();
    struct ErrInfo {
        CmmErrorType cmm_err;
        int8_t mb_err;
    };
    ErrInfo getErr(uint8_t* rd_buf);
    int8_t  getMbRegs(uint8_t* buf, int16_t n_bytes, uint16_t* dest);
    int8_t  initRdRegsReq(Modbus::Req_1& req, uint8_t slave, uint8_t func, uint16_t addr, uint16_t n_regs, uint16_t* reg_buf);

    int8_t  onReadRegs(uint16_t* regs, int16_t len);

    timespec    tm_sent{};
};

#endif // MODUBUS_H