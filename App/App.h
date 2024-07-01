#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "defines.h"
#include "SerialPort.h"
#include "Modbus.h"

class App
{
public:
    // App(Store &store, SQL sql) : store(store), db(sql){ 
    App(){ }

    void                init();
    void                task();
    void                task1s();
    void                task100ms();

private:
    SerialPort          port;
    Modbus              mdb{port};

};

#endif /*APPLICATION_H*/
