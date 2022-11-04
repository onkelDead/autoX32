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

void OEngine::Locate(bool complete) {
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

void OEngine::Play() {
    m_playing = true;
    m_backend->Play();
    m_project->SetPlaying(m_playing);
}

void OEngine::Stop() {
    m_playing = false;
    m_backend->Stop();
    m_project->SetPlaying(m_playing);   
    if (m_project->GetTrackSelected()) m_backend->ControllerShowRec(false);
}

void OEngine::Teach(bool pressed) {
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

void OEngine::TeachMode() {
    m_teach_mode = !m_teach_mode;
    m_backend->ControllerShowTeachMode(m_teach_mode);
    if (!m_teach_mode) {
        m_teach_active = false;
        m_project->StopRecord();
        m_backend->ControllerShowTeach(m_teach_mode);
    }    
}

void OEngine::Home() {
    m_backend->Locate(m_project->GetTimeRange()->m_loopstart);
}

void OEngine::End() {
    m_backend->Locate(m_project->GetTimeRange()->m_loopend);
}


void OEngine::SelectNextTrack() {
    SelectTrack(m_project->GetNextTrackPath(), true);
}

void OEngine::SelectPrevTrack() {
    SelectTrack(m_project->GetPrevTrackPath(), true);
}


void OEngine::ToggleTrackRecord() {
    if (m_project->GetTrackSelected() == nullptr)
        return;
    
    bool isRec = m_project->GetTrackSelected()->IsRecording();
    m_project->GetTrackSelected()->SetRecording(!isRec);
    m_backend->ControllerShowRec(!isRec);
}