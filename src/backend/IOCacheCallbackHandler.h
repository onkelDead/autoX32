/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOCacheCallbackHandler.h
 * Author: onkel
 *
 * Created on March 19, 2022, 9:40 AM
 */


#ifndef IOCACHECALLBACKHANDLER_H
#define IOCACHECALLBACKHANDLER_H

#include "IOscMessage.h"

class IOCacheCallbackHandler {
public:    
    virtual int NewMessageCallback(IOscMessage*) = 0;
    virtual int UpdateMessageCallback(IOscMessage*) = 0;
};


#endif /* IOCACHECALLBACKHANDLER_H */

