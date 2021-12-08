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

#ifndef IOTRACKSTORE_H
#define IOTRACKSTORE_H

#include "OTypes.h"
#include "OscCmd.h"


class IOTrackStore {
public:
    virtual track_entry *GetTracks() = 0;
    virtual OscCmd* GetOscCommand() = 0;
    virtual bool IsPlaying() = 0;
    virtual bool IsRecording() = 0;
    virtual void SetPlaying(bool val) = 0;
    virtual void SetRecording(bool val) = 0;
    
    virtual track_entry* UpdatePlayhead(gint, bool) = 0;
    
    virtual bool IsDirty() = 0;
    virtual void SetDirty(bool val) = 0;
    
    virtual void AddEntry(OscCmd*, gint) = 0;
    virtual gint GetCountEntries() = 0;
    virtual track_layout* GetLayout() = 0;
    
    virtual void SaveData(const char* filepath) = 0;
    virtual void LoadData(const char* filepath) = 0;
};

#endif /* IOTRACKSTORE_H */

