#include "App.h"

LOG_INIT;

void App::init(){
    int8_t cnn = port.open_port(brd_port_name, brd_baud);
    if(-1 == cnn){
        LOG_ERROR("Serial port NOT open");
    }
    mdb.init();
}
void App::task()        { mdb.task();       }
void App::task100ms()   { mdb.task100ms();  }
void App::task1s()      { mdb.task1s();     }
