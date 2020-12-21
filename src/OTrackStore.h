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
    void SetPos();
    void AddSamplePoint(track_entry* e);    
    void SaveData(const char* filepath);
    void LoadData(const char* filepath);
    

    track_entry* GetEntry(int);
    void RemoveEntry(track_entry*);
    
    bool lock_process;

    bool m_record;
    bool m_touch;
    track_entry* m_tracks;
    track_entry* m_playhead;
    OscCmd* m_cmd;
    int m_colorindex;
    
    bool m_dirty;
    
private:

};

#endif /* OTRACKSTORE_H */
