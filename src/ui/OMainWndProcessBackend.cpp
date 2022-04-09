/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OMainWndProcessBacckend.cpp
 * Author: onkel
 *
 * Created on 13. März 2022, 17:01
 */

#include "OMainWnd.h"
//#include "OscCmd.h"


void OMainWnd::notify_jack(JACK_EVENT jack_event) {
    m_jackqueue.push(jack_event);
    //m_JackDispatcher.emit();
}

void OMainWnd::OnJackEvent() {
    while (!m_jackqueue.empty()) {
        JACK_EVENT event;
        m_jackqueue.front_pop(&event);
        switch (event) {
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
                if (event != MTC_COMPLETE) {
                    UpdatePos(m_backend->GetMillis(), false);
                } else {
                    UpdatePos(m_backend->GetMillis(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                if (m_playhead->calc_new_pos(m_project.GetDawTime(),GetPosMillis()))
                    PublishUiEvent(UI_EVENTS::new_pos, NULL);
                
                if (m_backend->GetMidiMtc()->m_edge_sec || !m_project.GetPlaying()) {
                    m_timeview->SetTimeCode(m_backend->GetTimeCode());
                    m_backend->GetMidiMtc()->m_edge_sec = false;
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
                    IOscMessage* msg = store->GetMessage();
                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
                    my_messagequeue.push(msg);
                    m_MessageDispatcher.emit();                    
                    m_x32->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
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
            case CTL_END:
                PublishUiEvent(UI_EVENTS::end, NULL);
                break;
            case CTL_NEXT_TRACK:
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_PREV_TRACK:
                PublishUiEvent(prev_track, NULL);
                break;
            case CTL_UNSELECT:
                PublishUiEvent(UI_EVENTS::unselect, NULL);
                break;
            case CTL_TOGGLE_SOLO:
                PublishUiEvent(UI_EVENTS::toggle_solo, NULL);
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
            case CTL_WHEEL_MODE:
                m_backend->ControllerShowWheelMode();
                break;
        }
    }
}

