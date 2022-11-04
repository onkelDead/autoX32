/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OEngine.h
 * Author: onkel
 *
 * Created on June 11, 2022, 12:25 PM
 */

#ifndef OENGINE_H
#define OENGINE_H

#include "IOEngine.h"
#include "IOProject.h"
#include "ODAW.h"
#include "IOMixer.h"
#include "IOBackend.h"

class OEngine : public IOEngine, public IOTimerEvent{
public:
    OEngine();
    OEngine(const OEngine& orig);
    virtual ~OEngine();
    
protected:
    OConfig m_config;

    IOProject* m_project = nullptr;
    ODAW* m_daw = nullptr;
    IOMixer* m_mixer = nullptr;
    IOBackend* m_backend = nullptr;

    OQueue<JACK_EVENT> m_jackqueue;
    
    void StartEngine(IOTimerEvent*);
    void StopEngine();
    
    void SelectTrack(std::string, bool);
    void UnselectTrack(); 
    
    
    void Locate(bool complete);
    void Play();
    void Stop();
    void Home();
    void End();
    void Teach(bool);
    void TeachMode();
    void ToggleTrackRecord();

    void SelectNextTrack();
    void SelectPrevTrack();    
    
protected:
    bool m_playing = false;
    bool m_teach_mode = false;
    bool m_teach_active = false;
    
    
private:
    OTimer m_jackTimer;
    
};

#endif /* OENGINE_H */

