/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOBackend.h
 * Author: onkel
 *
 * Created on December 12, 2021, 9:23 AM
 */

#ifndef IOBACKEND_H
#define IOBACKEND_H

#include <stdlib.h>
#include "IOMainWnd.h"
#include "OMidiMtc.h"


class IOBackend {
public:
    virtual void Connect(IOMainWnd* wnd) = 0;
    
    virtual OMidiMtc* GetMidiMtc() = 0; 
    virtual std::string GetTimeCode() = 0;
    virtual int GetMillis() = 0;

    virtual void ControllerShowPlay() = 0;
    virtual void ControllerShowStop() = 0;
    virtual void ControllerShowTeachOn() = 0;
    virtual void ControllerShowTeachOff() = 0;    
    
    virtual void Play() = 0;
    virtual void Stop() = 0;
    
    virtual void Locate(int) = 0;
    virtual void SetFrame(gint) = 0;

    virtual void LoopStart() = 0;
    virtual void LoopEnd() = 0;
    virtual bool GetLoopState() = 0;
    virtual void SetLoopState(bool) = 0;
    
};

#endif /* IOBACKEND_H */

