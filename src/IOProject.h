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

#ifndef IPROJECT_H
#define IPROJECT_H

#include <map>
#include "IOMixer.h"
#include "IOTrackStore.h"

class IOProject {
public:
    virtual ~IOProject(){}
    virtual void SetMixer(IOMixer*) = 0;
    virtual void Save(std::string location) = 0;
    virtual void SaveCache(std::string location) = 0;    
    virtual int Load(std::string location) = 0;
    virtual void LoadCache(std::string location) = 0;
    virtual void Close() = 0;
    virtual std::map<std::string, IOTrackStore*> GetTracks() = 0;
    virtual IOTrackStore *GetTrack(std::string) = 0;
    virtual IOTrackStore* NewTrack(IOscMessage*) = 0;
    virtual void RemoveTrack(std::string path) = 0;

    virtual IOTrackStore* UpdatePos(int current, bool seek) = 0;
    virtual bool PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry) = 0;
    virtual void StopRecord() = 0;
    virtual bool GetPlaying() = 0;
    virtual void SetPlaying(bool val) = 0;
    virtual bool GetDirty() = 0;
    virtual void SetDirty() = 0;
    virtual IOTrackStore* GetTrackSelected() = 0;
    virtual void UnselectTrack() = 0;
    virtual IOTrackStore* SelectTrack(std::string path) = 0;
    virtual std::string GetNextTrackPath() = 0;
    virtual std::string GetPrevTrackPath() = 0;
    virtual daw_range* GetTimeRange() = 0;
    virtual int GetLoopStart() = 0;
    virtual int GetLoopEnd() = 0;
    virtual daw_time* GetDawTime() = 0;
    virtual void SetMaxFrames(int max_frames) = 0;
    virtual void SetBitRate(int) = 0;
};

#endif /* IPROJECT_H */

