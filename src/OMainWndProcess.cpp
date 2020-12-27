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
            case DAW_PATH::samples:
                if (!lock_daw_sample_event) {
//                    if (!m_timer.GetActive()) {
                        m_timer.SetSamplePos(m_daw.GetCurrentSample());
                        //UpdatePlayhead();
//                    }
                    if (!m_button_play->get_active()) {
                        UpdateDawTime(false);
                    }
                } else {
                    lock_daw_sample_event = false;
                }
                break;
            case DAW_PATH::smpte:
                m_timeview.m_timecode->set_text(m_daw.GetTimeCode());
                break;
            case DAW_PATH::reply:
                m_project.SetMaxSamples(m_daw.GetMaxSamples());
                m_project.SetBitRate(m_daw.GetBitRate());
                m_timer.SetSecDivide(m_daw.GetBitRate() / 1000);
                UpdateDawTime(false);
                break;
            case DAW_PATH::play:
                lock_play = true;
                m_button_play->set_active(true);
                m_timer.SetActive(true);
                lock_play = false;
                break;
            case DAW_PATH::stop:
                lock_play = true;
                m_button_play->set_active(false);
                m_timer.SetActive(false);
                lock_play = false;
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

void OMainWnd::OnMixerEvent() {
    while (my_mixerqueue.size() > 0) {
        bool cmd_used = false;
        OscCmd *cmd = my_mixerqueue.front();
        cmd->Parse();
        if (cmd->IsConfig()) {
            OscCmd* c = m_project.ProcessConfig(cmd);
            if (c) {
                OTrackView *tv = m_trackslayout.GetTrackview(c->GetPathStr());
                if (tv) {
                    tv->UpdateConfig();
                }
            }
        } else {
        	OTrackStore *trackstore = m_project.GetTrack(cmd->m_path);
        	OTrackView *tv = NULL;;
        	if (trackstore) {
				tv = m_trackslayout.GetTrackview(cmd->GetPathStr());
				if (tv) {
					if (tv->GetTouch()) {
						tv->SetRecord(true);
					}
				}
        	}
        	else {
				if (m_btn_teach->get_active()) {
					OscCmd *c = new OscCmd(*cmd);
					c->Parse();
					m_project.AddCommand(c);
					cmd->m_name = cmd->GetPathStr();
					OTrackStore* trackstore = m_project.NewTrack(c);
					trackstore->Lock();
					trackstore->Init();
					trackstore->Unlock();
					m_x32->Send(c->GetConfigName());
					m_x32->Send(c->GetConfigColor());
					cmd_used = true;
					ui_event *ue = new ui_event;
					ue->what = UI_EVENTS::new_track;
					ue->with = trackstore;
					m_new_ts_queue.push(ue);
					m_MixerDispatcher.emit();
				}
        	}
            if (m_project.ProcessPos(cmd, &m_timer)) {
            	if (tv && !m_project.GetPlaying()) {
					ui_event *ue = new ui_event;
					ue->what = UI_EVENTS::draw_trackview;
					ue->with = tv;
					m_new_ts_queue.push(ue);
					m_MixerDispatcher.emit();
            	}
            }
        }

        delete cmd;

        my_mixerqueue.pop();
    }
    m_project.ProcessPos(NULL, &m_timer);
}

void OMainWnd::notify_mixer(OscCmd *cmd) {
    my_mixerqueue.push(cmd);
    //    m_MixerDispatcher.emit();
}

void OMainWnd::TimerEvent(void* data) {
    ui_event e;

    // update UI-PlayHead/Load every 50ms
    if (m_timer.GetRunTime() > m_last_playhead_update + 50) {
        UpdatePlayhead();
        m_last_playhead_update = m_timer.GetRunTime();
//         show timer process load percentage
        sprintf(m_timer.load, "Load: %.2f%%", m_timer.GetLoad());
        m_new_ts_queue.push(&m_timer.ue);
        m_MixerDispatcher.emit();
    }
    OnMixerEvent();
}

void OMainWnd::OnViewEvent() {
    ui_event* e;
    m_new_ts_queue.front_pop(&e);
    if (e) {

        if (e->what == UI_EVENTS::new_track) {
            OTrackStore* trackstore = (OTrackStore*) e->with;
            if (!m_trackslayout.GetTrackview(trackstore->m_cmd->m_path)) {
                OTrackView *trackview = new OTrackView(this);
                trackview->SetTrackStore(trackstore);
                trackview->SetDawTime(m_project.GetDawTime());
                trackview->SetRecord(true);
                trackview->UpdateConfig();
                m_trackslayout.AddTrack(trackview);
            }
            delete e;
        }
        if (e->what == UI_EVENTS::load) {
            m_lbl_status->set_text((char*)e->with);
        }
        if (e->what == UI_EVENTS::draw_trackview) {
        	((OTrackView*)e->with)->queue_draw();
        	delete e;
        }
    }
}
