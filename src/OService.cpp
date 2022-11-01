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
                m_project->GetTimeRange()->m_loopend = m_daw->GetMaxMillis();
                m_session = m_daw->GetSessionName();
                std::cout << "OService::OnDawEvent session name " << m_session << std::endl;
                break;
            case DAW_PATH::samples:
                if (m_backend) {
                    m_backend->SetFrame(m_daw->GetSample() / 400);
                    m_project->UpdatePos(m_backend->GetMillis(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                break;
            case DAW_PATH::session:
                m_mixer->PauseCallbackHandler(true);
                std::cout << "OService: Load session " << m_daw->GetProjectFile() << std::endl;
                if (!m_project->Load(m_daw->GetLocation())) {
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

    m_backend->ControlerShowMtcComplete(0);
    m_backend->ControllerShowActive(true);
    
    std::cout << "Processing started." << std::endl;
    while(m_active) {
        usleep(10000);
    }
    std::cout << "Processing ended." << std::endl;
    
    m_daw->StopSessionMonitor();
    
    UnselectTrack();
    
    m_backend->ControllerShowActive(false);
    m_backend->ControllerShowRec(false);
    m_backend->ControllerShowTeachMode(false);
    m_backend->ControllerShowTeachOff();
    m_backend->ControllerShowLevel(0.0);
    
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
            {
                IOTrackStore* sel_ts = nullptr;
                if (event != MTC_COMPLETE) {
                    sel_ts = m_project->UpdatePos(m_backend->GetMillis(), false);
                } else {
                    sel_ts = m_project->UpdatePos(m_backend->GetMillis(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                if (sel_ts != nullptr) {
                    m_backend->ControllerShowLevel(sel_ts->GetPlayhead()->val.f);
                }
            }
                break;
            case CTL_PLAY:
            case MMC_PLAY:
                m_backend->Play();
                m_project->SetPlaying(true);
                m_playing = true;
                break;
            case CTL_STOP:
            case MMC_STOP:
            {
                m_backend->Stop();
                m_project->SetPlaying(false);
                std::map<std::string, IOTrackStore*> tracks = m_project->GetTracks();
                for (std::map<std::string, IOTrackStore*>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
                    IOTrackStore* ts = it->second;
                    if (ts->IsRecording()) {
                            ts->SetRecording(false);
                            if (m_project->GetTrackSelected() == ts) {
                                m_backend->ControllerShowRec(false);
                            }
                        }
                    }
            }
                m_playing = false;
                break;
            case MMC_RESET:
                m_daw->ShortMessage("/refresh");
                m_daw->ShortMessage("/strip/list");
                break;
            case CTL_TEACH_ON:
                if (m_teach_mode) {
                    m_teach_active = !m_teach_active;
                } else {
                    m_teach_active = true;
                }
                SetRecord(m_teach_active);
                break;
            case CTL_TEACH_OFF:
                if (!m_teach_mode) {
                    m_teach_active = false;
                    SetRecord(m_teach_active);
                }
                
                break;
            case CTL_FADER:
                if (m_project->GetTrackSelected() != nullptr) {
                    IOscMessage* msg = m_project->GetTrackSelected()->GetMessage();
                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
                    my_messagequeue.push(msg);
                    m_mixer->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());                    
                }
                break;
            case CTL_TOUCH_RELEASE:
            {
                IOTrackStore* sts = m_project->GetTrackSelected();
                if (sts != nullptr && sts->IsRecording()) {
                    sts->SetRecording(false);
                    m_backend->ControllerShowRec(false);
                }
            }
                break;
            case CTL_TEACH_MODE:
                m_teach_mode = !m_teach_mode;
                m_backend->ControllerShowTeachMode(m_teach_mode);
                break;
            case CTL_HOME:
                m_backend->Locate(m_project->GetTimeRange()->m_loopstart);
                break;
            case CTL_END:
                m_backend->Locate(m_project->GetTimeRange()->m_loopend);
                break;
            case CTL_NEXT_TRACK:
                SelectNextTrack();
                break;
            case CTL_PREV_TRACK:
                SelectPrevTrack();
                break;
            case CTL_UNSELECT:
                UnselectTrack();
                break;
            case CTL_TOGGLE_REC:
                ToggleTrackRecord();
                break;
            case CTL_SCRUB_ON:
                m_backend->m_scrub = !m_backend->m_scrub;
                m_backend->ControllerShowScrub();
                break;
            case CTL_SCRUB_OFF:
                break;
            case CTL_JUMP_FORWARD:
                if (m_backend->m_scrub)
                    m_backend->Shuffle(false);
                else
                    m_backend->Locate(m_backend->GetMillis() + 120);
                break;
            case CTL_JUMP_BACKWARD:
                if (m_backend->m_scrub)
                    m_backend->Shuffle(true);
                else
                    m_backend->Locate(m_backend->GetMillis() - 120);
                break;
            case CTL_WHEEL_MODE:
                m_backend->ControllerShowWheelMode();
                break;
            case CTL_MARKER:
                m_backend->ControllerShowMarker();
                break;
            case CTL_LOOP_START:
                m_project->GetTimeRange()->m_loopstart = m_backend->GetMillis();
                m_project->GetTimeRange()->m_dirty = true;    
                m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);          
                std::cout << "SetRange start " << m_project->GetTimeRange()->m_loopstart << std::endl;
                break;
            case CTL_LOOP_END:
                m_project->GetTimeRange()->m_loopend = m_backend->GetMillis();
                m_project->GetTimeRange()->m_dirty = true;    
                m_daw->SetRange(m_project->GetTimeRange()->m_loopstart, m_project->GetTimeRange()->m_loopend);                
                std::cout << "SetRange end " << m_project->GetTimeRange()->m_loopend << std::endl;
                break;
                
            case CTL_LOOP:
                m_backend->ControllerShowCycle();
                m_daw->ShortMessage("/loop_toggle");
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
            IOTrackStore* ts = msg->GetTrackstore();
            int upd = ts->ProcessMsg(msg, m_backend->GetMillis());
        
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
                if (std::regex_match (msg->GetPath(), std::regex("/ch/.*/mix/(fader|pan|on)") )) {
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

void OService::SetRecord(bool val) {
    m_record = val;
    if (!m_record) {
        m_project->StopRecord();
        m_backend->ControllerShowTeachOff();
    }
    else {
        m_backend->ControllerShowTeachOn();
    }
}

void OService::SelectNextTrack() {
    SelectTrack(m_project->GetNextTrackPath(), true);
}

void OService::SelectPrevTrack() {
    SelectTrack(m_project->GetPrevTrackPath(), true);
}


void OService::GetTrackConfig(IOTrackStore* trackstore){
    std::string conf_name = trackstore->GetConfigRequestName();
    
        
    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
    conf_name = trackstore->GetConfigRequestColor();
    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
}

void OService::ToggleTrackRecord() {
    if (m_project->GetTrackSelected() == nullptr)
        return;
    
    bool isRec = m_project->GetTrackSelected()->IsRecording();
    m_project->GetTrackSelected()->SetRecording(!isRec);
    m_backend->ControllerShowRec(!isRec);
}
