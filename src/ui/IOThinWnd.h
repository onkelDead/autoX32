/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOThinWnd.h
 * Author: onkel
 *
 * Created on January 6, 2023, 9:13 AM
 */

#ifndef IOTHINWND_H
#define IOTHINWND_H

#include "OConfig.h"
#include "OTypes.h"

class IOThinWnd {
public:
    virtual ~IOThinWnd(){}
    virtual OConfig* GetConfig() = 0;
private:

};

#endif /* IOTHINWND_H */

