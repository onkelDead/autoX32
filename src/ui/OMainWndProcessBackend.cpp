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
                EngineLocate(event != MTC_COMPLETE);
                PublishUiEvent(E_OPERATION::new_pos, NULL);
            }     
                break;
            case MMC_PLAY:
                EnginePlay();
                PublishUiEvent(E_OPERATION::play, NULL);
                break;
            case CTL_PLAY:
                if (m_playing) {
                    EngineStop();
                    PublishUiEvent(E_OPERATION::stop, NULL);
                }
                else {
                    EnginePlay();
                    PublishUiEvent(E_OPERATION::play, NULL);
                }
                break;
            case CTL_STOP:
            case MMC_STOP:
                EngineStop();
                PublishUiEvent(E_OPERATION::stop, NULL);
                break;
            case MMC_RESET:
                m_daw->ShortMessage("/refresh");
                m_daw->ShortMessage("/strip/list");
                break;
            case CTL_TEACH_PRESS:
                EngineTeach(true);
                PublishUiEvent(E_OPERATION::teach, NULL);
                break;
            case CTL_TEACH_RELEASE:
                EngineTeach(false);
                PublishUiEvent(E_OPERATION::teach, NULL);
                break;
            case CTL_FADER:
                EngineFader();
                break;
            case CTL_TOUCH_RELEASE:
                if (sts != nullptr && sts->IsRecording()) {
                    sts->SetRecording(false);
                    m_backend->ControllerShowRec(false);
                }
                PublishUiEvent(E_OPERATION::touch_release, NULL);
                break;
            case CTL_TEACH_MODE:
                EngineTeachMode();
                PublishUiEvent(E_OPERATION::touch_off, NULL);
                break;
            case CTL_STEP_MODE:
                EngineStepMode();
                break;               
            case CTL_HOME:
                EngineHome();
                break;
            case CTL_END:
                EngineEnd();
                break;
            case CTL_NEXT_TRACK:
                EngineSelectNextTrack();
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_PREV_TRACK:
                EngineSelectPrevTrack();                
                PublishUiEvent(next_track, NULL);
                break;
            case CTL_UNSELECT:
                UnselectTrack();
                PublishUiEvent(E_OPERATION::unselect, NULL);
                break;
            case CTL_TOGGLE_REC:
                EngineToggleTrackRecord();
                PublishUiEvent(E_OPERATION::toggle_rec, NULL);
                break;
            case CTL_WHEEL_LEFT:
                EngineWheelLeft();
                if (m_wheel_mode) PublishUiEvent(next_track, NULL);
                break;
            case CTL_WHEEL_RIGHT:
                EngineWheelRight();
                if (m_wheel_mode) PublishUiEvent(next_track, NULL);
                break;
            case CTL_WHEEL_MODE:
                EngineWheelMode();
                break;
            case CTL_MARKER_PRESS:
                EngineMarker(true);
                break;
            case CTL_MARKER_RELEASE:
                EngineMarker(false);
                break;
            case CTL_LOOP_START:
                on_btn_loop_start_clicked();
                break;
            case CTL_LOOP_END:
                on_btn_loop_end_clicked();
                break;
            case CTL_LOOP:
                EngineCycle();
                break;
            default:
                std::cout << "uncaught jack event id:" << event << std::endl;
                break;
        }
    }
}

