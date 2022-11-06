/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOEngine.h
 * Author: onkel
 *
 * Created on June 13, 2022, 6:03 AM
 */

#ifndef IOENGINE_H
#define IOENGINE_H

#include "IODawHandler.h"
#include "IOMessageHandler.h"
#include "IOJackHandler.h"

class IOEngine {
public:
    virtual ~IOEngine(){}
    virtual void SelectTrack(std::string, bool) = 0;
    
    virtual int InitDaw(IODawHandler*) = 0;
    virtual int InitMixer(IOMessageHandler*) = 0;
    virtual int InitBackend(IOJackHandler*) = 0;
};

#endif /* IOENGINE_H */

