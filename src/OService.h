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

class OService : public IOProject, public IODawHandler, public IOJackHandler, public IOTimerEvent, public IOMessageHandler {
public:
    OService();
    OService(const OService& orig);
    virtual ~OService();
    
    bool CheckArdourRecent();
    void Save();
    void Load(std::string location);
    void Close();

    IOTrackStore* NewTrack(IOscMessage*);    

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
    
    void SetMixer(IOMixer* mixer) {
        m_mixer = mixer;
    }

    void SetDaw(ODAW* daw) {
        m_daw = daw;
    }
    
    void SetLocation(std::string location) {
        m_location = location;
    }
    void SetBackend(IOBackend* backend) {
        m_backend = backend;
    }
    
    OConfig *GetConfig() {
        return &m_config;
    }
    
    int InitMixer();
    int InitDaw();
    int InitBackend();
    
    void StartProcessing();
    
private:
    
    OConfig m_config;
    
    IOMixer* m_mixer = nullptr;
    ODAW* m_daw = nullptr;
    IOBackend* m_backend = nullptr;
    OTimer m_jackTimer;
    OTimer m_dawTimer;

    std::atomic<bool> m_active = false;
    
    std::string m_session;
    std::string m_location;
    std::string m_projectFile;
    bool m_dirty = false;
    bool m_teach_mode = false;
    bool m_teach_active = false;
    bool m_playing = false;
    bool m_record = false;

    
    daw_range m_daw_range = { 0, -1, false};
    daw_time m_daw_time = {0, 1, 1., 0, -1};    
    
    std::map<std::string, IOTrackStore*> m_tracks;    
    
    
    int GetInteger(xmlNodePtr node, const char* name);

    void SaveRange(xmlTextWriterPtr writer);
    void SaveZoom(xmlTextWriterPtr writer);
    void SaveCommands(xmlTextWriterPtr writer);
    void SaveTracks(xmlTextWriterPtr writer);
     
    void UpdatePos(int current, bool seek);
    bool PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry);
    
    OQueue<IOscMessage*> my_messagequeue;
    OQueue<DAW_PATH> my_dawqueue;
    OQueue<JACK_EVENT> m_jackqueue;
    
    void SetRecord(bool val);
    
    void GetTrackConfig(IOTrackStore* trackstore);
    IOTrackStore* m_selected_track = nullptr;
    void SelectNextTrack();
    void SelectPrevTrack();
    void UnselectTrack();
    void ToggleTrackRecord(); 
    int m_selected_track_idx = -1;
};

#endif /* OSERVICE_H */

