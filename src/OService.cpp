/*
 Copyright 2020 Detlef Urban <onkel@paraair.de>

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <filesystem>
#include <regex>

#include "OService.h"
#include "OX32.h"
#include "OJack.h"
#include "OProject.h"
#include "OTrackStore.h"

OService::OService() {
    
}

OService::OService(const OService& orig) {
}

OService::~OService() {
}

void OService::StartProcessing() {
    
    StartEngine(this);
    
    m_active = true;

    m_backend->ControllerShowActive(true);
    
    std::cout << "Processing started." << std::endl;
    while(m_active) {
        usleep(10000);
    }
    std::cout << "Processing ended." << std::endl;
    
    
    UnselectTrack();
    
    m_backend->ControllerReset();
    
    StopEngine();
    m_project->Save(m_daw->GetLocation());
}

void OService::GetTrackConfig(IOTrackStore* trackstore){
    std::string conf_name = trackstore->GetConfigRequestName();
    
        
    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
    conf_name = trackstore->GetConfigRequestColor();
    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
}

void OService::OnProjectLoad() {
    m_backend->ControllerShowTeachMode(m_project->GetLockTeach());
    m_backend->ControllerShowTeach(m_project->GetTeachActive());
    m_backend->ControllerShowStepMode(m_project->GetStepMode());
    m_backend->ControllerShowWheelMode(m_project->GetWheelMode());
    
}