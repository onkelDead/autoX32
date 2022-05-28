/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "OMainWnd.h"


void OMainWnd::OnMessageEvent() {

    while (!my_messagequeue.empty()) {
        IOscMessage *msg;
        my_messagequeue.front_pop(&msg);

        IOTrackStore* ts = msg->GetTrackstore();
        if (ts) {
            IOTrackStore* ts = msg->GetTrackstore();
            int upd = 0;
            IOTrackView * view = m_trackslayout.GetTrackview(ts->GetMessage()->GetPath());
            
            if ((upd = ts->ProcessMsg(msg, GetPosMillis()))) {
                PublishUiEvent(E_OPERATION::draw_trackview, ((OTrackView*)ts->GetView()));
                //PublishUiEvent(E_OPERATION::draw_trackview, view);
            }
            if (view->GetSelected()) {
                switch(upd) {
                    case 1:
                        m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
                        break;
                    case 2:
                    case 3:
                        m_backend->ControllerShowLCDName(view->GetTrackName(), ts->GetColor_index());
                        break;
                }
            }
        }
        else {
            if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create new track and trackview 
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

    IOTrackView* tv = m_trackslayout.GetTrackview(path);
    IOTrackView* st = m_trackslayout.GetTrackSelected();
    if (st)
        SelectTrack(st->GetPath(), false);
    if (tv) {
        SelectTrack(path, true);
    }
    return;
}