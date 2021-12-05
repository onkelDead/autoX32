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

#include "OTypes.h"
#include "OscCmd.h"

class OTrackStore {
public:
    OTrackStore();    
    OTrackStore(OscCmd* cmd);
    virtual ~OTrackStore();

    
    OscCmd* GetOscCommand();


    void AddTimePoint(track_entry* e);    
    void SaveData(const char* filepath);
    void LoadData(const char* filepath);
    


    track_entry* NewEntry(gint pos = 0);
    track_entry* GetEntry(int);
    
    void RemoveEntry(track_entry*);
    void AddEntry(OscCmd*, gint);
    
    track_entry* GetPlayhead();
    track_entry* UpdatePlayhead(gint, bool);
    
    gint GetCountEntries();
    
    bool m_record = false;
    bool m_playing = false;    
    track_entry* m_tracks = nullptr;
    bool m_expanded = true;
    int m_height = 80;
    
    bool m_dirty = false;

    gint m_index = -1;
    bool m_visible = true;
    
private:
    void Init();

    inline void Lock();
    inline void Unlock();    
    
    inline void RemoveEntryInternal(track_entry*);
    inline track_entry* GetEntryInternal(gint);
    inline void AddtimePointInternal(track_entry *e);
    
    std::mutex m_mutex;
    
    track_entry* m_playhead = nullptr;
    OscCmd* m_cmd = nullptr;
   
};

#endif /* OTRACKSTORE_H */
