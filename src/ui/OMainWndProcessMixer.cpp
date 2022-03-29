/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "OMainWnd.h"
//#include "OscCmd.h"

#if 0

void OMainWnd::notify_mixer(OscCmd *cmd) {
    printf("mixer: %s %f\n", cmd->GetPath().c_str(), cmd->GetLastFloat());
    my_mixerqueue.push(cmd);
    m_MixerDispatcher.emit();
}

void OMainWnd::OnMixerEvent() {

    bool step_processed = false;

    while (my_mixerqueue.size() > 0) {
        bool cmd_used = false;

        OscCmd *cmd = my_mixerqueue.front();
        std::string path = cmd->GetPath();

        // lookup, if this path if known
        IOTrackStore *ts = m_project.GetTrack(path);
        if (ts) { // the track is known
            OTrackView *tv = m_trackslayout.GetTrackview(path);
            //            ts->GetMessage()->GetVal(0)->SetFloat(cmd);
            tv = m_trackslayout.GetTrackview(path);
            if (tv) { // we have a trackview for it
                if (m_btn_teach->get_active()) { // trackview is configured for touch
                    tv->SetRecord(true);
                }
            }
            if (m_project.ProcessPos(ts, cmd, GetPosMillis())) {
                if (tv && !m_project.GetPlaying()) {
                    PublishUiEvent(UI_EVENTS::draw_trackview, tv);
                    m_backend->ControllerShowLevel(cmd->GetLastFloat());
                }
            }
        }
        else { // the track is not known

            cmd->Parse();

            if (cmd->IsConfig()) { // its a config path so process its data
                PublishUiEvent(UI_EVENTS::conf_track, new OscCmd(*cmd));
            }

            else if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create new track and trackview 
                PublishUiEvent(UI_EVENTS::new_track, new OscCmd(*cmd));
            }

        }

        delete cmd;

        my_mixerqueue.pop();
    }
}
#endif

void OMainWnd::OnMessageEvent() {

    while (my_messagequeue.size() > 0) {
        IOscMessage *msg = my_messagequeue.front();

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

            std::string path = msg->GetPath();

            IOTrackStore *ts = m_project.GetTrack(path);
            if (ts) { // the track is known
                OTrackView *tv = m_trackslayout.GetTrackview(path);

                ts->GetMessage()->GetVal(0)->SetFloat(msg->GetVal(0)->GetFloat());
                if (m_btn_teach->get_active()) { // trackview is configured for touch
                    tv = m_trackslayout.GetTrackview(path);
                    if (tv) { // we have a trackview for it
                        tv->SetRecord(true);
                    }

                }

                
                if (ts->IsRecording()) {
                    ts->AddEntry(GetPosMillis());
                    PublishUiEvent(UI_EVENTS::draw_trackview, tv);
                }
            }
            else { // the track is not known

                if (m_btn_teach->get_active()) { // I'm configured for teach-in, so create new track and trackview 
                    PublishUiEvent(UI_EVENTS::new_channel, msg);
                }

            }
        }
        my_messagequeue.pop();
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