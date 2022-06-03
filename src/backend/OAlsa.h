/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OAlsa.h
 * Author: onkel
 *
 * Created on December 12, 2021, 10:14 AM
 */

#ifndef OALSA_H
#define OALSA_H

#include <alsa/asoundlib.h>
#include "IOBackend.h"
#include "OTimer.h"

class OAlsa : public IOBackend, IOTimerEvent {
public:
    OAlsa();
    virtual ~OAlsa();

    int Connect(IOJackHandler* wnd);
    void Disconnect() {};

    OMidiMtc* GetMidiMtc() {
        return &m_midi_mtc;
    }
    std::string GetTimeCode();
    int GetMillis();

    void Notify(JACK_EVENT event);

    void OnTimer(void*);
    
    void Play();
    void Stop();
    void Locate(int);
    void Shuffle(bool);
    void SetFrame(int);
    void QuarterFrame(uint8_t);

    void ControllerShowPlay();
    void ControllerShowStop();
    void ControllerShowRec(bool) {};
    void ControllerShowTeachOn();
    void ControllerShowTeachOff();
    void ControllerShowTeachMode(bool);
    void ControllerShowSelect(bool);
    void ControllerShowLCDName(std::string, int);
    void ControllerShowLevel(float);
    void ControlerShowMtcComplete(uint8_t);
    void ControlerShowMtcQuarter(uint8_t);
    void ControllerShowScrub();
    void ControllerShowWheelMode() {}
    void ControllerCustom(uint8_t c, uint8_t a, uint8_t b) {}
    void ControllerShowMarker() {}
    void ControllerShowCycle() {}
    void ControllerShowActive(bool) {};
    void LoopStart();
    void LoopEnd();
    bool GetLoopState();
    void SetLoopState(bool);
    
    snd_seq_t *alsa_client;
    struct pollfd *pfd;
    int npfd;

private:
    IOJackHandler* m_parent;
    OMidiMtc m_midi_mtc;
    bool m_loop_state = false;

    OTimer *m_timer = nullptr;
    
    int m_client_id;


};

#endif /* OALSA_H */

