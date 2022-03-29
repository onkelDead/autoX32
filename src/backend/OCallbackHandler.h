/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCallbackHandler.h
 * Author: onkel
 *
 * Created on March 19, 2022, 9:43 AM
 */

#ifndef OCALLBACKHANDLER_H
#define OCALLBACKHANDLER_H

#include "IOMessageHandler.h"

class OCallbackHandler : public IOMessageHandler {
public:
    OCallbackHandler();
    OCallbackHandler(const OCallbackHandler& orig);
    virtual ~OCallbackHandler();
    
    int NewMessageCallback(IOscMessage* msg);
    int UpdateMessageCallback(IOscMessage*);
    
private:

};

#endif /* OCALLBACKHANDLER_H */

