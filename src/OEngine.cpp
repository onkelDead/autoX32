/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OEngine.cpp
 * Author: onkel
 * 
 * Created on June 11, 2022, 12:25 PM
 */

#include <string.h>

#include "OProject.h"
#include "OEngine.h"
#include "OX32.h"
#include "OJack.h"

OEngine::OEngine() {
    m_mixer = new OX32();
    m_project = new OProject();
    m_daw = new ODAW();
    m_backend = new OJack(&m_config);

    m_project->SetMixer(m_mixer);
 
}

OEngine::OEngine(const OEngine& orig) {
}

OEngine::~OEngine() {
    if (m_backend) {
        if (m_jackTimer.isRunning()) {
            StopEngine();
        }
        m_backend->Disconnect();
        delete m_backend;
        m_backend = nullptr;
    }
    if (m_daw) {
        m_daw->Disconnect();
        delete m_daw;
        m_daw = nullptr;
    }
    if (m_mixer) {
        m_mixer->Disconnect();
        delete m_mixer;
        m_mixer = nullptr;
    }
    if (m_project) {
        m_project->Close();
        delete m_project;
        m_project = nullptr;
    }
}


void OEngine::StartEngine(IOTimerEvent* handler) {
    m_jackTimer.setInterval(5);
    m_jackTimer.SetUserData(&m_jackTimer);
    m_jackTimer.setFunc(handler);
    m_jackTimer.start();    
}

void OEngine::StopEngine() {
    while (!m_jackqueue.empty());
    m_jackTimer.stop();    
}

void OEngine::SelectTrack(std::string path, bool selected) {
    UnselectTrack();
    if (selected) {
        IOTrackStore* sts = m_project->SelectTrack(path);
        if (sts) {
            m_backend->ControllerShowLevel(sts->GetPlayhead()->val.f);
            m_backend->ControllerShowLCDName(sts->GetName(), sts->GetColor_index());
            m_backend->ControllerShowSelect(true);
            m_backend->ControllerShowRec(sts->IsRecording());
            if (path.starts_with("/ch")) {
                char idx[4] = {0, };
                memcpy(idx, path.data()+4, 2);
                m_mixer->SendInt("/-stat/selidx", atoi (idx)-1);
            }
        }
    } else {
        
        m_backend->ControllerShowLCDName("", 0);
        m_backend->ControllerShowSelect(false);
        m_backend->ControllerShowRec(false);
        m_backend->ControllerShowLevel(0.0);
    }
}

void OEngine::UnselectTrack() {
    IOTrackStore* sts = m_project->GetTrackSelected();
    
    if (!sts) return;
    
    std::string path = sts->GetPath();
    m_project->UnselectTrack();
    m_backend->ControllerShowLCDName("", 0);
    m_backend->ControllerShowSelect(false);
    m_backend->ControllerShowRec(false);
    m_backend->ControllerShowLevel(0.0);
    if (m_backend->m_drop_mode) {
        m_backend->ControllerShowDrop(false);
        m_backend->m_drop_mode = false;
    }
}

void OEngine::EngineLocate(bool complete) {
    bool sc = false;
    if (complete) {
        sc = m_project->UpdatePos(m_backend->GetFrame(), false);
    } else {
        sc = m_project->UpdatePos(m_backend->GetFrame(), true);
        m_backend->ControlerShowMtcComplete(0);
    }
    if (sc) {
        m_backend->ControllerShowLevel(m_project->GetTrackSelected()->GetPlayhead()->val.f);
    }    
}

void OEngine::EnginePlay() {
    m_playing = true;
    m_backend->Play();
    m_project->SetPlaying(m_playing);
}

void OEngine::EngineStop() {
    m_playing = false;
    m_backend->Stop();
    m_project->SetPlaying(m_playing);   
    if (m_project->GetTrackSelected()) m_backend->ControllerShowRec(false);
    if (m_cycle) {
        m_cycle = false;
        m_backend->ControllerShowCycle(false);
    }
}

void OEngine::EngineTeach(bool pressed) {
    if (pressed) {
        if (m_teach_mode) {
            m_teach_active = !m_teach_active;
        } else {
            m_teach_active = true;
        }
    }
    else {
        if (!m_teach_mode) {
            m_teach_active = false;
        }
    }
    if (!m_teach_active) {
        m_project->StopRecord();
    }
    m_backend->ControllerShowTeach(m_teach_active);
}

void OEngine::EngineTeachMode() {
    m_teach_mode = !m_teach_mode;
    m_backend->ControllerShowTeachMode(m_teach_mode);
    if (!m_teach_mode) {
        m_teach_active = false;
        m_project->StopRecord();
        m_backend->ControllerShowTeach(m_teach_mode);
    }    
}

void OEngine::EngineHome() {
    if (!m_marker)
        m_backend->Locate(m_project->GetTimeRange()->m_loopstart);
    else {
        m_project->GetTimeRange()->m_loopstart = m_backend->GetFrame();
        m_project->GetTimeRange()->m_dirty = true;    
        m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);          
        m_marker = false;
        m_backend->ControllerShowMarker(false);
    }
}

void OEngine::EngineEnd() {
    if (!m_marker)
        m_backend->Locate(m_project->GetTimeRange()->m_loopend);
    else {
        m_project->GetTimeRange()->m_loopend = m_backend->GetFrame();
        m_project->GetTimeRange()->m_dirty = true;    
        m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);              
        m_marker = false;
        m_backend->ControllerShowMarker(false);
    }
}

void OEngine::EngineSelectNextTrack() {
    SelectTrack(m_project->GetNextTrackPath(), true);
}

void OEngine::EngineSelectPrevTrack() {
    SelectTrack(m_project->GetPrevTrackPath(), true);
}

void OEngine::EngineToggleTrackRecord() {
    if (m_project->GetTrackSelected() == nullptr)
        return;
    
    bool isRec = m_project->GetTrackSelected()->IsRecording();
    m_project->GetTrackSelected()->SetRecording(!isRec);
    m_backend->ControllerShowRec(!isRec);
}

void OEngine::EngineFader() {
    IOTrackStore* sts = m_project->GetTrackSelected();
    if (sts) {
        if (m_teach_active && !sts->GetRecording()) {
            sts->SetRecording(true);
        }                    
        IOscMessage* msg = sts->GetMessage();
        msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
        if (sts->GetRecording()) {
            m_mixer->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
            sts->ProcessMsg(msg, m_backend->GetFrame());
        }
        m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
    }
}

void OEngine::EngineDropMode() {
    if (m_project->GetTrackSelected()) {
        m_backend->m_drop_mode = !m_backend->m_drop_mode;
        m_backend->ControllerShowDrop(m_backend->m_drop_mode);
    }    
}

bool OEngine::EngineDropTrack() {
    bool ret = false;
    if (m_backend->m_drop_mode) {
        IOTrackStore* sts = m_project->GetTrackSelected();
        if (sts != nullptr) {
            UnselectTrack();
            m_project->RemoveTrack(sts->GetPath());
            ret = true;
        }
    }
    return ret;
}

void OEngine::EngineWheelLeft() {
    if (!m_wheel_mode) {
        m_backend->Locate(m_backend->GetFrame() - (m_step_mode ? 1800 : 120));
    }
    else {
        EngineSelectPrevTrack();
    }
}

void OEngine::EngineWheelRight() {
    if (!m_wheel_mode) {
        m_backend->Locate(m_backend->GetFrame() + (m_step_mode ? 1800 : 120));
    }
    else {
        EngineSelectNextTrack();
    }
}

void OEngine::EngineStepMode() {
    m_step_mode = !m_step_mode;
    m_backend->ControllerShowStepMode(m_step_mode);
}

void OEngine::EngineWheelMode() {
    m_wheel_mode = !m_wheel_mode;
    m_backend->ControllerShowWheelMode(m_wheel_mode);
}

void OEngine::EngineCycle() {
    m_cycle = !m_cycle;
    m_backend->ControllerShowCycle(m_cycle);
    if (!m_cycle) {
        EngineStop();
    }
    else {
        m_daw->ShortMessage("/loop_toggle");
    }
}

void OEngine::EngineMarker(bool val) {
    if (m_marker && val) {
        m_marker = false;
    }
    else if (!m_marker && val) {
        m_marker = true;
    }
    m_backend->ControllerShowMarker(m_marker);
}