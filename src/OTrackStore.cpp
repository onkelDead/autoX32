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
    EvalFileName();
}

OTrackStore::~OTrackStore() {
    Clear();
}

void OTrackStore::Clear() {
    Lock();
    if (m_entries) {
        while (m_entries) {
            track_entry* next = m_entries->next;
            delete m_entries;
            m_entries = next;
        }
        m_entries = nullptr;
        m_playhead = nullptr;
    }
    Unlock();
}

track_entry* OTrackStore::NewEntry(int timepos) {
    track_entry* entry = new track_entry;
    memset(entry, 0, sizeof (track_entry));
    entry->time = timepos;
    entry->next = NULL;
    entry->prev = NULL;
    InternalSetCmdValue(entry);
    return entry;
}

int OTrackStore::ProcessMsg(IOscMessage* msg, int timepos) {
    if (msg == GetMessage() && m_record) {
        AddEntry(timepos);
        return 1;
    }
    if (msg->GetPath() == m_config_name_path) {
        m_view->SetTrackName(msg->GetVal(0)->GetString());
        return 2;
    }
    if (msg->GetPath() == m_config_color_path) {
        m_color_index = msg->GetVal(0)->GetInteger();
        m_view->SetTrackColor(m_color_index);
        return 3;
    }
    return false;
}

void OTrackStore::AddEntry(int timepos) {
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
        entry = InternalGetEntryAtPosition(timepos, true);
    }

    if (is_new) {
        InternalAddTimePoint(entry);
        m_playhead = entry;
    } else {
        InternalSetCmdValue(entry);
    }
    Unlock();
}

void OTrackStore::RemoveEntry(track_entry *entry) {
    Lock();
    InternalRemoveEntry(entry);
    Unlock();
}

track_entry* OTrackStore::GetEntryAtPosition(int pos, bool seek) {
    return InternalGetEntryAtPosition(pos, seek);
}

void OTrackStore::SetPlayhead(track_entry* e) {
    Lock();
    m_playhead = e;
    Unlock();
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

    if (io) {
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

// private functions

void OTrackStore::Lock() {
    while (!m_mutex.try_lock()) {
        printf("try lock");
    }
}

void OTrackStore::Unlock() {
    m_mutex.unlock();
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
    if (m_message) {
        switch (m_message->GetVal(0)->GetType()) {
            case 'i':
                entry->val.i = m_message->GetVal(0)->GetInteger();
                break;
            case 'f':
                entry->val.f = m_message->GetVal(0)->GetFloat();
                break;
        }
        return;
    }
    //    switch (m_typechar) {
    //        case 'f':
    //            entry->val.f = m_cmd->GetLastFloat();
    //            break;
    //        case 'i':
    //            entry->val.i = m_cmd->GetLastInt();
    //            break;
    //    }    
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
    if (m_message) {
        len = m_message->GetPath().length();
        s = strdup(m_message->GetPath().data());
    }
    //    else {
    //        len = m_cmd->GetPath().length();
    //        s = strdup(m_cmd->GetPath().data());
    //        
    //    }
    int i;
    char x[256];

    for (i = 0; i < len; i++) {
        x[i] = s[i] == '/' ? '_' : s[i];
    }
    x[i] = '\0';

    free(s);
    sprintf(m_file_name, "%s.dat", x);
}
