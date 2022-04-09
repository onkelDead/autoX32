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

class OAlsa : public IOBackend {
public:
    OAlsa();
    virtual ~OAlsa();

    void Connect(IOMainWnd* wnd);
    void Start();

    OMidiMtc* GetMidiMtc() {
        return &m_midi_mtc;
    }
    std::string GetTimeCode();
    int GetMillis();

    void Notify(JACK_EVENT event);

    void Play();
    void Stop();
    void Locate(gint);
    void Shuffle(bool);
    void SetFrame(gint);
    void QuarterFrame(uint8_t);

    void ControllerShowPlay();
    void ControllerShowStop();
    void ControllerShowRecOn();
    void ControllerShowRecOff();
    void ControllerShowTeachMode(bool);
    void ControllerShowSelect(bool);
    void ControllerShowLCDName(std::string);
    void ControllerShowLevel(float);
    void ControlerShowMtcComplete(uint8_t);
    void ControlerShowMtcQuarter(uint8_t);
    void ControllerShowScrub();
    void ControllerShowWheelMode() {}
    void LoopStart();
    void LoopEnd();
    bool GetLoopState();
    void SetLoopState(bool);
    
    snd_seq_t *alsa_client;
    struct pollfd *pfd;
    int npfd;

private:
    IOMainWnd* m_parent;
    OMidiMtc m_midi_mtc;
    bool m_loop_state = false;

    OTimer *m_timer = nullptr;
    
    int m_client_id;


};

#endif /* OALSA_H */

