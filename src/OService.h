/*
 Copyright 2022 Detlef Urban <onkel@paraair.de>

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

#ifndef OSERVICE_H
#define OSERVICE_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <map>
#include <vector>

#include "IOProject.h"
#include "IODawHandler.h"
#include "IOJackHandler.h"
#include "IOTrackStore.h"
#include "IOBackend.h"
#include "ODAW.h"
#include "OQueue.h"
#include "OTimer.h"

class OService : public IODawHandler, public IOTimerEvent, public IOJackHandler, public IOMessageHandler {
public:
    OService();
    OService(const OService& orig);
    virtual ~OService();

    void OnDawEvent();
    void OnTimer(void*);
    void OnJackEvent();
    void OnMessageEvent();
    
    int NewMessageCallback(IOscMessage*);
    int UpdateMessageCallback(IOscMessage*);
    void ProcessSelectMessage(int);    
    
    virtual void notify_daw(DAW_PATH path) {
        my_dawqueue.push(path);
        OnDawEvent();
    };        
    
    virtual void notify_jack(JACK_EVENT jack_event) {
        m_jackqueue.push(jack_event);
    }
    
    OConfig *GetConfig() {
        return &m_config;
    }
    
    int InitMixer();
    int InitDaw();
    int InitBackend();
    
    void StartProcessing();
    
    void SelectTrack(std::string, bool);
    void UnselectTrack();    
    
private:
    
    OConfig m_config;
    
    IOMixer* m_mixer = nullptr;
    ODAW* m_daw = nullptr;
    IOBackend* m_backend = nullptr;
    OTimer m_jackTimer;
    OTimer m_dawTimer;
    IOProject* m_project = nullptr;

    std::atomic<bool> m_active = false;
    
    std::string m_session;
    bool m_teach_mode = false;
    bool m_teach_active = false;
    bool m_playing = false;
    bool m_record = false;

    OQueue<IOscMessage*> my_messagequeue;
    OQueue<DAW_PATH> my_dawqueue;
    OQueue<JACK_EVENT> m_jackqueue;
    
    void SetRecord(bool val);
    
    void GetTrackConfig(IOTrackStore* trackstore);
    void SelectNextTrack();
    void SelectPrevTrack();
    void ToggleTrackRecord(); 
    int m_selected_track_idx = -1;
};

#endif /* OSERVICE_H */

