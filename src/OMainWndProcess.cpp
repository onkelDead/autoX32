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

void OMainWnd::OnJackEvent() {
    if (m_jackqueue.size() > 0) {
        JACK_EVENT c = m_jackqueue.front();
        switch (c) {
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
                if (m_shot_refresh) {
                    if (!--m_shot_refresh)
                        if (settings->get_boolean(SETTING_SMOOTH_SCREEN))
                            this->get_window()->freeze_updates();
                }
                else if (m_jack.m_jackMtc.m_edge_sec) {
                    m_jack.m_jackMtc.m_edge_sec = false;
                    if (settings->get_boolean(SETTING_SMOOTH_SCREEN))
                        this->get_window()->thaw_updates();
                    m_shot_refresh = 3;
                }
                m_timeview->SetTimeCode(m_jack.GetTimeCode());
                m_project.UpdatePos(m_jack.GetMillis());
                UpdatePlayhead();
                break;
            case MMC_PLAY:
                m_lock_play = true;
                m_button_play->set_active(true);
                m_jack.ControllerShowPlay();
                m_lock_play = false;
                break;
            case MMC_STOP:
                m_lock_play = true;
                m_button_play->set_active(false);
                m_jack.ControllerShowStop();
                m_lock_play = false;
                break;
            case CTL_PLAYSTOP:
                m_lock_play = true;
                if (!m_button_play->get_active())
                    m_jack.Play();
                else
                    m_jack.Stop();
                m_lock_play = false;
                break;
            case CTL_TEACH_ON:
                m_btn_teach->set_active(true);
                //on_btn_teach_clicked();
                break;
            case CTL_TEACH_OFF:
                m_btn_teach->set_active(false);
                //on_btn_teach_clicked();
                break;
        }
    }
    m_jackqueue.pop();
}

void OMainWnd::OnDawEvent() {
    if (my_dawqueue.size() > 0) {
        DAW_PATH c = my_dawqueue.front();
        switch (c) {
                /*
                 * 	interval = 10ms | 0.01s
                 * 	bitrate = 48000Hz | 48000 samples per sec.
                 * 	=> 480 samples per interval.
                 *
                 * 	now timer pos stores real samples
                 * 	to store only steps, samples must be devide by 480
                 *
                 * 	48000 * 0.01 == 480
                 *
                 * 	512 == 0b100000000 (9 bits)
                 *
                 * 	48000 / 512 == new sample rate
                 *
                 *
                 *
                 */

            case DAW_PATH::timestr:
                //			if (!m_lock_daw_time_event) {
                //				int daw_millis = m_daw.GetMilliSeconds();
                //				int timer_millis = m_timer->GetPosMillis();
                //				if (m_timer->GetActive()) {
                //					int gap = daw_millis - timer_millis;
                //
                //					//printf("d:%d t:%d gap:%d\n", daw_millis, timer_millis, gap);
                //					m_timer->SetTimeRequest(daw_millis);
                //				}
                //
                //			} else {
                //				m_lock_daw_time_event = false;
                //			}
                //			if (!m_timer->GetActive()) {
                //				m_timer->SetPosMillis(m_daw.GetMilliSeconds());
                //				m_playhead->set_initialized(true);
                //				//m_timeview->ScaleView();
                //				UpdatePlayhead();
                //			}
                //m_timeview->SetTimeCode(m_daw.GetTimeCode());
                break;
            case DAW_PATH::reply:
                m_project.SetMaxMillis(m_daw.GetMaxMillis());
                m_project.SetBitRate(m_daw.GetBitRate());
                UpdateDawTime(false);
                m_timeview->SetZoomLoop();
                break;
            case DAW_PATH::play:
                //			m_lock_play = true;
                //			m_button_play->set_active(true);
                //			m_lock_play = false;
                break;
            case DAW_PATH::stop:
                //			m_lock_play = true;
                //			m_button_play->set_active(false);
                //			m_lock_play = false;
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

void OMainWnd::notify_jack(JACK_EVENT jack_event) {
    m_jackqueue.push(jack_event);
    m_JackDispatcher.emit();
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

void OMainWnd::OnMixerEvent() {

    bool step_processed = false;

    while (my_mixerqueue.size() > 0) {
        bool cmd_used = false;
        OscCmd *cmd = my_mixerqueue.front();
        cmd->Parse();
        if (cmd->IsConfig()) {
            OscCmd *c = m_project.ProcessConfig(cmd);
            if (c) {
                OTrackView *tv = m_trackslayout.GetTrackview(c->GetPath());
                if (tv) {
                    tv->UpdateConfig();
                }
            }
        } else {
            // is this track already known ?
            OTrackStore *trackstore = m_project.GetTrack(cmd->GetPath());
            OTrackView *tv = NULL;
            ;
            if (trackstore) { // the track is known
                tv = m_trackslayout.GetTrackview(cmd->GetPath());
                if (tv) { // we have a trackview for it
                    if (tv->GetTouch() || m_btn_teach->get_active()) { // trackview is configured for touch
                        tv->SetRecord(true);
                    }
                }
            } else { // the track is not known
                if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create track and trackview
                    OscCmd *c = new OscCmd(*cmd);
                    c->Parse(); // TODO: check if obsolete, view above cmd->parse()
                    m_project.AddCommand(c);
                    cmd->SetName(cmd->GetPath());
                    trackstore = m_project.NewTrack(c);
                    m_x32->Send(c->GetConfigRequestName());
                    m_x32->Send(c->GetConfigRequestColor());
                    PublishUiEvent(UI_EVENTS::new_track, trackstore);
                }
            }
            if (m_project.ProcessPos(trackstore, cmd, GetPosMillis())) {
                if (tv && !m_project.GetPlaying()) {
                    PublishUiEvent(UI_EVENTS::draw_trackview, tv);
                }
            }
            step_processed = true;
        }

        delete cmd;

        my_mixerqueue.pop();
    }
    //	if (!step_processed && !m_lock_daw_time) { // TODO: why this?
    //		m_project.UpdatePos(GetPosMillis());
    //	}
}

void OMainWnd::notify_mixer(OscCmd *cmd) {
    my_mixerqueue.push(cmd);
    m_MixerDispatcher.emit();
}

void OMainWnd::TimerEvent(void *data) {

}

void OMainWnd::OnViewEvent() {
    ui_event *e;
    m_new_ts_queue.front_pop(&e);
    if (e) {

        if (e->what == UI_EVENTS::new_track) {
            OTrackStore *trackstore = (OTrackStore*) e->with;
            if (!m_trackslayout.GetTrackview(trackstore->m_cmd->GetPath())) {
                OTrackView *trackview = new OTrackView(this, m_project.GetDawTime());
                trackview->SetTrackStore(trackstore);
                trackview->SetRecord(true);
                trackview->UpdateConfig();
                m_trackslayout.AddTrack(trackview);
            }
            delete e;
        }
        if (e->what == UI_EVENTS::load) {

        }
        if (e->what == UI_EVENTS::draw_trackview) {
            ((OTrackView*) e->with)->queue_draw();
            delete e;
        }
    }
}
