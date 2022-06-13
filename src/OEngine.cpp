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
    
    m_jackTimer.setInterval(10);
    m_jackTimer.SetUserData(&m_jackTimer);
    m_jackTimer.setFunc(this);
    m_jackTimer.start();    
}

OEngine::OEngine(const OEngine& orig) {
}

OEngine::~OEngine() {
    if (m_backend) {
        m_jackTimer.stop();
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

