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
                    if (!m_timer.isRunning()) {
                        m_timer.SetSamplePos(m_daw.GetCurrentSample());
                        UpdatePlayhead();
                    }
                    m_project.ProcessPos(NULL, &m_timer);
                    if (!m_button_play->get_active()) {
                        UpdateDawTime(false);
                    }
                }
                else {
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
                m_timer.start();
                lock_play = false;
                break;
            case DAW_PATH::stop:
                lock_play = true;
                m_button_play->set_active(false);
                m_timer.stop();
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
        OscCmd *cmd = my_mixerqueue.front();
        if (cmd->IsConfig()) {
            OscCmd* c = m_project.ProcessConfig(cmd);
            if (c) {
                OTrackView *tv = m_trackslayout.GetTrackview(c->GetPathStr());
                if (tv) {
                    tv->UpdateConfig();
                }
            }
        } else {

            bool changed = m_project.ProcessPos(cmd, &m_timer);

            OTrackView *tv = m_trackslayout.GetTrackview(cmd->GetPathStr());
            if (tv) {
                if (tv->GetTouch()) {
                    tv->SetRecord(true);
                }
                if (changed)
                    tv->queue_draw();
            } else {
                if (m_btn_teach->get_active()) {
                    if (m_trackslayout.GetTrackview(cmd->GetPathStr()) == NULL) {
                        m_project.AddCommand(cmd);
                        cmd->m_name = cmd->GetPathStr();
                        OTrackView *v = new OTrackView(this);
                        v->BindRemove(this);
                        OTrackStore* ts = m_project.NewTrack(cmd);
                        ts->Init();
                        v->SetTrackStore(ts);
                        v->SetDawTime(m_project.GetDawTime());
                        v->SetRecord(true);
                        v->UpdateConfig();
                        m_trackslayout.AddTrack(v);
                        show_all_children(true);
                        m_x32->Send(cmd->GetConfigName());
                        m_x32->Send(cmd->GetConfigColor());
                        my_mixerqueue.pop();
                        return;
                    }
                }
            }
        }

        delete cmd;
        my_mixerqueue.pop();
    }
}

void OMainWnd::notify_mixer(OscCmd *cmd) {
    my_mixerqueue.push(cmd);
    m_MixerDispatcher.emit();
}

void OMainWnd::TimerEvent(void* data) {
    
    // update UI-PlayHead every 50ms
    if (m_timer.GetRunTime() > m_last_playhead_update + 50) {
        UpdatePlayhead();
        m_last_playhead_update = m_timer.GetRunTime();
        // show timer process load percentage
        char load[32];
        sprintf(load, "Load: %.2f%%", m_timer.GetLoad());
        m_lbl_status->set_text(load);
    }
    m_project.ProcessPos(NULL, &m_timer);
}
