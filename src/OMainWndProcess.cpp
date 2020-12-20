/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
                m_project.SetSample(m_daw.GetCurrentSample());
                //m_project.ProcessPos(NULL);
                m_timeview.UpdateDawTime(false);
                m_timeview.queue_draw();
                //m_trackslayout.queue_draw();
                break;
            case DAW_PATH::smpte:
                m_timeview.m_timecode->set_text(m_daw.GetTimeCode());
                break;
            case DAW_PATH::reply:
                m_project.SetMaxSamples(m_daw.GetMaxSamples());
                m_project.SetBitRate(m_daw.GetBitRate());
                m_timeview.UpdateDawTime(false);
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

            bool changed = m_project.ProcessPos(cmd);

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
                        m_trackslayout.AddTrack(v);
                        show_all_children(true);
                        m_x32->Send(cmd->GetConfigName());
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
    m_project.ProcessPos(NULL);
}