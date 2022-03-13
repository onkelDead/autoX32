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

#define ONKEL_C_IN_PORT "Onkel Controller in"
#define ONKEL_C_IN_PORT_NAME "autoX32:Onkel Controller in"
#define ONKEL_C_OUT_PORT "Onkel Controller out"
#define ONKEL_C_OUT_PORT_NAME "autoX32:Onkel Controller out"


#define CTL_COMMAND(a, b, c) ((a) << 16) + ((b) << 8) + (c)

typedef struct {
    uint8_t len;
    uint8_t buf[64];
    uint8_t mbf;
} ctl_command;

extern ctl_command s_stop_on;

extern ctl_command s_stop_off;

extern ctl_command s_play_on;

extern ctl_command s_play_off;
extern ctl_command s_rec_on;
extern ctl_command s_rec_off;
extern ctl_command s_f1_on;
extern ctl_command s_f1_off;
extern ctl_command s_scrub_on;
extern ctl_command s_scrub_off;
extern ctl_command s_wheel_mode_on;
extern ctl_command s_wheel_mode_off;
extern ctl_command s_select_on;
extern ctl_command s_select_off;

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
    virtual void ControllerShowRecOn() = 0;
    virtual void ControllerShowRecOff() = 0;   
    virtual void ControllerShowTeachMode(bool) = 0;
    virtual void ControllerShowSelect(bool) = 0;
     
    
    virtual void ControllerShowLCDName(std::string name) = 0;
    virtual void ControllerShowLevel(float) = 0;
    
    virtual void ControlerShowMtcComplete(uint8_t) = 0;
    virtual void ControlerShowMtcQuarter(uint8_t) = 0;
    
    virtual void ControllerShowScrub() = 0;
    
    virtual void ControllerShowWheelMode() = 0;
    
    virtual void Play() = 0;
    virtual void Stop() = 0;
    
    virtual void Locate(int) = 0;
    virtual void Shuffle(bool) = 0;
    virtual void SetFrame(gint) = 0;

    virtual void QuarterFrame(uint8_t) = 0;
    
    virtual void LoopStart() = 0;
    virtual void LoopEnd() = 0;
    virtual bool GetLoopState() = 0;
    virtual void SetLoopState(bool) = 0;
    
    std::queue<ctl_command*> ctl_out;
    std::queue<uint8_t> mmc_out;    
    
    uint8_t m_fader_val = 0;
    bool m_fader_touched = false;
    bool m_scrub = false;
    int m_shuffle_speed = 0;
    bool m_wheel_mode = false;
    
protected: 

};

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mmc_event(uint8_t* data, size_t len, IOBackend* backend);
int process_mtc_event(uint8_t* data, IOBackend* backend);

#endif /* IOBACKEND_H */

