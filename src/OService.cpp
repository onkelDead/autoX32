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

#include "OService.h"
#include "OX32.h"
#include "OJack.h"
#include "OProject.h"
#include "OTrackStore.h"

OService::OService() {
    m_project = new OProject();
    m_mixer = new OX32();
    m_daw = new ODAW();
    m_backend = new OJack(&m_config);
    m_project->SetMixer(m_mixer);
    
    m_jackTimer.setInterval(10);
    m_jackTimer.SetUserData(&m_jackTimer);
    m_jackTimer.setFunc(this);
    m_jackTimer.start();    
}

OService::OService(const OService& orig) {
}

OService::~OService() {
    if (m_mixer) {
        m_mixer->Disconnect();
        delete m_mixer;
        m_mixer = nullptr;
    }
    if (m_daw) {
        m_daw->Disconnect();
        delete m_daw;
        m_daw = nullptr;
    }
    if (m_backend) {
        m_jackTimer.stop();
        m_backend->Disconnect();
        delete m_backend;
        m_backend = nullptr;
    }
    if (m_project) {
        m_project->Close();
        delete m_project;
        m_project = nullptr;
    }
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
                m_daw_range.m_loopend = m_daw->GetMaxMillis();
                m_session = m_daw->GetSessionName();
                std::cout << "OService::OnDawEvent session name " << m_session << std::endl;
                break;
            case DAW_PATH::samples:
                if (m_backend) {
                    m_backend->SetFrame(m_daw->GetSample() / 400);
                    m_project->UpdatePos(m_backend->GetMillis(), true);
                }
                break;
            case DAW_PATH::session:
                m_mixer->PauseCallbackHandler(true);
                if (!m_project->Load(m_daw->GetLocation())) {
                    std::cout << "OService: Load session " << m_daw->GetProjectFile() << std::endl;
                    m_mixer->WriteAll();
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
    m_daw->StartSessionMonitor();
    m_active = true;

    m_backend->ControllerShowActive(true);
    
    std::cout << "Processing started." << std::endl;
    while(m_active) {
        sleep(1);
    }
    std::cout << "Processing ended." << std::endl;
    
    m_daw->StopSessionMonitor();
    
    UnselectTrack();
    
    m_backend->ControllerShowActive(false);
    
    m_jackTimer.stop();
    m_dawTimer.stop();
    
    m_project->Save(m_daw->GetLocation());

}

void OService::OnTimer(void* user_data)  {
    if (user_data == &m_jackTimer) {
        OnJackEvent();
        OnDawEvent();
        OnMessageEvent();
        return;
    }
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
                if (event != MTC_COMPLETE) {
                    m_project->UpdatePos(m_backend->GetMillis(), false);
                } else {
                    m_project->UpdatePos(m_backend->GetMillis(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                break;
            case CTL_PLAY:
            case MMC_PLAY:
                m_backend->Play();
                m_playing = true;
                break;
            case CTL_STOP:
            case MMC_STOP:
                m_backend->Stop();
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
                if (m_selected_track != nullptr) {
                    IOscMessage* msg = m_selected_track->GetMessage();
                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
                    my_messagequeue.push(msg);
                    m_mixer->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());                    
                }
//                if (m_trackslayout.GetSelectedTrackView()) {
//                    IOTrackStore* store = m_trackslayout.GetSelectedTrackView()->GetTrackStore();
//                    IOscMessage* msg = store->GetMessage();
//                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
//                    my_messagequeue.push(msg);
//                    m_MessageDispatcher.emit();                    
//                    m_x32->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
//                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
//                }

                break;
            case CTL_TOUCH_RELEASE:
                //PublishUiEvent(E_OPERATION::touch_release, NULL);
                break;
            case CTL_TEACH_MODE:
                m_teach_mode = !m_teach_mode;
                m_backend->ControllerShowTeachMode(m_teach_mode);
                break;
                //            case CTL_LOOP_SET:
                //                if (!m_backend->GetLoopState()) {
                //                    on_btn_loop_start_clicked();
                //                    m_backend->LoopStart();
                //                } else {
                //                    on_btn_loop_end_clicked();
                //                    m_backend->LoopEnd();
                //                }
                //                break;
                //            case CTL_LOOP_CLEAR:
                //                m_backend->SetLoopState(false);
                //                m_daw.ClearRange();
                //                break;
                //            case CTL_TOGGLE_LOOP:
                //                m_daw.ShortMessage("/loop_toggle");
                //                break;
            case CTL_HOME:
                m_backend->Locate(m_daw_range.m_loopstart);
                break;
            case CTL_END:
                m_backend->Locate(m_daw_range.m_loopend);
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
            case CTL_TOGGLE_SOLO:
//                PublishUiEvent(E_OPERATION::toggle_solo, NULL);
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
                m_daw_range.m_loopstart = m_backend->GetMillis();
                m_daw_range.m_dirty = true;    
                m_daw->SetRange(m_daw_range.m_loopstart, m_daw_range.m_loopend);          
                std::cout << "SetRange start " << m_daw_range.m_loopstart << std::endl;
                break;
            case CTL_LOOP_END:
                m_daw_range.m_loopend = m_backend->GetMillis();
                m_daw_range.m_dirty = true;    
                m_daw->SetRange(m_daw_range.m_loopstart, m_daw_range.m_loopend);                
                std::cout << "SetRange end " << m_daw_range.m_loopend << std::endl;
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
//    char path[32];
//    
//    sprintf(path, "/ch/%02d/mix/fader", idx + 1);
//
//    if (m_tracks.find() == m_tracks.end())
//        return;
//    IOTrackView* st = m_tracks.at(path);
//    if (st)
//        SelectTrack(st->GetPath(), false);
    return;
}

void OService::OnMessageEvent() {

    while (!my_messagequeue.empty()) {
        IOscMessage *msg;
        my_messagequeue.front_pop(&msg);

        IOTrackStore* ts = msg->GetTrackstore();
        if (ts) {
            IOTrackStore* ts = msg->GetTrackstore();
            int upd = 0;
//            IOTrackView * view = m_trackslayout.GetTrackview(ts->GetMessage()->GetPath());
            
            if ((upd = ts->ProcessMsg(msg, m_backend->GetMillis()))) {
//                PublishUiEvent(E_OPERATION::draw_trackview, view);
            }
            if (ts == m_selected_track) {
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
            if (m_teach_active) { // I'm configured for teach-in, so create new track and trackview 
                std::cout << "OService::OnMessageEvent new track " << msg->GetPath() << std::endl;
                IOTrackStore *trackstore = m_project->NewTrack(msg);
                msg->SetTrackstore(trackstore);    
                trackstore->SetPlaying(m_playing);
                trackstore->SetRecording(m_playing);
                std::string conf_name = trackstore->GetConfigRequestName();
                m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
                m_mixer->Send(conf_name);
                conf_name = trackstore->GetConfigRequestColor();
                m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
                m_mixer->Send(conf_name);                
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
//    int c = 0;
//    if (m_tracks.size() == 0) 
//        return;
//    m_selected_track_idx++;
//    if (m_selected_track_idx >= m_tracks.size()) {
//        m_selected_track_idx = 0;
//    }
//    
//    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
//        if (c==m_selected_track_idx) {
//            m_selected_track = it->second;
//            m_backend->ControllerShowLevel(m_selected_track->GetPlayhead()->val.f);
//            m_backend->ControllerShowLCDName(m_selected_track->GetName(), m_selected_track->GetColor_index());
//            m_backend->ControllerShowSelect(true);
//            m_backend->ControllerShowRec(m_selected_track->IsRecording());
//        }
//        c++;
//    }
}

void OService::SelectPrevTrack() {
    
}

void OService::UnselectTrack() {
    m_backend->ControllerShowLCDName("", 0);
    m_backend->ControllerShowSelect(false);
    m_backend->ControllerShowRec(false);
    m_backend->ControllerShowLevel(0);
    m_selected_track = nullptr;
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
    if (m_selected_track == nullptr)
        return;
    
    bool isRec = m_selected_track->IsRecording();
    m_selected_track->SetRecording(!isRec);
    m_backend->ControllerShowRec(!isRec);
}