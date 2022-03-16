/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "OMainWnd.h"
#include "OscCmd.h"

void OMainWnd::notify_mixer(OscCmd *cmd) {
    printf("mixer: %s %f\n", cmd->GetPath().c_str(), cmd->GetLastFloat());
    my_mixerqueue.push(cmd);
    //m_MixerDispatcher.emit();
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
            ts->GetOscCommand()->CopyLastVal(cmd);
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
