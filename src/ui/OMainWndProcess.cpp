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

#include "OscCmd.h"


void OMainWnd::OnDawEvent() {
    if (my_dawqueue.size() > 0) {
        DAW_PATH c = my_dawqueue.front();
        switch (c) {
            case DAW_PATH::reply:
                m_project.SetMaxMillis(m_daw.GetMaxMillis());
                m_project.SetBitRate(m_daw.GetBitRate());
                UpdateDawTime(false);
                m_timeview->SetZoomLoop();
                break;
            case DAW_PATH::samples:
                m_backend->SetFrame(m_daw.GetSample() / 400);
                m_timeview->SetTimeCode(m_backend->GetTimeCode());
                UpdatePos(m_backend->GetMillis(), true);
                UpdatePlayhead();

                break;
            default:
                break;
        }
        my_dawqueue.pop();
    }
}

void OMainWnd::notify_daw(DAW_PATH path) {
    my_dawqueue.push(path);
    m_DawDispatcher.emit();
}

void OMainWnd::PublishUiEvent(UI_EVENTS what, void *with) {
    ui_event *ue = new ui_event;
    ue->what = what;
    ue->with = with;
    m_new_ts_queue.push(ue);
    m_ViewDispatcher.emit();
}

void OMainWnd::PublishUiEvent(ui_event *ue) {
    m_new_ts_queue.push(ue);
    m_ViewDispatcher.emit();
}



void OMainWnd::OnViewEvent() {
    ui_event *e;
    m_new_ts_queue.front_pop(&e);
    if (e) {

        switch (e->what) {
            case UI_EVENTS::new_track:
            {
                OscCmd* cmd = (OscCmd*) e->with;

                m_project.AddCommand(cmd);
                cmd->SetName(cmd->GetPath());
                IOTrackStore *trackstore = m_project.NewTrack(cmd);
                trackstore->SetPlaying(m_project.m_playing);
                m_x32->Send(cmd->GetConfigRequestName());
                m_x32->Send(cmd->GetConfigRequestColor());
                m_x32->Send(cmd->GetStatsRequestSolo());
                
                if (!m_trackslayout.GetTrackview(trackstore->GetOscCommand()->GetPath())) {
                    OTrackView *trackview = new OTrackView(this, m_project.GetDawTime());
                    trackview->SetTrackStore(trackstore);
                    trackview->SetRecord(true);
                    trackview->UpdateConfig();
                    m_trackslayout.AddTrack(trackview, trackstore->GetLayout()->m_visible);

                }
                delete e;
            }
                break;

            case UI_EVENTS::draw_trackview:
            {
                ((OTrackView*) e->with)->queue_draw();
                delete e;
            }
                break;
            case UI_EVENTS::new_pos:
                UpdatePlayhead();
                break;
            case UI_EVENTS::play:
                m_button_play->set_active(true);
                m_backend->ControllerShowPlay();
                break;
            case UI_EVENTS::stop:
                m_button_play->set_active(false);
                m_backend->ControllerShowStop();
                break;
            case UI_EVENTS::touch_on:
                if (m_teach_mode) {
                    if (m_btn_teach->get_active())
                        m_btn_teach->set_active(false);
                    else
                        m_btn_teach->set_active(true);
                } else {
                    m_btn_teach->set_active(true);
                }
                break;
            case UI_EVENTS::touch_off:
                if (!m_teach_mode) {
                    m_btn_teach->set_active(false);
                }
                break;
            case UI_EVENTS::home:
                on_button_home_clicked();
                break;
            case UI_EVENTS::end:
                on_button_end_clicked();
                break;

            case UI_EVENTS::next_track:
                SelectTrack(m_trackslayout.GetNextTrack(), true);
                break;
            case UI_EVENTS::prev_track:
                SelectTrack(m_trackslayout.GetPrevTrack(), true);
                break;
            case UI_EVENTS::unselect:
                UnselectTrack();
                break;
            case UI_EVENTS::toggle_solo:
                ToggleSolo();
                break;
            case UI_EVENTS::jump_forward:
                if (m_backend->m_scrub)
                    m_backend->Shuffle(false);
                else
                    m_backend->Locate(m_backend->GetMillis() + 120);
                break;
            case UI_EVENTS::jump_backward:
                if (m_backend->m_scrub)
                    m_backend->Shuffle(true);
                else
                    m_backend->Locate(m_backend->GetMillis() - 120);
                break;
            case UI_EVENTS::touch_release:
                if (m_btn_teach->get_active()) {
                    m_btn_teach->set_active(false);
                }
                break;
            case UI_EVENTS::conf_track:
            {
                OscCmd* cmd = (OscCmd*) e->with;
                OscCmd* c = m_project.ProcessConfig(cmd);
                if (c) {
                    OTrackView *tv = m_trackslayout.GetTrackview(c->GetPath());
                    if (tv) {
                        tv->UpdateConfig();
                    }
                }
            }
                break;
        }
    }
}
