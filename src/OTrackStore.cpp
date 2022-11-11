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

#include <iostream>
#include <libgen.h>
#include <string.h>
#include <limits.h>
#include "OTrackStore.h"

OTrackStore::OTrackStore(IOscMessage *msg) : m_message(msg) {
    m_layout.m_expanded = true;
    m_layout.m_height = 80;
    m_layout.m_visible = true;
    m_layout.m_index = -1;
    m_typechar = m_message->GetTypes()[0];
    m_entries = NewEntry();
    m_playhead = m_entries;
}

OTrackStore::~OTrackStore() {
    Clear();
}

void OTrackStore::Clear() {
    if (m_entries) {
        while (m_entries) {
            track_entry* next = m_entries->next;
            delete m_entries;
            m_entries = next;
        }
        m_entries = nullptr;
        m_playhead = nullptr;
    }
}

track_entry* OTrackStore::NewEntry(int timepos) {
    track_entry* entry = new track_entry;
  //  memset(entry, 0, sizeof (track_entry));
    entry->time = timepos;
    entry->next = NULL;
    entry->prev = NULL;
    InternalSetCmdValue(entry);
    return entry;
}

int OTrackStore::ProcessMsg(IOscMessage* msg, int timepos) {
    if (msg == GetMessage()) {
        if (m_record) {
            AddEntry(timepos);
        }
        return 1;
    }
    if (msg->GetPath() == m_config_name_path) {
        m_name = msg->GetVal(0)->GetString();
        return 2;
    }
    if (msg->GetPath() == m_config_color_path) {
        m_color_index = msg->GetVal(0)->GetInteger();
        return 3;
    }
    return 0;
}

void OTrackStore::AddEntry(int timepos) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool is_new = false;
    track_entry *entry = NULL;
    if (m_playing) {
        if (timepos != m_playhead->time) {
            entry = NewEntry(timepos);
            is_new = true;
        } else {
            entry = m_playhead;
        }
        m_dirty = true;
    } else {
        entry = InternalGetEntryAtPosition(timepos, true);
    }

    if (is_new) {
        //printf("AddEntry %d\n", timepos);
        InternalAddTimePoint(entry);
        m_playhead = entry;
    } else {
        //printf("updateEntry %d\n", timepos);
        InternalSetCmdValue(entry);
    }
}

void OTrackStore::RemoveEntry(track_entry *entry) {
    std::lock_guard<std::mutex> lock(m_mutex);
    InternalRemoveEntry(entry);
}

track_entry* OTrackStore::GetEntryAtPosition(int pos, bool seek) {
    return InternalGetEntryAtPosition(pos, seek);
}

track_entry* OTrackStore::UpdatePos(int current, bool seek) {
    track_entry* e = GetEntryAtPosition(current, seek);

    // overwrite if required
    if (IsRecording() && IsPlaying()) {
        if (GetPlayhead() != e) {
            RemoveEntry(e);
        }            
    }
    else {
        if (GetPlayhead() != e) {
            SetPlayhead(e);
        }
        return e;
    }
    return nullptr;
}

void OTrackStore::SetPlayhead(track_entry* e) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_playhead = e;
}

void OTrackStore::SaveData(const char *filepath) {
    char file[PATH_MAX];
    char* path = strdup(filepath);

    EvalFileName();

    sprintf(file, "%s/%s", path, m_file_name);
    free(path);
    FILE *io = fopen(file, "wb");
    std::lock_guard<std::mutex> lock(m_mutex);
    track_entry *it = m_entries;
    while (it) {

        fwrite(&it->time, sizeof (it->time), 1, io);
        fwrite(&it->val, sizeof (it->val), 1, io);
        it = it->next;
    }
    fclose(io);
    m_dirty = false;
}

void OTrackStore::LoadData(const char *filepath) {
    char file[PATH_MAX];
    char* path = strdup(filepath);

    EvalFileName();
    sprintf(file, "%s/%s", path, m_file_name);
    free(path);
    FILE *io = fopen(file, "rb");

    if (io) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Clear();
        while (!feof(io)) {
            size_t s;
            track_entry *it = NewEntry();
            s = fread(&it->time, sizeof (it->time), 1, io);
            if (s != 1) {
                delete it;
                break;
            }
            s = fread(&it->val, sizeof (it->val), 1, io);
            if (s != 1) {
                delete it;
                break;
            }
            InternalAddTimePoint(it);
        }
        m_dirty = false;
        fclose(io);
    }
}

int OTrackStore::GetCountEntries() {
    int counter = 0;
    track_entry *e = m_entries;
    while (e) {
        e = e->next;
        counter++;
    }
    return counter;
}

void OTrackStore::CheckData(int* count, int* errors) {
    track_entry* entry = m_entries;

    while (entry && entry->next) {
        if (entry->time >= entry->next->time) {
            printf("wrong position at %d, time:%d, val:%f, next time:%d, next val %f\n", *count, entry->time, entry->val.f, entry->next->time, entry->next->val.f);
            (*errors)++;
        }
        entry = entry->next;
        (*count)++;
    }
}

track_entry* OTrackStore::InternalGetEntryAtPosition(int pos, bool seek) {
    bool changed = false;

    track_entry *entry = m_playhead;
    if (!seek) {
        while (entry->next && entry->next->time < pos) {
           entry = entry->next;
        }
        return entry;
    }
    
    if (entry) {
        while (entry->next && entry->next->time < pos) {
            entry = entry->next;
            changed = true;
        }

        if (!changed) {
            while (entry->time > pos && entry->prev) { // backwind
                entry = entry->prev;
                if (entry->time == 0)
                    break;
            }
        }
    }
    return entry;
}

void OTrackStore::InternalAddTimePoint(track_entry *e) {
    if (m_entries == NULL) {
        m_entries = e;
        m_playhead = e;
    } else {
        e->prev = m_playhead;
        e->next = m_playhead->next;
        if (e->next)
            e->next->prev = e;
        m_playhead->next = e;
        m_playhead = e;
    }
}

void OTrackStore::InternalSetCmdValue(track_entry* entry) {
        switch (m_typechar) {
            case 'i':
                entry->val.i = m_message->GetVal(0)->GetInteger();
                break;
            case 'f':
                entry->val.f = m_message->GetVal(0)->GetFloat();
                break;
        }
        return;
}

void OTrackStore::InternalRemoveEntry(track_entry *entry) {
    if (entry->prev)
        entry->prev->next = entry->next;
    if (entry->next)
        entry->next->prev = entry->prev;
    if (m_playhead == entry)
        m_playhead = entry->next;
    delete entry;
    m_dirty = true;
}

void OTrackStore::EvalFileName() {
    int len;
    char *s;
    
    if (IsFileNameValid) return;
    
    if (m_message) {
        len = m_message->GetPath().length();
        s = strdup(m_message->GetPath().data());
    }

    int i;
    char x[256];

    for (i = 0; i < len; i++) {
        x[i] = s[i] == '/' ? '_' : s[i];
    }
    x[i] = '\0';

    free(s);
    sprintf(m_file_name, "%s.dat", x);
    IsFileNameValid = true;
}
