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

OTrackStore::OTrackStore(OscCmd *cmd) :
		m_cmd(cmd) {
	Lock();
	Init();
	Unlock();
}

OTrackStore::~OTrackStore() {
	Lock();
	do {
		track_entry *track = m_tracks->next;
		delete m_tracks;
		m_tracks = track;
	} while (m_tracks);
	Unlock();
}

void OTrackStore::Init() {
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
}

void OTrackStore::Lock() {
	m_mutex.lock();
}

void OTrackStore::Unlock() {
	m_mutex.unlock();
}

track_entry* OTrackStore::NewEntry() {
	return new track_entry;
}

track_entry* OTrackStore::GetEntry(int pos) {
	bool changed = false;

	if (m_tracks == NULL || m_playhead == NULL) {
		return NULL;
	}

	track_entry *entry = m_playhead;

	while (entry->next && entry->next->time < pos) {
		entry = entry->next;
		changed = true;
	}

	if (!changed) {
		while (entry->time > pos) { // backwind
			entry = entry->prev;
			if (entry->time == 0)
				break;
		}
	}

	return entry;
}

void OTrackStore::AddTimePoint(track_entry *e) {
	if (m_tracks == NULL) {
		m_tracks = e;
		m_playhead = e;
	} else {
		e->prev = m_playhead;
		e->next = m_playhead->next;
		if (m_playhead->next)
			e->next->prev = e;
		m_playhead->next = e;
		m_playhead = e;
	}
	m_dirty = true;
}

void OTrackStore::RemoveEntry(track_entry *entry) {
	if (entry->prev)
		entry->prev->next = entry->next;
	if (entry->next)
		entry->next->prev = entry->prev;
	printf("remove time %d\n", entry->time);
	entry->time = -1;
	delete entry;
	m_dirty = true;
}

void OTrackStore::SaveData(const char *filepath) {
	std::string oscpath = m_cmd->GetPath();
	int i;
	int l = oscpath.length();
	char *s = strdup(oscpath.data());
	char x[l + 1];
	char file[PATH_MAX];
	char *fp = strdup(filepath);

	for (i = 0; i < l; i++) {
		x[i] = s[i] == '/' ? '_' : s[i];
	}
	x[i] = '\0';

	sprintf(file, "%s/%s.dat", dirname((char*) fp), x);

	FILE *io = fopen(file, "wb");

	track_entry *it = m_tracks;
	while (it) {

		fwrite(&it->time, sizeof(it->time), 1, io);
		fwrite(&it->val, sizeof(it->val), 1, io);
		it = it->next;
	}
	fclose(io);
	free(fp);
	m_dirty = false;
}

void OTrackStore::LoadData(const char *filepath) {
	std::string oscpath = m_cmd->GetPath();
	int i;
	int l = oscpath.length();
	char *s = strdup(oscpath.data());
	char x[l + 1];
	char file[PATH_MAX];
	char *fp = strdup(filepath);

	for (i = 0; i < l; i++) {
		x[i] = s[i] == '/' ? '_' : s[i];
	}
	x[i] = '\0';

	sprintf(file, "%s/%s.dat", dirname((char*) fp), x);
	FILE *io = fopen(file, "rb");

	while (!feof(io)) {
		size_t s;
		track_entry *it = NewEntry();
		it->prev = 0;
		it->next = 0;

		s = fread(&it->time, sizeof(it->time), 1, io);
		if (s != 1)
			break;
		s = fread(&it->val, sizeof(it->val), 1, io);
		if (s != 1)
			break;

		AddTimePoint(it);
	}
	m_dirty = false;
	fclose(io);
}
