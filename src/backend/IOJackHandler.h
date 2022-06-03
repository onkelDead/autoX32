/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOJackHandler.h
 * Author: onkel
 *
 * Created on May 27, 2022, 2:48 PM
 */

#ifndef IOJACKHANDLER_H
#define IOJACKHANDLER_H

#include "OTypes.h"
#include "OConfig.h"

class IOJackHandler {
public:
    virtual ~IOJackHandler() {}
    virtual void notify_jack(JACK_EVENT) = 0;
    virtual OConfig* GetConfig() = 0;
    
};

#endif /* IOJACKHANDLER_H */

