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

#include <queue>
#include <stdlib.h>
#include "IOMainWnd.h"
#include "OMidiMtc.h"

#define CTL_COMMAND(a, b, c) ((a) << 16) + ((b) << 8) + (c)

class IOBackend {
public:
    virtual void Connect(IOMainWnd* wnd) = 0;
    virtual void Start() = 0;
    virtual OMidiMtc* GetMidiMtc() = 0; 
    virtual std::string GetTimeCode() = 0;
    virtual int GetMillis() = 0;

    virtual void Notify(JACK_EVENT event) = 0;

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
    
    std::queue<int> ctl_out;
    std::queue<uint8_t> mmc_out;    
protected: 

};

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mmc_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mtc_event(uint8_t* data, IOBackend* backend);

#endif /* IOBACKEND_H */

