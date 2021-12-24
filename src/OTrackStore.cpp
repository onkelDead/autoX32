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

#include <libgen.h>
#include "OTrackStore.h"

// c'tor
OTrackStore::OTrackStore(OscCmd *cmd) : m_cmd(cmd) {
    m_layout.m_expanded = true;
    m_layout.m_height = 80;
    m_layout.m_visible = true;
    m_layout.m_index = -1;    
    m_typechar = m_cmd->GetTypes().c_str()[0];
    m_entries = NewEntry();
    m_playhead = m_entries;
    EvalFileName();
}

OTrackStore::~OTrackStore() {
    Clear();
}

void OTrackStore::Clear() {
    Lock();
    if (m_entries) {
        while (m_entries)
        {
            track_entry* next = m_entries->next;
            delete m_entries;
            m_entries = next;
        }
        m_entries = nullptr;
        m_playhead = nullptr;
    }
    Unlock();
}

OscCmd* OTrackStore::GetOscCommand() {
    return m_cmd;
}

track_entry* OTrackStore::GetHeadEntry() {
    return m_entries;
}

track_entry* OTrackStore::GetPlayhead() {
    return m_playhead;
}

track_entry* OTrackStore::NewEntry(gint timepos) {
    track_entry* entry = new track_entry;
    memset(entry, 0, sizeof (track_entry));
    entry->time = timepos;
    entry->next = NULL;
    entry->prev = NULL;
    InternalSetCmdValue(entry);
    return entry;
}

track_entry* OTrackStore::GetEntryAtPosition(int pos) {

    if (m_entries == NULL || m_playhead == NULL) {
        return NULL;
    }

    Lock();
    track_entry *entry = InternalGetEntryAtPosition(pos);
    Unlock();
    return entry;
}

void OTrackStore::AddTimePoint(track_entry *e) {
    Lock();
    InternalAddTimePoint(e);
    m_dirty = true;
    Unlock();
}

void OTrackStore::AddEntry(OscCmd* cmd, gint timepos) {
    bool is_new = false;
    Lock();
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
        entry = InternalGetEntryAtPosition(timepos);
    }
    
    if (is_new) {
        InternalAddTimePoint(entry);
        m_playhead = entry;
    }
    else {
        InternalSetCmdValue(entry);
    }
    Unlock();
}

void OTrackStore::RemoveEntry(track_entry *entry) {
    Lock();
    InternalRemoveEntry(entry);
    Unlock();
}

track_entry* OTrackStore::UpdatePlayhead(gint current, bool jump) {
    track_entry *new_pos = nullptr;
    Lock();
    track_entry* entry = InternalGetEntryAtPosition(current);

    if (entry != m_playhead) {
        if (m_record && m_playing && !jump) {
            InternalRemoveEntry(entry);
        } else {
            new_pos = entry;
            m_playhead = entry;
        }
    }
    Unlock();
    return new_pos;
}

void OTrackStore::SaveData(const char *filepath) {
    char file[PATH_MAX];
    char* path = strdup(filepath);
    
    sprintf(file, "%s/%s", dirname((char*) path), m_file_name);
    free(path);
    FILE *io = fopen(file, "wb");
    Lock();
    track_entry *it = m_entries;
    while (it) {

        fwrite(&it->time, sizeof (it->time), 1, io);
        fwrite(&it->val, sizeof (it->val), 1, io);
        it = it->next;
    }
    fclose(io);
    Unlock();
    m_dirty = false;
}

void OTrackStore::LoadData(const char *filepath) {
    char file[PATH_MAX];
    char* path = strdup(filepath);
    
    
    sprintf(file, "%s/%s", dirname(path), m_file_name);
    free(path);
    FILE *io = fopen(file, "rb");

    Clear();
    Lock();
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
    Unlock();
    m_dirty = false;
    fclose(io);
}

gint OTrackStore::GetCountEntries() {
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
    
    while(entry && entry->next) {
        if (entry->time >= entry->next->time) {
            printf("wrong position at %d, time:%d, val:%f, next time:%d, next val %f\n", *count, entry->time, entry->val.f, entry->next->time, entry->next->val.f);
            (*errors)++;
        }
        entry = entry->next;
        (*count)++;
    }    
}

// private functions
void OTrackStore::Lock() {
    while (!m_mutex.try_lock()) {
        printf("try lock");
    }
}

void OTrackStore::Unlock() {
    m_mutex.unlock();
}

track_entry* OTrackStore::InternalGetEntryAtPosition(gint pos) {
    bool changed = false;

    track_entry *entry = m_playhead;
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
        case 'f':
            entry->val.f = m_cmd->GetLastFloat();
            break;
        case 'i':
            entry->val.i = m_cmd->GetLastInt();
            break;
    }    
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
    int len = m_cmd->GetPath().length();
    char *s = strdup(m_cmd->GetPath().data());
    int i;
    char x[256];

    for (i = 0; i < len; i++) {
        x[i] = s[i] == '/' ? '_' : s[i];
    }
    x[i] = '\0';

    free(s);
    sprintf(m_file_name, "%s.dat", x);
}