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
#include "IOJackHandler.h"
#include "OMidiMtc.h"
#include "OQueue.h"

#define ONKEL_C_IN_PORT "Onkel Controller in"
#define ONKEL_C_IN_PORT_NAME "autoX32:Onkel Controller in"
#define ONKEL_C_OUT_PORT "Onkel Controller out"
#define ONKEL_C_OUT_PORT_NAME "autoX32:Onkel Controller out"


#define CTL_COMMAND(a, b, c) ((a) << 16) + ((b) << 8) + (c)

// Used X-Touch buttons
#define CTL_BUTTON_SELECT       0x03
#define CTL_BUTTON_REC          0x06
#define CTL_BUTTON_F1           0x07
#define CTL_BUTTON_F2           0x08
#define CTL_BUTTON_F3           0x09
#define CTL_BUTTON_F4           0x0a
#define CTL_BUTTON_F5           0x0b
#define CTL_BUTTON_F6           0x0c
#define CTL_BUTTON_MARKER       0x0d
#define CTL_BUTTON_CYCLE        0x0f
#define CTL_BUTTON_START        0x14
#define CTL_BUTTON_END          0x15
#define CTL_BUTTON_STOP         0x16
#define CTL_BUTTON_PLAY         0x17
#define CTL_BUTTON_TEACH        0x18
#define CTL_CURSOR_UP           0x1e
#define CTL_BUTTON_SCRUB        0x20
#define CTL_CURSOR_DOWN         0x22
#define CTL_FADER_TOUCH         0x46
#define CTL_WHEEL_LEFT_RIGHT    0x58

class IOBackend {
public:
    virtual ~IOBackend(){}
    virtual int Connect(IOJackHandler* wnd) = 0;
    virtual void Disconnect() = 0;
    
    virtual void ReconnectPorts() = 0;
    
    virtual OMidiMtc* GetMidiMtc() = 0; 
    virtual std::string GetTimeCode() = 0;
    virtual int GetMillis() = 0;

    virtual void Notify(JACK_EVENT event) = 0;

    virtual void ControllerShowPlay() = 0;
    virtual void ControllerShowStop() = 0;
    virtual void ControllerShowTeachOn() = 0;
    virtual void ControllerShowTeachOff() = 0;   
    virtual void ControllerShowTeachMode(bool) = 0;
    virtual void ControllerShowSelect(bool) = 0;
    virtual void ControllerShowRec(bool) = 0;
     
    
    virtual void ControllerShowLCDName(std::string name, int color) = 0;
    virtual void ControllerShowLevel(float) = 0;
    
    virtual void ControlerShowMtcComplete(uint8_t) = 0;
    virtual void ControlerShowMtcQuarter(uint8_t) = 0;
    
    virtual void ControllerShowScrub() = 0;
    
    virtual void ControllerShowWheelMode() = 0;
    virtual void ControllerShowActive(bool) = 0;
    virtual void ControllerCustom(uint8_t, uint8_t, uint8_t) = 0;
    
    virtual void ControllerShowMarker() = 0;
    virtual void ControllerShowCycle() = 0;
    
    virtual void Play() = 0;
    virtual void Stop() = 0;
    
    virtual void Locate(int) = 0;
    virtual void Shuffle(bool) = 0;
    virtual void SetFrame(int) = 0;

    virtual void QuarterFrame(uint8_t) = 0;
    
    virtual void LoopStart() = 0;
    virtual void LoopEnd() = 0;
    virtual bool GetLoopState() = 0;
    virtual void SetLoopState(bool) = 0;
    
    OQueue<ctl_command*> ctl_out;

    OQueue<uint8_t> mmc_out;    
    
    uint8_t m_fader_val = 0;
    bool m_fader_touched = false;
    bool m_scrub = false;
    int m_shuffle_speed = 0;
    bool m_wheel_mode = false;
    
    bool m_marker = false;
    bool m_cycle = false;
    
protected: 

};

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mmc_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mtc_event(uint8_t* data, IOBackend* backend);

#endif /* IOBACKEND_H */

