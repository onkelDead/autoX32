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
    while (m_jackqueue.size() > 0) {
        JACK_EVENT event = m_jackqueue.front();
        switch (event) {
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
                if (event != MTC_COMPLETE) {
                    UpdatePos(m_backend->GetMillis(), false);
                } else {
                    m_timeview->SetTimeCode(m_backend->GetTimeCode());
                    UpdatePos(m_backend->GetMillis(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                if (m_backend->GetMidiMtc()->m_edge_sec || !m_project.GetPlaying()) {
                    m_backend->GetMidiMtc()->m_edge_sec = false;
                    PublishUiEvent(UI_EVENTS::new_pos, NULL);
                }
                break;
            case CTL_PLAY:
            case MMC_PLAY:
                PublishUiEvent(UI_EVENTS::play, NULL);
                break;
            case CTL_STOP:
            case MMC_STOP:
                PublishUiEvent(UI_EVENTS::stop, NULL);
                break;
            case MMC_RESET:
                m_daw.ShortMessage("/refresh");
                m_daw.ShortMessage("/strip/list");
                break;
            case CTL_TEACH_ON:
                PublishUiEvent(UI_EVENTS::touch_on, NULL);
                break;
            case CTL_TEACH_OFF:
                PublishUiEvent(UI_EVENTS::touch_off, NULL);
                break;
            case CTL_FADER:
                if (m_trackslayout.GetSelectedTrackView()) {
                    IOTrackStore* store = m_trackslayout.GetSelectedTrackView()->GetTrackStore();
                    OscCmd* cmd = new OscCmd(*store->GetOscCommand());
                    cmd->SetLastFloat((float) m_backend->m_fader_val / 127.);
                    my_mixerqueue.push(cmd);
                }

                break;
            case CTL_TOUCH_RELEASE:
                PublishUiEvent(UI_EVENTS::touch_release, NULL);
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
                PublishUiEvent(UI_EVENTS::home, NULL);
                break;
            case CTL_NEXT_TRACK:
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_PREV_TRACK:
                PublishUiEvent(prev_track, NULL);
                break;
            case CTL_SCRUB_ON:
                m_backend->m_scrub = !m_backend->m_scrub;
                m_backend->ControllerShowScrub();
                break;
            case CTL_SCRUB_OFF:
                break;
            case CTL_JUMP_FORWARD:
                PublishUiEvent(UI_EVENTS::jump_forward, NULL);
                break;
            case CTL_JUMP_BACKWARD:
                PublishUiEvent(UI_EVENTS::jump_backward, NULL);
                break;
        }
        m_jackqueue.pop();
    }
}

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

void OMainWnd::notify_jack(JACK_EVENT jack_event) {
    m_jackqueue.push(jack_event);
    //m_JackDispatcher.emit();
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
        std::string path = cmd->GetPath();
        //cmd->Parse();
        int d = path.find("/config/");
        if (d > 0) {
            OscCmd *c = new OscCmd(*cmd);
            PublishUiEvent(UI_EVENTS::conf_track, c);
        } else
            if (path.at(1) != '-') { // skip status messages

            // is this track already known ?
            IOTrackStore *trackstore = m_project.GetTrack(path);
            OTrackView *tv = NULL;

            if (trackstore) { // the track is known
                trackstore->GetOscCommand()->CopyLastVal(cmd);
                tv = m_trackslayout.GetTrackview(path);
                if (tv) { // we have a trackview for it
                    if (m_btn_teach->get_active()) { // trackview is configured for touch
                        tv->SetRecord(true);
                    }
                }
            } else { // the track is not known
                if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create track and trackview
                    OscCmd *c = new OscCmd(*cmd);
                    PublishUiEvent(UI_EVENTS::new_track, c);
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
}

void OMainWnd::notify_mixer(OscCmd *cmd) {
    my_mixerqueue.push(cmd);
    //m_MixerDispatcher.emit();
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
                cmd->Parse();
                m_x32->Send(cmd->GetConfigRequestName());
                m_x32->Send(cmd->GetConfigRequestColor());
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
                on_button_back_clicked();
                break;
            case UI_EVENTS::next_track:
                SelectTrack(m_trackslayout.GetNextTrack(), true);
                break;
            case UI_EVENTS::prev_track:
                SelectTrack(m_trackslayout.GetPrevTrack(), true);
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
                OTrackView *tv = m_trackslayout.GetTrackview(c->GetPath());
                if (tv) {
                    tv->UpdateConfig();
                }
            }
                break;
        }
    }
}
