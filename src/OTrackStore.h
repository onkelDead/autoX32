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
#include "IOMessageHandler.h"
#include "IOscMessage.h"
#include "IOTrackStore.h"
#include "OTypes.h"

class OTrackStore : public IOTrackStore {
public:
    
    // constructor / destructor
    OTrackStore(IOscMessage *msg);
    virtual ~OTrackStore();

    // overrides
    void AddEntry(int timepos);
    void Clear();
    void RemoveEntry(track_entry*);
    track_entry* GetEntryAtPosition(int pos, bool seek);
    
    track_entry* UpdatePos(int current, bool jump);
    
    int ProcessMsg(IOscMessage*, int);
    void CheckData(int* count, int* errors);

    // serialization
    void SaveData(const char* filepath);
    void LoadData(const char* filepath);

    // getters / setters
    inline bool IsPlaying() {
        return m_playing;
    };

    inline void SetPlaying(bool val) {
        m_playing = val;
    }

    inline bool IsRecording() {
        return m_record;
    }

    inline void SetRecording(bool val) {
        m_record = val;
    }

    inline bool IsDirty() {
        return m_dirty;
    }

    inline void SetDirty(bool val) {
        m_dirty = val;
    }

    int GetCountEntries();


    inline track_entry* GetHeadEntry() {
        return m_entries;
    }

    inline track_entry* GetPlayhead() {
        return m_playhead;
    }  
    
    inline void SetPlayhead(track_entry* e);

    inline track_layout* GetLayout() {
        return &m_layout;
    }

    void SetMessage(IOscMessage* message) {
        m_message = message;
    }

    inline IOscMessage* GetMessage() {
        return m_message;
    }

    std::string GetConfigRequestName() {
        if (m_config_name_path.empty()) {
            m_config_name_path = m_message->GetConfigRequestName();
        }
        return m_config_name_path;
    }

    std::string GetConfigRequestColor() {
        if (m_config_color_path.empty()) {
            m_config_color_path = m_message->GetConfigRequestColor();
        }
        return m_config_color_path;
    }
    
    inline void SetView(void* view) {
        m_view = view;
    }

    inline void* GetView() const {
        return m_view;
    }    

    inline void SetColor_index(int color_index) {
        m_color_index = color_index;
    }

    inline int GetColor_index() const {
        return m_color_index;
    }

    std::string GetName() {
        return m_name;
    }

    void SetName(std::string name) {
        m_name = name;
    }
    
    std::string GetPath() { return m_message->GetPath(); }

private:

    char m_typechar;

    bool m_record = false;
    bool m_playing = false;

    track_entry* m_entries = nullptr;
    bool m_dirty = false;

    track_layout m_layout;

    std::mutex m_mutex;

    track_entry* m_playhead = nullptr;
    IOscMessage* m_message = nullptr;

    void* m_view = nullptr;

    char m_file_name[512];

    std::string m_config_name_path;
    std::string m_config_color_path;
    
    std::string m_name;
    int m_color_index;

    // private functions
    bool IsFileNameValid = false;
    void EvalFileName();

    track_entry* NewEntry(int pos = 0);
    inline void InternalRemoveEntry(track_entry*);
    inline track_entry* InternalGetEntryAtPosition(int, bool seek);
    inline void InternalAddTimePoint(track_entry*);
    inline void InternalSetCmdValue(track_entry*);

};

#endif /* OTRACKSTORE_H */
