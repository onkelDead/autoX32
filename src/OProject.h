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

#ifndef SRC_OPROJECT_H_
#define SRC_OPROJECT_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <map>
#include <vector>

#include "IOTrackStore.h"
#include "IOProject.h"
#include "OTimer.h"

#include "IOMixer.h"

class OProject : public IOProject{
public:
    OProject();
    virtual ~OProject();

    void Save(std::string location);
    int Load(std::string location);
    void Close();
    
    void UpdatePos(int current, bool seek);
    bool PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry);
    void StopRecord();
    
    bool GetDirty();
    void SetDirty();

    bool GetPlaying();
    void SetPlaying(bool val);
    daw_range* GetTimeRange();
    int GetLoopStart();
    int GetLoopEnd();
    daw_time* GetDawTime();
    void SetMaxMillis(int max_millis);
    void SetBitRate(int);
    void LockPlayhead(bool);
    
    IOTrackStore *GetTrack(std::string);
    std::map<std::string, IOTrackStore*> GetTracks();
    
    IOTrackStore* GetTrackSelected() { return m_selectedTrack; }
    std::string GetNextTrackPath();
    std::string GetPrevTrackPath();
    IOTrackStore* SelectTrack(std::string path) {
        m_selectedTrack = GetTrack(path);
        return m_selectedTrack;
    }
    
    void UnselectTrack() { m_selectedTrack = nullptr; }
    
    void SetMixer(IOMixer*);
    
    IOTrackStore* NewTrack(IOscMessage*);    
    void RemoveTrack(std::string path);
    
    
    std::vector<std::string> m_recent_projects;

    
private:

    bool m_dirty = false;
    bool m_playing = false;
    
    daw_range m_daw_range = { 0, -1, false};
    daw_time m_daw_time = {0, 1, 1., 0, -1};
    
    bool m_lock_playhead = false;

    IOMixer* m_mixer = nullptr;
    
    std::map<std::string, IOTrackStore*> m_tracks;
    IOTrackStore* m_selectedTrack = nullptr;

    void SaveRange(xmlTextWriterPtr writer);
    void SaveZoom(xmlTextWriterPtr writer);
    void SaveCache(std::string location);
    void SaveTracks(xmlTextWriterPtr writer, std::string location);
    
    void LoadCache(std::string location);
    
    int GetInteger(xmlNodePtr node, const char* name);
};

#endif /* SRC_OPROJECT_H_ */
