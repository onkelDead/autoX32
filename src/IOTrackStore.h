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

#include "IOscMessage.h"
#include "IOMessageHandler.h"
#include "OTypes.h"


class IOTrackView;

class IOTrackStore {
public:
    virtual ~IOTrackStore(){}
    virtual track_entry *GetHeadEntry() = 0;
    
    virtual void SetView(void* view) = 0;
    virtual void* GetView() const = 0;
    
    virtual void SetMessage(IOscMessage* message) = 0;
    virtual IOscMessage* GetMessage() = 0;
    
    virtual int ProcessMsg(IOscMessage*, int) = 0;
    
    virtual bool IsPlaying() = 0;
    virtual void SetPlaying(bool val) = 0;
    virtual bool IsRecording() = 0;
    virtual bool SetRecording(bool val) = 0;
    virtual bool ToggleRecord() = 0;
    virtual bool IsDirty() = 0;
    virtual void SetDirty(bool val) = 0;
    
    virtual track_entry* GetPlayhead() = 0;
    virtual void SetPlayhead(track_entry* e) = 0;
    virtual track_entry* GetEntryAtPosition(int pos, bool seek) = 0;
    
    virtual track_entry* UpdatePos(int current, bool jump) = 0;
    
    virtual void AddEntry(int) = 0;
    virtual void RemoveEntry(track_entry *entry) = 0; 
    virtual int GetCountEntries() = 0;
    virtual track_layout* GetLayout() = 0;
    
    virtual void SaveData(const char* filepath) = 0;
    virtual void LoadData(const char* filepath) = 0;
    
    virtual void CheckData(int* count, int* errors) = 0; 
    
    virtual std::string GetConfigRequestName() = 0;
    virtual std::string GetConfigRequestColor() = 0;
    virtual void SetColor_index(int color_index) = 0;
    virtual int GetColor_index() const = 0;
    virtual std::string GetName() = 0;
    virtual void SetName(std::string name) = 0;
    virtual std::string GetPath() = 0;
};

#endif /* IOTRACKSTORE_H */

