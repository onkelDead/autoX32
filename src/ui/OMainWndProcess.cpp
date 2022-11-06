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
            case E_OPERATION::new_track:
            {
                IOTrackStore* trackstore = (IOTrackStore*) op->context;
                std::string path = trackstore->GetPath();
                
                if (!m_trackslayout.GetTrackview(path)) {
                    OTrackView *trackview = new OTrackView(this, m_project->GetDawTime());
                    trackstore->SetView(trackview);
                    trackview->SetPath(path);
                    trackview->SetTrackStore(trackstore);
                    trackview->SetTrackName(trackstore->GetName());
                    m_trackslayout.AddTrack(trackview, trackstore->GetLayout()->m_visible);
                    m_trackslayout.show_all();
                    m_trackslayout.UnselectTrack();
                    m_trackslayout.SelectTrack(path);
                    SelectTrack(path, true);
                    trackview->SetRecord(true);
                }                
            }
                break;
            case E_OPERATION::drop_track:
            {
                m_trackslayout.RemoveTrackView();

                break;
            }
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
                m_sensitive = true;
                break;

            case E_OPERATION::marker_start:
                m_sensitive = false;
                on_btn_loop_start_clicked();
                m_sensitive = true;
                break;
            case E_OPERATION::marker_end:
                m_sensitive = false;
                on_btn_loop_end_clicked();
                m_sensitive = true;
                break;
            case E_OPERATION::select_track:
                
                SelectTrackUI();
                break;

            case E_OPERATION::unselect_track:
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
            default:
                break;
        }
    }
}
