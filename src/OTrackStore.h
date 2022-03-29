/*
  Copyright 2020 Detlef Urban <onkel@paraair.de>

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

#ifndef OTRACKSTORE_H
#define OTRACKSTORE_H

#include <mutex>
#include "IOTrackView.h"
#include "IOMessageHandler.h"
#include "IOscMessage.h"
#include "IOTrackStore.h"
#include "OTypes.h"
//#include "OscCmd.h"

class OTrackStore : public IOTrackStore {
public:
    OTrackStore(IOscMessage *msg);
    virtual ~OTrackStore();

    track_entry *GetHeadEntry();

    void AddTimePoint(track_entry* e);    
    void SaveData(const char* filepath);
    void LoadData(const char* filepath);
    
    int NewMessageCallback(IOscMessage*);
    int UpdateMessageCallback(IOscMessage*);    
    
    void CheckData(int* count, int* errors);
    
    track_entry* NewEntry(int pos = 0);
    track_entry* GetEntryAtPosition(int);
    
    bool ProcessMsg(IOscMessage*, int);
    
    void RemoveEntry(track_entry*);
    void AddEntry(int);
    
    track_entry* GetPlayhead();
    track_entry* UpdatePlayhead(int, bool);

    inline bool IsPlaying() { return m_playing; };
    inline void SetPlaying(bool val) { m_playing = val; }
    
    inline bool IsRecording() { return m_record; }
    inline void SetRecording(bool val) { m_record = val; }

    inline bool IsDirty() { return m_dirty; }
    inline void SetDirty(bool val) { m_dirty = val; }
    
    int GetCountEntries();
    
    inline track_layout* GetLayout() { return &m_layout; }
    
    void Clear();

    void SetMessage(IOscMessage* message) {
        m_message = message;
    }

    IOscMessage* GetMessage() {
        return m_message;
    }

    std::string GetConfigRequestName() {
        m_config_name_path = m_message->GetConfigRequestName();
        return m_config_name_path;
    }
    
    std::string GetConfigRequestColor() {
        m_config_color_path = m_message->GetConfigRequestColor();
        return m_config_color_path;
    }

    void SetView(IOTrackView* view) {
        m_view = view;
    }

    IOTrackView* GetView() const {
        return m_view;
    }

    void SetColor_index(int color_index) {
        m_color_index = color_index;
    }

    int GetColor_index() const {
        return m_color_index;
    }
    
private:
    
    char m_typechar;
    
    bool m_record = false;
    bool m_playing = false;    

    track_entry* m_entries = nullptr;
    bool m_dirty = false;
    
    track_layout m_layout;

    inline void Lock();
    inline void Unlock();    
    
    inline void InternalRemoveEntry(track_entry*);
    inline track_entry* InternalGetEntryAtPosition(int);
    inline void InternalAddTimePoint(track_entry*);
    inline void InternalSetCmdValue(track_entry*);
    
    std::mutex m_mutex;
    
    track_entry* m_playhead = nullptr;
    IOscMessage* m_message = nullptr;
    
    IOTrackView* m_view = nullptr;
    
    char m_file_name[128];
    void EvalFileName();
   
    std::string m_config_name_path;
    std::string m_config_color_path;
    
    int m_color_index;
    
    
};

#endif /* OTRACKSTORE_H */
