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
    IOTrackStore* sts = m_project->GetTrackSelected();
    while (!m_jackqueue.empty()) {
        JACK_EVENT event;
        m_jackqueue.front_pop(&event);
        switch (event) {
            case MTC_QUARTER_FRAME:
            case MTC_COMPLETE:
            {
                Locate(event != MTC_COMPLETE);
                PublishUiEvent(E_OPERATION::new_pos, NULL);
            }     
                break;
            case MMC_PLAY:
                Play();
                PublishUiEvent(E_OPERATION::play, NULL);
                break;
            case CTL_PLAY:
                if (m_playing) {
                    Stop();
                    PublishUiEvent(E_OPERATION::stop, NULL);
                }
                else {
                    Play();
                    PublishUiEvent(E_OPERATION::play, NULL);
                }
                break;
            case CTL_STOP:
            case MMC_STOP:
                Stop();
                PublishUiEvent(E_OPERATION::stop, NULL);
                break;
            case MMC_RESET:
                m_daw->ShortMessage("/refresh");
                m_daw->ShortMessage("/strip/list");
                break;
            case CTL_TEACH_PRESS:
                Teach(true);
                PublishUiEvent(E_OPERATION::teach, NULL);
                break;
            case CTL_TEACH_RELEASE:
                Teach(false);
                PublishUiEvent(E_OPERATION::teach, NULL);
                break;
            case CTL_FADER:
                if (sts) {
                    if (m_teach_active && !sts->GetRecording()) {
                        sts->SetRecording(true);
                    }                    
                    IOscMessage* msg = sts->GetMessage();
                    msg->GetVal(0)->SetFloat((float) m_backend->m_fader_val / 127.);
//                    my_messagequeue.push(msg);
//                    m_MessageDispatcher.emit();                    
                    if (sts->GetRecording()) {
                        m_mixer->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                        sts->ProcessMsg(msg, GetPosFrame());
                    }
                    m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                }
                break;
            case CTL_TOUCH_RELEASE:
                if (sts != nullptr && sts->IsRecording()) {
                    sts->SetRecording(false);
                    m_backend->ControllerShowRec(false);
                }
                PublishUiEvent(E_OPERATION::touch_release, NULL);
                break;
            case CTL_TEACH_MODE:
                TeachMode();
                PublishUiEvent(E_OPERATION::touch_off, NULL);
                break;
            case CTL_STEP_MODE:
                m_backend->m_step_mode = !m_backend->m_step_mode;
                m_backend->ControllerShowStepMode(m_backend->m_step_mode);
                break;               
            case CTL_HOME:
                Home();
                break;
            case CTL_END:
                End();
                break;
            case CTL_NEXT_TRACK:
                SelectNextTrack();
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_PREV_TRACK:
                SelectPrevTrack();                
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_UNSELECT:
                UnselectTrack();
                PublishUiEvent(E_OPERATION::unselect, NULL);
                break;
            case CTL_TOGGLE_REC:
                ToggleTrackRecord();
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
                if (sts) {
                    PublishUiEvent(E_OPERATION::drop_track, NULL);
                }
                break;
            case CTL_KNOB:
                if (m_backend->m_drop_mode) {
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

