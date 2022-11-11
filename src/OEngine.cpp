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

#include <regex>

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

int OEngine::InitDaw(IODawHandler* dawHandler) {
    std::cout << "Initialize DAW..." << std::endl;
    if (m_daw->Connect(m_config.get_string(SETTINGS_DAW_HOST), m_config.get_string(SETTINGS_DAW_PORT), m_config.get_string(SETTINGS_DAW__REPLAY_PORT), dawHandler)) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << m_config.get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete m_daw;
        return 1;
    }    
    std::cout << "DAW initialized." << std::endl;
    return 0;
}

int OEngine::InitMixer(IOMessageHandler* msgHandler) {
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

int OEngine::InitBackend(IOJackHandler* jackHandler) {
    std::cout << "Initialize Backend..." << std::endl;

    if (m_backend->Connect(jackHandler)) {
        std::cerr << "autoX32_service ERROR: unable to initialize jack client" << std::endl;
        delete m_backend;
        m_backend = 0;
        return 1;
    }

    m_backend->ControllerReset();

    std::cout << "Backend initialized" << std::endl;
    return 0;
}

void OEngine::StartEngine(IOTimerEvent* handler) {
    m_daw->StartSessionMonitor();

    m_mixer->Start();

    m_jackTimer.setInterval(3);
    m_jackTimer.SetUserData(&m_jackTimer);
    m_jackTimer.setFunc(handler);
    m_jackTimer.start();    
}

void OEngine::StopEngine() {
    while (!m_jackqueue.empty());
    m_jackTimer.stop();    
    m_daw->StopSessionMonitor();
    
}

void OEngine::OnTimer(void* user_data)  {
    m_backend->ReconnectPorts();
    OnJackEvent();
    OnMixerEvent();
}

void OEngine::OnDawEvent() {
    while (!my_dawqueue.empty()) {
        DAW_PATH c;
        my_dawqueue.front_pop(&c);
        switch (c) {
            case DAW_PATH::reply:
                m_project->SetMaxFrames(m_daw->GetMaxFrames());
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
                OnProjectLoad();
                break;
            default:
                break;
        }
    }
}

void OEngine::OnMixerEvent() {

    while (!my_messagequeue.empty()) {
        IOscMessage *msg;
        my_messagequeue.front_pop(&msg);

        IOTrackStore* trackstore = msg->GetTrackstore();
        if (trackstore) {
            
            if (m_teach_active) {
                trackstore->SetRecording(true);
            }
            
            int upd = 0;
            if ((upd = trackstore->ProcessMsg(msg, m_backend->GetFrame()))) {
                OnTrackUpdate(trackstore);
            }
        
            if (trackstore == m_project->GetTrackSelected()) {
                switch(upd) {
                    case 1:
                        if (trackstore->IsRecording())
                            m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                        break;
                    case 2:
                    case 3:
                        m_backend->ControllerShowLCDName(trackstore->GetName(), trackstore->GetColor_index());
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
                    OnTrackNew(trackstore);
                }
            }
        }
    }
}

int OEngine::NewMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    return 0;
}

int OEngine::UpdateMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    return 0;
}

void OEngine::ProcessSelectMessage(int idx) {
    char path[32];
    
    sprintf(path, "/ch/%02d/mix/fader", idx + 1);

    SelectTrack(path, true);
    return;
}

void OEngine::OnJackEvent() {
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
                EngineUnselectTrack();
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
            case CTL_CENTER_THIN:
                EngineCenterThin();
                break;
        }
    }
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
        OnSelectTrack();
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
    OnLocate();
}

void OEngine::EnginePlay() {
    m_playing = true;
    m_backend->Play();
    m_project->SetPlaying(m_playing);
    OnPlay();
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
    OnStop();
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
    OnTeach(m_teach_mode);
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
        OnMarkerStart();
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
        OnMarkerEnd();
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
    OnTrackRec();
}

void OEngine::EngineUnselectTrack() {
    UnselectTrack();
    OnUnselectTrack();
}

void OEngine::EngineFader() {
    IOTrackStore* sts = m_project->GetTrackSelected();
    if (sts) {
        if (m_teach_active && !sts->IsRecording()) {
            sts->SetRecording(true);
        }                    
        IOscMessage* msg = sts->GetMessage();
        msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
        if (sts->IsRecording()) {
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
            OnDropTrack();
        }
    }
    return ret;
}

void OEngine::EngineWheelLeft() {
    if (!m_wheel_mode) {
        m_backend->Locate(m_backend->GetFrame() - (m_step_mode ? 4 : 120));
    }
    else {
        EngineSelectPrevTrack();
    }
}

void OEngine::EngineWheelRight() {
    if (!m_wheel_mode) {
        m_backend->Locate(m_backend->GetFrame() + (m_step_mode ? 4 : 120));
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

void OEngine::EngineCenterThin() {
    OnCenterThin();
}