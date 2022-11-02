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
    m_jackTimer.setInterval(20);
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