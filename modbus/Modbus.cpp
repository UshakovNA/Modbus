/* TODO
*  
*/
#include "Modbus.h"

#define swap16(x) ((x<<8)+(x>>8))

LOG_INIT;

uint16_t regs_1[5]{};
uint16_t regs_2[5]{};
uint16_t regs_3[5]{};

int8_t  Modbus::initRdRegsReq(Modbus::Req_1& req, uint8_t slave, uint8_t func, uint16_t addr, uint16_t n_regs, uint16_t* reg_buf){
    uint16_t _tmp=0;
    req.slave     = slave;
    req.func      = func;
    _tmp = addr;
    req.addr      = (_tmp<<8)+(_tmp>>8);          // register address
    _tmp = n_regs;
    req.param     = (_tmp<<8)+(_tmp>>8);          // number of registers
    req.crc       = crc((uint8_t*)&req, 6);
    req.regs      = reg_buf;
    return 0;
}

void Modbus::init(){
    initRdRegsReq(req_1,1,3,40960,5,regs_1);
    initRdRegsReq(req_2,1,3,40970,5,regs_2);
    initRdRegsReq(req_3,1,3,40975,5,regs_3);
    req_1.en = true;
    req_2.en = true;
    req_3.en = true;
    return; 
}

void Modbus::task(){
    send();
    receive();
}
void Modbus::task100ms(){ }
void Modbus::task1s(){ 
    // DEBUG - slow send
    // static int8_t cnt=0;
    // if(cnt>=2){ 
    //     cnt=0;
    //     send(); 
    // } else { cnt++; }
}
                  
void Modbus::reset_alr(){
    for(size_t i=0;i<=REQ_N;i++){
        p_req_que[i]->cnt_fails   = 0;
        p_req_que[i]->mb_err      = 0;
        p_req_que[i]->tm_last     = 0;
    }
}

int8_t Modbus::onReadRegs(uint16_t* regs, int16_t len){
    printf("read regs:\t");
    for(int16_t i=0;i<len;i++){
        printf("%02d\t", regs[i]);
    }
    printf("\n");
    return 0;
}

int8_t Modbus::send(){
    if(wait) return 1;
    if(!port.ready) {
        p_cur_req = nullptr;
        return 2;}
    int8_t ret = 0;

    if(cur_req_n>=REQ_N) cur_req_n = 0;
    p_cur_req = p_req_que[cur_req_n];
    bool next = false;
    if(!p_cur_req->en){ next = true; }
    else {
        switch(p_cur_req->func){
            case 3:
            case 4:
            case 5:
            case 6: {
                LOG_DEBUG("req send %d", cur_req_n);
                // DEBUG
                // uint8_t* ptr = (uint8_t*)p_cur_req;
                // for(size_t i=0;i<8;i++){ printf("%02x\t", *ptr++); }
                // printf("\n");
                port.send((uint8_t*)p_cur_req, 8);
                clock_gettime(CLOCK_MONOTONIC_RAW, &tm_sent);
                wait = true;
                } break;
            default: 
                LOG_ERROR("req function not implemented");
                p_cur_req->mb_err = 0x10;
                next = true;
                ret = 2;
                break;
        }
    }
    // next request
    if(next){ cur_req_n++; }
    return ret;
}

Modbus::ErrInfo Modbus::getErr(uint8_t* rd_buf){
    ErrInfo     einf{};
    int8_t      rd_slave    = rd_buf[0];
    int8_t      rd_func     = rd_buf[1];
    int16_t     byte_len    = 0;

    // error 2 - wrong slave
    if(p_cur_req->slave != rd_slave) einf.cmm_err = CmmErrorType::Slave;
    // error 3 - wrong function code
    else if( p_cur_req->func != (rd_func&~0x80) ) einf.cmm_err = CmmErrorType::FuncID;
    // error 4 - incorrect data len
    // else if( (rd_func==3) && (swap16(p_cur_req->param)!=(rd_buf[2]/2))) einf.cmm_err = CmmErrorType::RegN;
    else if(uint16_t qwe = swap16(p_cur_req->param); (rd_func==3) && (qwe!=rd_buf[2]/2) ) einf.cmm_err = CmmErrorType::RegN;
    // error 5 - Modbus error
    else if( rd_func&0x80 ) { 
            einf.cmm_err = CmmErrorType::MbErr; 
            p_cur_req->mb_err = rd_buf[2]; }
    // error 6 - CRC error
    else {
        uint16_t    rd_crc{};
        switch(rd_func){
            case 3:
            case 4:  
                    byte_len = rd_buf[2];
                break;
            case 5:
            case 6: 
            case 15: 
            case 16: 
                    byte_len = 3;
                break;
            default: break;
        }
        uint16_t pos = 3+byte_len;
        rd_crc = (rd_buf[pos]<<8)+rd_buf[pos+1];
        uint16_t _crc = crc(rd_buf, 3+byte_len);
        _crc = swap16(_crc);
        if(rd_crc != _crc) einf.cmm_err = CmmErrorType::CRC;
    }
    // error 7 - 15,16 - not all regs was written

    return einf;
}

int8_t  Modbus::getMbRegs(uint8_t* buf, int16_t n_bytes, uint16_t* dest){
    for(int16_t i=0;i<(n_bytes/2);i++){
        dest[i] = (buf[i*2]<<8)+buf[i*2+1];
    }
    return 0;
}

int8_t Modbus::receive(){
    if(!wait) return 1;
    if(!port.ready) return 2;

    timespec _tm{};
    clock_gettime(CLOCK_MONOTONIC_RAW, &_tm);

    uint16_t _tm_wait = TM_DIF(_tm, tm_sent);
    if(_tm_wait < RD_DELAY) return 3;

    bool next = 0;
    if(_tm_wait>=timeout) {
        last_err    = CmmErrorType::Timeout;   
        p_cur_req->cnt_fails++;
        p_cur_req->tm_last = timeout;
        next = 1;
        LOG_ERROR("req(%d) timeout", cur_req_n);
    } else {
        // response receive
        uint8_t buf[RD_BUF_LEN]{};
        // ssize_t n = port.receive(buf, RD_BUF_LEN);
        int16_t n = port.receive(buf,127);
        if(n<=0) return 4;
        LOG_TRACE("get data: %d\t req %d\t ms: %d", n, cur_req_n, _tm_wait);

        next = 1;
        p_cur_req->tm_last = _tm_wait;
        auto[last_err, mb_err]  = getErr(buf);

        // read data
        if(last_err==CmmErrorType::None){
            int8_t rd_func = buf[1];
            switch(rd_func){
                case 3:
                case 4:{
                    // int16_t len = swap16(p_cur_req->param);
                    getMbRegs(&buf[3],buf[2],p_cur_req->regs);
                    onReadRegs(p_cur_req->regs, buf[2]/2);
                    } break;
                default: break;
            }
            mb_err = 0;
        }
        else { LOG_ERROR("err: %d\tmberr: %d", (int16_t)last_err, mb_err); }
        p_cur_req->mb_err = mb_err;
    }
    // next request
    if(next){
        cur_req_n++; 
        wait = 0;
        LOG_TRACE("NEXT req");
    }
    return 0;
}

uint16_t Modbus::crc(uint8_t *data, uint8_t size) {
    uint16_t pos;
    uint16_t crc = 0xFFFF;
    for (pos = 0; pos < size; pos++) {
        crc ^= (uint16_t)data[pos];             // XOR byte into least sig. byte of crc
        for (uint8_t i = 8; i != 0; i--) {      // Loop over each bit
            if ((crc & 0x0001) != 0) {          // If the LSB is set
                crc >>= 1;                      // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else crc >>= 1;                   // Else LSB is not set // Just shift right
        }
    }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}  