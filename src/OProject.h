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

#include "OscCmd.h"
#include "OTimer.h"
#include "OTrackStore.h"
#include "IOProject.h"

#include "IOX32.h"

class OProject : public IOProject{
public:
    OProject();
    OProject(std::string location);
    virtual ~OProject();

    void New();
    void Save();
    void Load(std::string location);
    void Close();

    std::string GetProjectLocation();
    void SetProjectLocation(std::string);
    
    bool GetDirty();

    bool GetPlaying();
    daw_range* GetTimeRange();
    int GetLoopStart();
    daw_time* GetDawTime();
    void SetMaxMillis(int max_millis);
    void SetBitRate(int);
    void LockPlayhead(bool);
    
    void SetPlaying(bool val);

    virtual OscCmd* GetCommand(char* path);
    virtual void AddCommand(OscCmd*);

    void RemoveCommand(OscCmd*);
    
    OTrackStore *GetTrack(std::string);
    std::map<std::string, OTrackStore*> GetTracks();
    
    void SetMixer(IOX32*);
    
    bool UpdatePos(OTimer*);
    bool ProcessPos(OscCmd*, OTimer*);

    void PlayTrackEntry(OTrackStore* trackstore, track_entry* entry);

    OTrackStore* NewTrack(OscCmd*);
    void AddEntry(OTrackStore*, OscCmd*, int);
    
    OscCmd* ProcessConfig(OscCmd*);

    void AddRecentProject(std::string);
    
    std::vector<std::string> m_recent_projects;

private:

    bool m_dirty = false;
    
    std::string m_location;
    std::string m_projectFile;
    
    bool m_playing = false;
    
    daw_range m_daw_range;
    daw_time m_daw_time;
    
    bool m_lock_playhead = false;

    IOX32* m_mixer = nullptr;

    std::map<std::string, OscCmd*> m_known_mixer_commands;
    std::map<std::string, OTrackStore*> m_tracks;

    void SaveRange(xmlTextWriterPtr writer);
    void SaveZoom(xmlTextWriterPtr writer);
    void SaveCommands(xmlTextWriterPtr writer);
    void SaveTracks(xmlTextWriterPtr writer);
};

#endif /* SRC_OPROJECT_H_ */
