/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "OMainWnd.h"
#include <regex>

void OMainWnd::OnMessageEvent() {

    while (!my_messagequeue.empty()) {
        IOscMessage *msg;
        my_messagequeue.front_pop(&msg);

        IOTrackStore* ts = msg->GetTrackstore();
        if (ts) {
            IOTrackStore* ts = msg->GetTrackstore();
            int upd = 0;
            IOTrackView * view = m_trackslayout.GetTrackview(ts->GetPath());
            
            if ((upd = ts->ProcessMsg(msg, GetPosFrame()))) {
                PublishUiEvent(E_OPERATION::draw_trackview, ((OTrackView*)ts->GetView()));
            }
            switch(upd) {
                case 2:
                    view->SetTrackName(ts->GetName());
                    break;
            }
            if (view->GetSelected()) {
                switch(upd) {
                    case 1:
                        m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                        break;
                    case 2:
                    case 3:
                        m_backend->ControllerShowLCDName(ts->GetName(), ts->GetColor_index());
                        break;
                }
            }
        }
        else {
            if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create new track and trackview 
                if (std::regex_match (msg->GetPath(), std::regex(m_config.get_string(SETTINGS_TRACK_FILTER)) ))
                    PublishUiEvent(E_OPERATION::new_channel, msg);
            }
        }
    }
}

int OMainWnd::NewMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    m_MessageDispatcher.emit();
    return 0;
}

int OMainWnd::UpdateMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    m_MessageDispatcher.emit();
    return 0;
}

void OMainWnd::ProcessSelectMessage(int idx) {
    char path[32];
    
    sprintf(path, "/ch/%02d/mix/fader", idx + 1);

    SelectTrack(path, true);    
    return;
}