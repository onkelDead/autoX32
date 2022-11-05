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

int OService::InitMixer() {
    std::cout << "Initialize Mixer..." << std::endl;

    if (m_mixer->Connect(m_config.get_string(SETTINGS_MIXER_HOST))) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << m_config.get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete m_mixer;
        m_mixer = nullptr;
        return 1;
    }
    m_mixer->SetMessageHandler(this);
    
    std::cout << "Mixer initialized" << std::endl;
    return 0;
}

int OService::InitDaw() {
    std::cout << "Initialize DAW..." << std::endl;
    if (m_daw->Connect(m_config.get_string(SETTINGS_DAW_HOST), m_config.get_string(SETTINGS_DAW_PORT), m_config.get_string(SETTINGS_DAW__REPLAY_PORT), this)) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << m_config.get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete m_daw;
        return 1;
    }    
    std::cout << "DAW initialized." << std::endl;
    return 0;
}

int OService::InitBackend() {
    std::cout << "Initialize Backend..." << std::endl;

    if (m_backend->Connect(this)) {
        std::cerr << "autoX32_service ERROR: unable to initialize jack client" << std::endl;
        delete m_backend;
        m_backend = 0;
        return 1;
    }

    std::cout << "Backend initialized" << std::endl;
    return 0;
}

void OService::OnDawEvent() {
    while (!my_dawqueue.empty()) {
        DAW_PATH c;
        my_dawqueue.front_pop(&c);
        switch (c) {
            case DAW_PATH::reply:
                m_project->GetTimeRange()->m_loopend = m_daw->GetMaxFrames();
                m_session = m_daw->GetSessionName();
                std::cout << "OService::OnDawEvent session name " << m_session << std::endl;
                break;
            case DAW_PATH::samples:
                if (m_backend) {
                    m_backend->SetFrame(m_daw->GetSample() / 400);
                    m_project->UpdatePos(m_backend->GetFrame(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                break;
            case DAW_PATH::session:
                m_mixer->PauseCallbackHandler(true);
                std::cout << "OService: Load session " << m_daw->GetProjectFile() << std::endl;
                if (!m_project->Load(m_daw->GetLocation())) {
                    if (m_config.get_boolean(SETTINGS_LOAD_CACHE))
                        m_mixer->WriteAll();
                    else
                        m_mixer->ReadAll();
                    m_mixer->WriteAll();
                    std::map<std::string, IOTrackStore*> tracks = m_project->GetTracks();
                    for (std::map<std::string, IOTrackStore*>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
                        IOTrackStore* ts = it->second;
                        ts->GetMessage()->SetTrackstore(ts);
                        ts->SetName(m_mixer->GetCachedMessage(ts->GetConfigRequestName())->GetVal(0)->GetString());
                        ts->SetColor_index(m_mixer->GetCachedMessage(ts->GetConfigRequestColor())->GetVal(0)->GetInteger());
                    }
                    m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);
                }
                else {
                    std::cout << "OService: no session " << m_daw->GetProjectFile() <<  ", -> created." << std::endl;
                    m_mixer->ReadAll();
                    m_project->Save(m_daw->GetLocation());
                }
                m_mixer->PauseCallbackHandler(false);
                break;
            default:
                break;
        }
    }
}

void OService::StartProcessing() {
    m_mixer->Start();
    
    StartEngine(this);
    
    m_daw->StartSessionMonitor();
    m_active = true;

    m_backend->ControllerReset();
    m_backend->ControllerShowActive(true);
    
    std::cout << "Processing started." << std::endl;
    while(m_active) {
        usleep(10000);
    }
    std::cout << "Processing ended." << std::endl;
    
    m_daw->StopSessionMonitor();
    
    UnselectTrack();
    
    m_backend->ControllerReset();
    
    StopEngine();
    m_project->Save(m_daw->GetLocation());
}

void OService::OnTimer(void* user_data)  {
    m_backend->ReconnectPorts();
    OnJackEvent();
    OnDawEvent();
    OnMixerEvent();
}

void OService::OnJackEvent() {
    IOTrackStore* sts = m_project->GetTrackSelected();
    while (!m_jackqueue.empty()) {
        JACK_EVENT event;
        m_jackqueue.front_pop(&event);
        switch (event) {
            case CTL_SHUTDOWN:
                m_active = false;
                break;
            case CTL_SAVE:
                m_project->Save(m_daw->GetLocation());
                break;
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
                EngineLocate(event != MTC_COMPLETE);
                break;
            case MMC_PLAY:
                EnginePlay();
                break;
            case CTL_PLAY:
                if (m_playing) {
                    EngineStop();
                }
                else {
                    EnginePlay();
                }
                break;
            case CTL_STOP:
            case MMC_STOP:
                EngineStop();
                break;
            case MMC_RESET:
                m_daw->ShortMessage("/refresh");
                m_daw->ShortMessage("/strip/list");
                break;
            case CTL_TEACH_PRESS:
                EngineTeach(true);
                break;
            case CTL_TEACH_RELEASE:
                EngineTeach(false);
                break;
            case CTL_FADER:
                EngineFader();
                break;
            case CTL_TOUCH_RELEASE:
                if (sts != nullptr && sts->IsRecording()) {
                    sts->SetRecording(false);
                    m_backend->ControllerShowRec(false);
                }
                break;
            case CTL_TEACH_MODE:
                EngineTeachMode();
                break;
            case CTL_HOME:
                EngineHome();
                break;
            case CTL_END:
                EngineEnd();
                break;
            case CTL_NEXT_TRACK:
                EngineSelectNextTrack();
                break;
            case CTL_PREV_TRACK:
                EngineSelectPrevTrack();
                break;
            case CTL_UNSELECT:
                UnselectTrack();
                break;
            case CTL_DROP_TRACK:
                EngineDropMode();
                break;
            case CTL_KNOB:
                EngineDropTrack();
                break;
            case CTL_TOGGLE_REC:
                EngineToggleTrackRecord();
                break;
            case CTL_SCRUB_ON:
                m_backend->m_scrub = !m_backend->m_scrub;
                m_backend->ControllerShowScrub();
                break;
            case CTL_SCRUB_OFF:
                break;
            case CTL_STEP_MODE:
                EngineStepMode();
                break;
            case CTL_WHEEL_LEFT:
                EngineWheelLeft();
                break;
            case CTL_WHEEL_RIGHT:
                EngineWheelRight();
                break;
            case CTL_WHEEL_MODE:
                EngineWheelMode();
                break;
            case CTL_MARKER_PRESS:
                EngineMarker(true);
                break;
            case CTL_MARKER_RELEASE:
                EngineMarker(0);
                break;
            case CTL_LOOP_START:
                std::cout << "SetRange start " << m_project->GetTimeRange()->m_loopstart << std::endl;
                break;
            case CTL_LOOP_END:
          
                std::cout << "SetRange end " << m_project->GetTimeRange()->m_loopend << std::endl;
                break;
                
            case CTL_LOOP:
                EngineCycle();
                break;
            default:
                std::cout << "uncaught jack event id:" << event << std::endl;
                break;
        }
    }
}

int OService::NewMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    return 0;
}

int OService::UpdateMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    return 0;
}

void OService::ProcessSelectMessage(int idx) {
    char path[32];
    
    sprintf(path, "/ch/%02d/mix/fader", idx + 1);

    SelectTrack(path, true);
    return;
}

void OService::OnMixerEvent() {

    while (!my_messagequeue.empty()) {
        IOscMessage *msg;
        my_messagequeue.front_pop(&msg);

        IOTrackStore* ts = msg->GetTrackstore();
        if (ts) {
            
            if (m_teach_active) {
                ts->SetRecording(true);
            }
            
            int upd = ts->ProcessMsg(msg, m_backend->GetFrame());
        
            if (ts == m_project->GetTrackSelected()) {
                switch(upd) {
                    case 1:
                        m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                        break;
                    case 2:
                    case 3:
                        m_backend->ControllerShowLCDName(ts->GetName(), ts->GetColor_index());
                        break;
                }
            }
        }
        else {
            if (m_teach_active) { // I'm configured for teach-in, so create new track and trackview v
                if (std::regex_match (msg->GetPath(), std::regex(m_config.get_string(SETTINGS_TRACK_FILTER)) )) {
                    std::cout << "OService::OnMessageEvent new track " << msg->GetPath() << std::endl;
                    IOTrackStore *trackstore = m_project->NewTrack(msg);
                    msg->SetTrackstore(trackstore);    
                    trackstore->SetPlaying(m_playing);
                    trackstore->SetRecording(m_record);
                    std::string conf_name = trackstore->GetConfigRequestName();
                    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
                    m_mixer->Send(conf_name);
                    conf_name = trackstore->GetConfigRequestColor();
                    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
                    m_mixer->Send(conf_name);     
                    SelectTrack(msg->GetPath(), true);
                }
            }
        }
    }
}

void OService::GetTrackConfig(IOTrackStore* trackstore){
    std::string conf_name = trackstore->GetConfigRequestName();
    
        
    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
    conf_name = trackstore->GetConfigRequestColor();
    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
}

