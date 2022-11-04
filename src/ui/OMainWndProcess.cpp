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

#include <iostream>
#include <gtkmm.h>
#include "OMainWnd.h"

void OMainWnd::OnDawEvent() {
    while (!my_dawqueue.empty()) {
        DAW_PATH c;
        my_dawqueue.front_pop(&c);
        switch (c) {
            case DAW_PATH::reply:
                m_project->SetMaxFrames(m_daw->GetMaxFrames());
                m_project->SetBitRate(m_daw->GetBitRate());
                UpdateDawTime(false);
                m_timeview->SetZoomLoop();
                break;
            case DAW_PATH::samples:
                m_backend->SetFrame(m_daw->GetSample() / 400);
                m_timeview->SetTimeCode(m_backend->GetTimeCode());
                m_project->UpdatePos(m_backend->GetFrame(), true);
                UpdatePlayhead(true);
                m_backend->ControlerShowMtcComplete(0);
                break;
            case DAW_PATH::session:
                m_mixer->PauseCallbackHandler(true);
                if (!OpenProject(m_daw->GetLocation())) {
                    std::cout << "OMainWnd: Load session " << m_daw->GetProjectFile() << std::endl;
                    if (m_config.get_boolean(SETTINGS_LOAD_CACHE))
                        m_mixer->WriteAll();
                    else
                        m_mixer->ReadAll();
                }
                else {
                    std::cout << "OService: no session " << m_daw->GetProjectFile() <<  ", -> created." << std::endl;
                    m_mixer->ReadAll();
                    m_project->Save(m_daw->GetLocation());
                }
                m_mixer->PauseCallbackHandler(false);
                set_title("autoX32 - [" + m_daw->GetLocation() + "]");
                break;
                
            default:
                break;
        }
    }
}

void OMainWnd::notify_daw(DAW_PATH path) {
    my_dawqueue.push(path);
    m_DawDispatcher.emit();
}

void OMainWnd::PublishUiEvent(E_OPERATION what, void *with) {
    operation_t *ue = new operation_t;
    ue->event = what;
    ue->context = with;
    PublishUiEvent(ue);
}

void OMainWnd::PublishUiEvent(operation_t *ue) {
    m_queue_operation.push(ue);
    m_ViewDispatcher.emit();
}

void OMainWnd::GetTrackConfig(IOTrackStore* trackstore){
    std::string conf_name = trackstore->GetConfigRequestName();
    
        
    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
    conf_name = trackstore->GetConfigRequestColor();
    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
}

void OMainWnd::OnUIOperation() {
    operation_t *op;
    m_queue_operation.front_pop(&op);
    if (op) {

        switch (op->event) {
            case E_OPERATION::new_channel:
            {
                IOscMessage* msg = (IOscMessage*) op->context;

                IOTrackStore *trackstore = m_project->NewTrack(msg);
                msg->SetTrackstore(trackstore);
                trackstore->SetPlaying(m_project->GetPlaying());

                if (!m_trackslayout.GetTrackview(msg->GetPath())) {
                    OTrackView *trackview = new OTrackView(this, m_project->GetDawTime());
                    trackview->SetPath(msg->GetPath());
                    trackview->SetTrackStore(trackstore);
                    trackstore->SetName(m_mixer->GetCachedMessage(trackstore->GetConfigRequestName())->GetVal(0)->GetString());
                    trackstore->SetColor_index(m_mixer->GetCachedMessage(trackstore->GetConfigRequestColor())->GetVal(0)->GetInteger());
                    trackview->SetTrackName(trackstore->GetName());
                    trackstore->SetView(trackview);
                    m_trackslayout.AddTrack(trackview, trackstore->GetLayout()->m_visible);
                    m_trackslayout.show_all();
                    m_trackslayout.UnselectTrack();
                    m_trackslayout.SelectTrack(msg->GetPath());
                    SelectTrack(msg->GetPath(), true);
                    trackview->SetRecord(true);
                    trackstore->SetRecording(true);
                }
                // TODO: get hidden tracks for all relevant parameters.
                
                
            }
                break;
            case E_OPERATION::draw_trackview:
            {
                ((OTrackView*) op->context)->queue_draw();
                delete op;
            }
                break;
            case E_OPERATION::new_pos:
                UpdatePlayhead(false);
                break;
            case E_OPERATION::play:
                m_sensitive = false;
                m_button_play->set_active(true);
                m_sensitive = true;
                break;
            case E_OPERATION::stop:
                m_sensitive = false;
                m_button_play->set_active(false);
                m_sensitive = true;
                m_trackslayout.StopRecord();
                break;
            case E_OPERATION::teach:
                m_sensitive = false;
                m_btn_teach->set_active(m_teach_active);
//                if (m_teach_mode) {
//                    if (m_btn_teach->get_active())
//                        m_btn_teach->set_active(false);
//                    else
//                        m_btn_teach->set_active(true);
//                } else {
//                    m_btn_teach->set_active(true);
//                }
                m_sensitive = true;
                break;
            case E_OPERATION::touch_off:
                if (!m_teach_mode) {
                    m_btn_teach->set_active(false);
                }
                break;
            case E_OPERATION::home:
                on_button_home_clicked();
                break;
            case E_OPERATION::end:
                on_button_end_clicked();
                break;

            case E_OPERATION::next_track:
                
                SelectTrackUI();
                break;
            case E_OPERATION::prev_track:
                SelectTrackUI();
                break;
            case E_OPERATION::unselect:
                m_trackslayout.UnselectTrack();
                SelectTrackUI();
                break;
            case E_OPERATION::toggle_rec:
            {
                m_sensitive = false;
                IOTrackStore *sts = m_project->GetTrackSelected();
                if (sts) {
                    m_trackslayout.GetTrackview(sts->GetPath())->SetRecord(sts->IsRecording());
                }
                m_sensitive = true;
            }
                break;  
            case E_OPERATION::toggle_recview:
            {
                IOTrackStore *sts = (IOTrackStore*)op->context;
                if (m_project->GetTrackSelected() == sts) {
                    m_backend->ControllerShowRec(sts->IsRecording());
                }
            }
                break;
//            case E_OPERATION::jump_forward:
//                if (m_backend->m_scrub)
//                    m_backend->Shuffle(false);
//                else
//                    m_backend->Locate(m_backend->GetFrame() + (m_backend->m_step_mode ? 1800 : 120));
//                break;
//            case E_OPERATION::jump_backward:
//                if (m_backend->m_scrub)
//                    m_backend->Shuffle(true);
//                else
//                    m_backend->Locate(m_backend->GetFrame() - (m_backend->m_step_mode ? 1800 : 120));
//                break;
            case E_OPERATION::touch_release:
            {
                if (m_btn_teach->get_active()) {
                    m_btn_teach->set_active(false);
                }
                IOTrackStore* sts = m_project->GetTrackSelected();
                if (sts) {
                    m_trackslayout.GetTrackview(sts->GetPath())->SetRecord(false);
                }
            }
                break;
//            case E_OPERATION::drop_track:
//                remove_track()
//                break;
            default:
                break;
        }
    }
}
