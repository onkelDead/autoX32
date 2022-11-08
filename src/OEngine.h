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

class OEngine : public IOEngine, public IOTimerEvent, public IODawHandler, public IOMessageHandler, public IOJackHandler {
public:
    OEngine();
    OEngine(const OEngine& orig);
    virtual ~OEngine();
    
    int InitDaw(IODawHandler*);
    int InitMixer(IOMessageHandler*);
    int InitBackend(IOJackHandler*);

    void OnTimer(void*);
    
    void OnDawEvent();
    void notify_daw(DAW_PATH path) {
        my_dawqueue.push(path);
        OnDawEvent();
    };        
    void OnMixerEvent();
    
    void OnJackEvent();

    virtual void notify_jack(JACK_EVENT jack_event) {
        m_jackqueue.push(jack_event);
    }    
    
    int NewMessageCallback(IOscMessage*);
    int UpdateMessageCallback(IOscMessage*);
    void ProcessSelectMessage(int);    
    

    virtual void OnProjectLoad() {};
    virtual void OnTrackUpdate(IOTrackStore*) {}
    virtual void OnTrackNew(IOTrackStore*) {}
    virtual void OnLocate() {}
    virtual void OnPlay() {}
    virtual void OnStop() {}
    virtual void OnTeach(bool) {}
    virtual void OnUnselectTrack() {}
    virtual void OnSelectTrack() {}
    virtual void OnTrackRec() {}
    virtual void OnMarkerStart() {}
    virtual void OnMarkerEnd() {}
    virtual void OnDropTrack() {}
    virtual void OnCenterThin() {}
    
protected:
    OConfig m_config;

    IOProject* m_project = nullptr;
    ODAW* m_daw = nullptr;
    IOMixer* m_mixer = nullptr;
    IOBackend* m_backend = nullptr;

    OQueue<JACK_EVENT> m_jackqueue;
    OQueue<IOscMessage*> my_messagequeue;
    
    void StartEngine(IOTimerEvent*);
    void StopEngine();
    
    void SelectTrack(std::string, bool);
    void UnselectTrack(); 
    
    
    void EngineLocate(bool complete);
    void EnginePlay();
    void EngineStop();
    void EngineCycle();
    void EngineHome();
    void EngineEnd();
    void EngineTeach(bool);
    void EngineTeachMode();
    void EngineToggleTrackRecord();
    
    void EngineMarker(bool);
    
    void EngineFader();

    void EngineSelectNextTrack();
    void EngineSelectPrevTrack();    
    void EngineUnselectTrack();
    
    void EngineDropMode();
    bool EngineDropTrack();
    
    void EngineStepMode();
    void EngineWheelLeft();
    void EngineWheelRight();
    void EngineWheelMode();
    
    void EngineCenterThin();
    
protected:
    bool m_playing = false;
    bool m_teach_mode = false;
    bool m_teach_active = false;
    bool m_step_mode = false;
    bool m_wheel_mode = false;
    bool m_cycle = false;
    bool m_marker = false;
    bool m_record = false;

    std::string m_session;

    OQueue<DAW_PATH> my_dawqueue;
    
    std::atomic<bool> m_active = false;
    
    
private:
    OTimer m_jackTimer;
    
};

#endif /* OENGINE_H */

