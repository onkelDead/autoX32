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
#include "OTrackStore.h"

OService::OService() {
    m_mixer = new OX32();
    m_daw = new ODAW();
    m_backend = new OJack(&m_config);
    
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
}

int OService::InitMixer() {
    if (m_mixer->Connect(m_config.get_string(SETTINGS_MIXER_HOST))) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << m_config.get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete m_mixer;
        m_mixer = nullptr;
        return 1;
    }
    m_mixer->SetMessageHandler(this);
    
    if (!CheckArdourRecent())
        m_mixer->ReadAll();
    return 0;
}

int OService::InitDaw() {
    if (m_daw->Connect(m_config.get_string(SETTINGS_DAW_HOST), m_config.get_string(SETTINGS_DAW_PORT), m_config.get_string(SETTINGS_DAW__REPLAY_PORT), this)) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << m_config.get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete m_daw;
        return 1;
    }    
    return 0;
}

int OService::InitBackend() {
    if (m_backend->Connect(this)) {
        std::cerr << "autoX32_service ERROR: unable to initialize kack client" << std::endl;
        delete m_backend;
        return 1;
    }

    return 0;
}

void OService::Load(std::string location) {
    SetLocation(location);
    std::string name = basename(m_location.data());

    m_projectFile = m_location;
    m_projectFile.append("/").append(name.append(".xml").data());

    xmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;

    doc = xmlParseFile(m_projectFile.data());
    if (doc == nullptr) {
        m_mixer->ReadAll();
        return;
    }
    context = xmlXPathNewContext(doc);
    result = xmlXPathEvalExpression(BAD_CAST "//project/range", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_daw_range.m_loopstart = GetInteger(node, "start");
        m_daw_range.m_loopend = GetInteger(node, "end");
        m_daw_range.m_dirty = false;
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/zoom", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_daw_time.m_viewstart = GetInteger(node, "start");
        m_daw_time.m_viewend = GetInteger(node, "end");
    }
    xmlXPathFreeObject(result);


    result = xmlXPathEvalExpression(BAD_CAST "//project/cmd", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        while(node) {
            if (node->type == XML_ELEMENT_NODE) {
                if (strcmp((const char*)node->name, "cmd") != 0)
                    break;
                xmlChar *xmlPath = xmlGetProp(node, BAD_CAST "path");
                xmlChar *xmlTypes = xmlGetProp(node, BAD_CAST "types");
                xmlChar *xmlValue = xmlGetProp(node, BAD_CAST "value");

                char* path = strdup((char*) xmlPath);
                char* types = strdup((char*) xmlTypes);
                char* val = strdup((char*) xmlValue);
//                m_known_mixer_commands[path] = m_mixer->AddCacheMessage(path, types);
                xmlFree(xmlPath);
                xmlFree(xmlTypes);
                xmlFree(xmlValue);
                char* name = (char*) xmlGetProp(node, BAD_CAST "name");
                m_mixer->AddCacheMessage(path, types, val);
                //m_known_mixer_commands[path]->SetName(name);
                xmlFree(name);
//                Gdk::RGBA color;
//                color.set_rgba_u(0, 0, 0, 32768);
//                color.set_red_u(GetInteger(node, "red"));
//                color.set_green_u(GetInteger(node, "green"));
//                color.set_blue_u(GetInteger(node, "blue"));
//                color.set_alpha_u(GetInteger(node, "alpha"));
                //m_known_mixer_commands[path]->SetColor(color);
                free(path);
                free(types);
                free(val);
            }
            node = node->next; 
        }
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/track", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        int c = 0;
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        while(node) {
            if (node->type == XML_ELEMENT_NODE) {
                if (strcmp((const char*)node->name, "track") != 0)
                    break;
                char* path = (char*) xmlGetProp(node, BAD_CAST "pat"
                "h");
                char* expanded = (char*) xmlGetProp(node, BAD_CAST "expand");
                char* height = (char*) xmlGetProp(node, BAD_CAST "height");
                char* layout_index = (char*) xmlGetProp(node, BAD_CAST "layout_index");
                char* visible = (char*) xmlGetProp(node, BAD_CAST "visible");
                
                IOscMessage* msg = m_mixer->GetCachedMessage(path);
                IOTrackStore *ts = NewTrack(msg);
                msg->SetTrackstore(ts);
                
                ts->GetLayout()->m_expanded = atoi(expanded);
                ts->GetLayout()->m_height = atoi(height);
                if (layout_index) {
                    ts->GetLayout()->m_index = atoi(layout_index);
                    xmlFree(layout_index);
                }
                else
                    ts->GetLayout()->m_index = c++;
                ts->GetLayout()->m_visible = visible ? atoi(visible) : true;
                        
                ts->LoadData(m_projectFile.c_str());
                if (visible)
                    xmlFree(visible);
                xmlFree(path);
                xmlFree(expanded);
                xmlFree(height);
            }
            node = node->next;
        }
    }
    xmlXPathFreeObject(result);

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        GetTrackConfig(ts);
    }    
}

void OService::Save() {
    if (m_location == "") 
        return;
    
    std::string name = basename(m_location.data());

    m_projectFile = m_location;
    m_projectFile.append("/").append(name.append(".xml").data());

    xmlTextWriterPtr writer;

    if (!std::filesystem::exists(m_location)) {
        std::filesystem::create_directory(m_location);
    }
    
    writer = xmlNewTextWriterFilename(m_projectFile.data(), 0);
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    xmlTextWriterStartElement(writer, BAD_CAST "project");
    SaveRange(writer);
    SaveZoom(writer);
    SaveCommands(writer);
    SaveTracks(writer);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
    m_dirty = false;
}

void OService::Close() {
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete (OTrackStore*)it->second;
    }
//    for (std::map<std::string, IOscMessage*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
//        delete (OscMessage*)it->second;
//    }
    m_tracks.clear();
    m_location = "";
    m_projectFile = "";
    m_daw_range.m_dirty = false;
}

int OService::GetInteger(xmlNodePtr node, const char* name) {
    xmlChar *keyword = xmlGetProp(node, BAD_CAST name);
    int result = atoi((const char *) keyword);
    xmlFree(keyword);
    return result;
}

void OService::SaveRange(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "range");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start", "%d", m_daw_range.m_loopstart);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end", "%d", m_daw_range.m_loopend);
    m_daw_range.m_dirty = false;
    xmlTextWriterEndElement(writer);
    printf("Project::Save: range saved\n");
}

void OService::SaveZoom(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "zoom");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start", "%d", m_daw_time.m_viewstart);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end", "%d", m_daw_time.m_viewend);
    xmlTextWriterEndElement(writer);
    printf("Project::Save: zoom saved\n");
}

void OService::SaveCommands(xmlTextWriterPtr writer) {
    m_mixer->Save(writer);
}

void OService::SaveTracks(xmlTextWriterPtr writer) {
    char cv[16];
    int index = 0;
    
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        sprintf(cv, "%d", ts->GetLayout()->m_expanded);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "expand", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_height);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "height", "%s", cv);
        sprintf(cv, "%d", index++);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "layout_index", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_visible);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "visible", "%s", cv);

        xmlTextWriterEndElement(writer);
        if (ts->IsDirty()) {
            ts->SaveData(m_projectFile.data());
            printf("Project::Save: track %s saved\n", it->first.data());
        }
    }
}

IOTrackStore* OService::NewTrack(IOscMessage* msg) {
    IOTrackStore* ts = new OTrackStore(msg);
    m_tracks[msg->GetPath()] = ts;
    return ts;
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
                    UpdatePos(m_backend->GetMillis(), true);
                }
                break;
            default:
                break;
        }
    }
}

void OService::UpdatePos(int current, bool seek) {
    bool ret_code = false;

    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;

        PlayTrackEntry(ts, ts->UpdatePos(current, seek));
        
        // update controller fader
//        IOTrackView* view = m_trackslayout.GetTrackview(ts->GetMessage()->GetPath());
//        if (ret_code && view && view->GetSelected()) {
//            m_backend->ControllerShowLevel(ts->GetPlayhead()->val.f);
//        }
    }
}

bool OService::PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry) {
    if (entry == nullptr || trackstore == nullptr)
        return false;
    IOscMessage* cmd = trackstore->GetMessage();
    switch (cmd->GetTypes()[0]) {
        case 'f':
            m_mixer->SendFloat(cmd->GetPath(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(cmd->GetPath(), entry->val.i);
            break;
        case 's':
            m_mixer->SendString(cmd->GetPath(), &entry->val.s);
            break;            
    }
    return true;    
}

void OService::StartProcessing() {
    m_dawTimer.setInterval(5000);
    m_dawTimer.SetUserData(&m_dawTimer);
    m_dawTimer.setFunc(this);
    m_dawTimer.start();
    
    m_daw->SetRange(m_daw_range.m_loopstart, m_daw_range.m_loopend);

    m_active = true;

    m_backend->ControllerShowActive(true);
    while(m_active) {
        sleep(1);
    }
    UnselectTrack();
    
    m_backend->ControllerShowActive(false);
    
    m_jackTimer.stop();
    m_dawTimer.stop();
}

void OService::OnTimer(void* user_data)  {
    if (user_data == &m_jackTimer) {
        OnJackEvent();
        OnDawEvent();
        OnMessageEvent();
        return;
    }
    
    CheckArdourRecent();
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
                Save();
                break;
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
                if (event != MTC_COMPLETE) {
                    UpdatePos(m_backend->GetMillis(), false);
                } else {
                    UpdatePos(m_backend->GetMillis(), true);
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
                IOTrackStore *trackstore = NewTrack(msg);
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
        for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
            IOTrackStore* ts = it->second;
            ts->SetRecording(false);
        }        
        m_backend->ControllerShowTeachOff();
    }
    else {
        m_backend->ControllerShowTeachOn();
    }
}

bool OService::CheckArdourRecent() {
    FILE* file_recent;
    char path[256];
    char name[256];

    file_recent = fopen("/home/onkel/.config/ardour6/recent", "r");
    if (file_recent != NULL) {
        fscanf(file_recent, "%s", name);-
        fscanf(file_recent, "%s", path);
        fclose(file_recent);
        strncat(path, "/autoX32", 32);
        if (strncmp(path, m_location.data(), strlen(path))) {
            Save();
            Close();

            if (access(path, F_OK)) {
                printf("project don't exists\n");
                if (std::filesystem::create_directory(path) == false) {
                    perror("mkdir() error");
                    return false;
                }
                m_location = path;
                m_projectFile = m_location;
                m_projectFile.append("/").append(name).append(".xml");                
            }
            else {
                Load(path);
                m_mixer->WriteAll();

                return true;
            }
        }
    }
    return false;
}

void OService::SelectNextTrack() {
    int c = 0;
    if (m_tracks.size() == 0) 
        return;
    m_selected_track_idx++;
    if (m_selected_track_idx >= m_tracks.size()) {
        m_selected_track_idx = 0;
    }
    
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        if (c==m_selected_track_idx) {
            m_selected_track = it->second;
            m_backend->ControllerShowLevel(m_selected_track->GetPlayhead()->val.f);
            m_backend->ControllerShowLCDName(m_selected_track->GetName(), m_selected_track->GetColor_index());
            m_backend->ControllerShowSelect(true);
            m_backend->ControllerShowRec(m_selected_track->IsRecording());
        }
        c++;
    }
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