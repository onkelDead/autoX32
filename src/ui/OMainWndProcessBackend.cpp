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

void OMainWnd::notify_jack(JACK_EVENT jack_event) {
    m_jackqueue.push(jack_event);
}

void OMainWnd::OnJackEvent() {
    while (!m_jackqueue.empty()) {
        JACK_EVENT event;
        m_jackqueue.front_pop(&event);
        switch (event) {
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
            {
                IOTrackStore* sel_ts = nullptr;
                if (event != MTC_COMPLETE) {
                    sel_ts = m_project->UpdatePos(m_backend->GetFrame(), false);
                } else {
                    sel_ts = m_project->UpdatePos(m_backend->GetFrame(), true);
                    m_backend->ControlerShowMtcComplete(0);
                }
                if (sel_ts != nullptr) {
                    m_backend->ControllerShowLevel(sel_ts->GetPlayhead()->val.f);
                }
                PublishUiEvent(E_OPERATION::new_pos, NULL);
            }      
               break;
            case MMC_PLAY:
                PublishUiEvent(E_OPERATION::play, NULL);
                break;
            case CTL_PLAY:
                if (m_project->GetPlaying()) {
                    PublishUiEvent(E_OPERATION::stop, NULL);
                }
                else {
                    PublishUiEvent(E_OPERATION::play, NULL);
                }
                break;
            case CTL_STOP:
            case MMC_STOP:
                PublishUiEvent(E_OPERATION::stop, NULL);
                break;
            case MMC_RESET:
                m_daw->ShortMessage("/refresh");
                m_daw->ShortMessage("/strip/list");
                break;
            case CTL_TEACH_ON:
                PublishUiEvent(E_OPERATION::touch_on, NULL);
                break;
            case CTL_TEACH_OFF:
                PublishUiEvent(E_OPERATION::touch_off, NULL);
                break;
            case CTL_FADER:
            {
                IOTrackStore* store = m_project->GetTrackSelected();
                if (store) {
                    if (m_btn_teach->get_active() && !store->GetRecording()) {
                        store->SetRecording(true);
                    }                    
                    IOscMessage* msg = store->GetMessage();
                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
                    my_messagequeue.push(msg);
                    m_MessageDispatcher.emit();                    
                    m_mixer->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                }
            }
                break;
            case CTL_TOUCH_RELEASE:
                PublishUiEvent(E_OPERATION::touch_release, NULL);
                break;
            case CTL_TEACH_MODE:
                m_teach_mode = !m_teach_mode;
                m_backend->ControllerShowTeachMode(m_teach_mode);
                break;
            case CTL_HOME:
                PublishUiEvent(E_OPERATION::home, NULL);
                break;
            case CTL_END:
                PublishUiEvent(E_OPERATION::end, NULL);
                break;
            case CTL_NEXT_TRACK:
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_PREV_TRACK:
                PublishUiEvent(prev_track, NULL);
                break;
            case CTL_UNSELECT:
                PublishUiEvent(E_OPERATION::unselect, NULL);
                break;
            case CTL_TOGGLE_SOLO:
                PublishUiEvent(E_OPERATION::toggle_solo, NULL);
                break;
            case CTL_TOGGLE_REC:
                PublishUiEvent(E_OPERATION::toggle_rec, NULL);
                break;
            case CTL_SCRUB_ON:
                m_backend->m_scrub = !m_backend->m_scrub;
                m_backend->ControllerShowScrub();
                break;
            case CTL_SCRUB_OFF:
                break;
            case CTL_JUMP_FORWARD:
                PublishUiEvent(E_OPERATION::jump_forward, NULL);
                break;
            case CTL_JUMP_BACKWARD:
                PublishUiEvent(E_OPERATION::jump_backward, NULL);
                break;
            case CTL_WHEEL_MODE:
                m_backend->ControllerShowWheelMode();
                break;
            case CTL_MARKER:
                m_backend->ControllerShowMarker();
                break;
            case CTL_LOOP_START:
                on_btn_loop_start_clicked();
                break;
            case CTL_LOOP_END:
                on_btn_loop_end_clicked();
                break;
            case CTL_LOOP:
                m_backend->ControllerShowCycle();
                m_daw->ShortMessage("/loop_toggle");
                break;
            case CTL_DROP_TRACK:
                if (m_project->GetTrackSelected()) {
                    PublishUiEvent(E_OPERATION::drop_track, NULL);
                }
                break;
            case CTL_KNOB:
                if (m_backend->m_drop_mode) {
                    IOTrackStore* sts = m_project->GetTrackSelected();
                    if (sts != nullptr) {
                        UnselectTrack();
                        remove_track(sts->GetPath());
                    }
                }
                break;
            default:
                std::cout << "uncaught jack event id:" << event << std::endl;
                break;
        }
    }
}

