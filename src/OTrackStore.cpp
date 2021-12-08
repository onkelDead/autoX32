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

OTrackStore::OTrackStore() {
}

OTrackStore::OTrackStore(OscCmd *cmd) : m_cmd(cmd) {
    Init();
}

OTrackStore::~OTrackStore() {
    Clear();
}

void OTrackStore::Init() {
    Lock();
    m_tracks = NewEntry();
    m_tracks->next = NULL;
    m_tracks->prev = NULL;
    m_tracks->time = 0;
    switch (m_cmd->GetTypes().data()[0]) {
        case 'f':
            m_tracks->val.f = m_cmd->GetLastFloat();
            break;
        case 'i':
            m_tracks->val.i = m_cmd->GetLastInt();
            break;
    }
    m_playhead = m_tracks;
    Unlock();
}

void OTrackStore::Clear() {
    Lock();
    if (m_tracks) {
        while (m_tracks)
        {
            track_entry *next = m_tracks->next;
            delete m_tracks;
            m_tracks = next;
        }
        m_tracks = nullptr;
        m_playhead = nullptr;
    }
    Unlock();
}

void OTrackStore::Lock() {
    while (!m_mutex.try_lock()) {
        printf("try lock");
    }
}

void OTrackStore::Unlock() {
    m_mutex.unlock();
}

OscCmd* OTrackStore::GetOscCommand() {
    return m_cmd;
}

track_entry* OTrackStore::NewEntry(gint timepos) {
    track_entry* entry = new track_entry;
    memset(entry, 0, sizeof (track_entry));
    entry->time = timepos;
    entry->next = NULL;
    entry->prev = NULL;
    return entry;
}

track_entry* OTrackStore::GetEntry(int pos) {

    if (m_tracks == NULL || m_playhead == NULL) {
        return NULL;
    }

    Lock();
    track_entry *entry = GetEntryInternal(pos);
    Unlock();
    return entry;
}

track_entry* OTrackStore::GetEntryInternal(gint pos) {
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

void OTrackStore::AddTimePoint(track_entry *e) {
    Lock();
    AddtimePointInternal(e);
    m_dirty = true;
    Unlock();
}

void OTrackStore::AddtimePointInternal(track_entry *e) {
    if (m_tracks == NULL) {
        m_tracks = e;
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

void OTrackStore::AddEntry(OscCmd* cmd, gint timepos) {
    bool is_new = false;
    if (m_record) {
        Lock();
        track_entry *entry = NULL;
        if (m_playing) {
            if (timepos != m_playhead->time) {
                entry = NewEntry();
                entry->time = timepos;
                is_new = true;
            } else {
                entry = m_playhead;
            }
        } else {
            entry = GetEntryInternal(timepos);
        }

        if (entry) {
            switch (cmd->GetTypes().data()[0]) {
                case 'f':
                    entry->val.f = cmd->GetLastFloat();
                    entry->delta.f = cmd->GetLastFloat() - ((entry->prev != NULL) ? entry->prev->val.f : 1.0);
                    break;
                case 'i':
                    entry->val.i = cmd->GetLastInt();
                    entry->delta.i = cmd->GetLastInt() - ((entry->prev != NULL) ? entry->prev->val.i : 1);
                    break;
            }
            m_dirty = true;
        }
        if (is_new) {
            AddtimePointInternal(entry);
            m_playhead = entry;
        }
        Unlock();
    }
}

void OTrackStore::RemoveEntry(track_entry *entry) {
    Lock();
    RemoveEntryInternal(entry);
    Unlock();
}

void OTrackStore::RemoveEntryInternal(track_entry *entry) {
    if (entry->prev)
        entry->prev->next = entry->next;
    if (entry->next)
        entry->next->prev = entry->prev;
    entry->time = -1;
    delete entry;
    m_dirty = true;
}

track_entry* OTrackStore::GetPlayhead() {
    return m_playhead;
}

track_entry* OTrackStore::UpdatePlayhead(gint current, bool jump) {
    track_entry *new_pos = nullptr;
    track_entry* entry = GetEntry(current);
    Lock();
    if (entry != m_playhead) {
        if (m_record && m_playing && !jump) {
            RemoveEntryInternal(entry);
        } else {
            new_pos = entry;
            m_playhead = entry;
        }
    }
    Unlock();
    return new_pos;
}

void OTrackStore::SaveData(const char *filepath) {
    std::string oscpath = m_cmd->GetPath();
    int i;
    int l = oscpath.length();
    char *s = strdup(oscpath.data());
    char x[256];
    char file[PATH_MAX];
    char *fp = strdup(filepath);

    for (i = 0; i < l; i++) {
        x[i] = s[i] == '/' ? '_' : s[i];
    }
    x[i] = '\0';

    sprintf(file, "%s/%s.dat", dirname((char*) fp), x);

    FILE *io = fopen(file, "wb");
    Lock();
    track_entry *it = m_tracks;
    while (it) {

        fwrite(&it->time, sizeof (it->time), 1, io);
        fwrite(&it->val, sizeof (it->val), 1, io);
        fwrite(&it->delta, sizeof (it->delta), 1, io);
        it = it->next;
    }
    fclose(io);
    free(fp);
    free(s);
    Unlock();
    m_dirty = false;
}

void OTrackStore::LoadData(const char *filepath) {
    std::string oscpath = m_cmd->GetPath();
    int i;
    int l = oscpath.length();
    char *s = strdup(oscpath.data());
    char x[256];
    char file[PATH_MAX];
    char *fp = strdup(filepath);

    for (i = 0; i < l; i++) {
        x[i] = s[i] == '/' ? '_' : s[i];
    }
    x[i] = '\0';
    free(s);

    sprintf(file, "%s/%s.dat", dirname((char*) fp), x);
    free(fp);
    FILE *io = fopen(file, "rb");

    Clear();
    Lock();
    while (!feof(io)) {
        size_t s;
        track_entry *it = NewEntry();
        it->prev = 0;
        it->next = 0;

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
        s = fread(&it->delta, sizeof (it->delta), 1, io);
        if (s != 1) {
            delete it;
            break;
        }
        AddtimePointInternal(it);
    }
    Unlock();
    m_dirty = false;
    fclose(io);
}

gint OTrackStore::GetCountEntries() {
    gint c = 0;
    track_entry *e = m_tracks;
    while (e) {
        e = e->next;
        c++;
    }
    return c;
}