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

    void Init();
    
    void SetOscCommand(OscCmd* cmd);


    void AddSamplePoint(track_entry* e);    
    void SaveData(const char* filepath);
    void LoadData(const char* filepath);
    
    void Lock();
    void Unlock();

    track_entry* NewEntry();
    track_entry* GetEntry(int);
    void RemoveEntry(track_entry*);
    
    bool m_record = false;
    bool m_touch = false;
    track_entry* m_tracks = nullptr;
    track_entry* m_playhead = nullptr;
    OscCmd* m_cmd = nullptr;
    int m_colorindex = 0;
    bool m_expanded = true;
    int m_height = 80;
    
    bool m_dirty = false;
    
private:

    std::mutex m_mutex;
};

#endif /* OTRACKSTORE_H */
