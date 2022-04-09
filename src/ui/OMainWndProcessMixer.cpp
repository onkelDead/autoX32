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

        if (msg->GetTrackstore()) {
            IOTrackStore* ts = msg->GetTrackstore();
            
            if (ts->ProcessMsg(msg, GetPosMillis())) {
                PublishUiEvent(UI_EVENTS::draw_trackview, ((OTrackView*)ts->GetView()));
            }
            if (ts->GetView()->GetSelected()) {
                m_backend->ControllerShowLevel(msg->GetVal(0)->GetFloat());
            }
        }
        else {
            if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create new track and trackview 
                PublishUiEvent(UI_EVENTS::new_channel, msg);
            }
        }
    }
}

int OMainWnd::NewMessageCallback(IOscMessage* msg) {
    std::cout << "OMainWnd::NewMessageCallback: called." << std::endl;
    my_messagequeue.push(msg);
    m_MessageDispatcher.emit();
    return 0;
}

int OMainWnd::UpdateMessageCallback(IOscMessage* msg) {
    my_messagequeue.push(msg);
    m_MessageDispatcher.emit();
    return 0;
}