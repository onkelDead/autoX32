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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "OProject.h"
#include "OTrackStore.h"
#include "IOX32.h"

OProject::OProject() {
    m_daw_range.m_loopend = -1;
    m_daw_range.m_loopstart = 0;
    m_daw_range.m_dirty = false;
    m_daw_time.m_bitrate = 0;
    m_daw_time.m_maxmillis = 1;
    m_daw_time.scale = 1;
    m_daw_time.m_viewstart = 0;
    m_daw_time.m_viewend = -1;
}

OProject::OProject(std::string location) {

    m_daw_time.m_bitrate = 0;
    m_daw_time.m_maxmillis = 1;
    m_daw_time.scale = 1;
    m_daw_time.m_viewstart = 0;
    m_daw_time.m_viewend = -1;

    std::string name = basename(location.data());

    m_projectFile = location;
    m_projectFile.append("/").append(name.append(".xml").data());
}

OProject::~OProject() {
}

void OProject::SetMixer(IOX32* mixer) {
    m_mixer = mixer;
}

bool OProject::GetDirty() {
    bool dirty = m_dirty;
    
    dirty |= m_daw_range.m_dirty;
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        dirty |= it->second->m_dirty;
    }
    return dirty;
}

bool OProject::GetPlaying() {
	return m_playing;
}

std::string OProject::GetProjectLocation() {
    return m_location;
}

void OProject::SetProjectLocation(std::string new_location) {
    m_location = new_location;
}

void OProject::LockPlayhead(bool val) {
    m_lock_playhead = val;
}

void OProject::New() {
    std::string name = basename(m_location.data());

    m_projectFile = m_location;
    m_projectFile.append("/").append(name.append(".xml").data());
}

void OProject::Load(std::string location) {
    SetProjectLocation(location);
    std::string name = basename(m_location.data());

    m_projectFile = m_location;
    m_projectFile.append("/").append(name.append(".xml").data());

    xmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;
    xmlChar *keyword;

    doc = xmlParseFile(m_projectFile.data());
    context = xmlXPathNewContext(doc);
    result = xmlXPathEvalExpression(BAD_CAST "//project/range", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_daw_range.m_loopstart = atoi((const char *) xmlGetProp(node, BAD_CAST "start"));
        m_daw_range.m_loopend = atoi((const char *) xmlGetProp(node, BAD_CAST "end"));
        m_daw_range.m_dirty = false;
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/zoom", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_daw_time.m_viewstart = atoi((const char *) xmlGetProp(node, BAD_CAST "start"));
        m_daw_time.m_viewend = atoi((const char *) xmlGetProp(node, BAD_CAST "end"));
    }
    xmlXPathFreeObject(result);


    result = xmlXPathEvalExpression(BAD_CAST "//project/cmd", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        for (int i = 0; i < nodeset->nodeMax; i++) {
            if (node) {
                const char* cv;
                xmlChar *xmlPath = xmlGetProp(node, BAD_CAST "path");
                xmlChar *xmlTypes = xmlGetProp(node, BAD_CAST "types");

                if (xmlPath == nullptr || xmlTypes == nullptr)
                	continue;
                const char* path =  strdup((char*)xmlPath);
                const char* types = strdup((char*)xmlTypes);
                m_known_mixer_commands[path] = new OscCmd(path, types);
                const char* name = (char*) xmlGetProp(node, BAD_CAST "name");
                m_known_mixer_commands[path]->SetName(name);
                Gdk::RGBA color;
                color.set_rgba_u(0, 0, 0, 32768);
                cv = (char*) xmlGetProp(node, BAD_CAST "red");
                color.set_red_u(atoi(cv));
                cv = (char*) xmlGetProp(node, BAD_CAST "green");
                color.set_green_u(atoi(cv));
                cv = (char*) xmlGetProp(node, BAD_CAST "blue");
                color.set_blue_u(atoi(cv));
                cv = (char*) xmlGetProp(node, BAD_CAST "alpha");
                color.set_alpha_u(atoi(cv));
                m_known_mixer_commands[path]->SetColor(color);
            }
            else {
            	break;
            }
            node = node->next;
        }
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/track", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        for (int i = 0; i < nodeset->nodeMax; i++) {
            if (node) {
                const char* path = (char*) xmlGetProp(node, BAD_CAST "path");
                const char* expanded = (char*) xmlGetProp(node, BAD_CAST "expand");
                const char* height = (char*) xmlGetProp(node, BAD_CAST "height");

                OscCmd* cmd = m_known_mixer_commands[path];
                OTrackStore *ts = NewTrack(cmd);
                ts->Lock();
                ts->m_expanded = atoi(expanded);
                ts->m_height = atoi(height);
                ts->LoadData(m_projectFile.data());
                ts->Unlock();
            }
            else {
            	break;
            }
            node = node->next;
        }
    }
    xmlXPathFreeObject(result);

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
}

void OProject::Save() {
    std::string name = basename(m_location.data());

    m_projectFile = m_location;
    m_projectFile.append("/").append(name.append(".xml").data());
    
    xmlTextWriterPtr writer;

    writer = xmlNewTextWriterFilename(m_projectFile.data(), 0);
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    xmlTextWriterStartElement(writer, BAD_CAST "project");
    SaveRange(writer);
    SaveZoom(writer);
    SaveCommands(writer);
    SaveTracks(writer);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
    m_dirty = false;
}

void OProject::Close() {
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete it->second;
    }    
    for (std::map<std::string, OscCmd*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
        delete it->second;
    }
    m_tracks.clear();
    m_known_mixer_commands.clear();
    m_location = "";
    m_projectFile = "";
    m_daw_range.m_dirty = false;
}

void OProject::AddRecentProject(std::string location) {
    bool exit = false;
    for (std::vector<std::string>::iterator it = m_recent_projects.begin(); it != m_recent_projects.end(); ++it) {
        if (exit)
            break;
        if (*it == location) {
            exit = true;
            m_recent_projects.erase(it);
        }
    }
    
    m_recent_projects.insert(m_recent_projects.begin(), location);
    if (m_recent_projects.size() > 4) {
        m_recent_projects.erase(m_recent_projects.end());
    }
}

daw_range* OProject::GetTimeRange() {
    return &m_daw_range;
}

daw_time* OProject::GetDawTime() {
    return &m_daw_time;
}

void OProject::SaveRange(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "range");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start", "%d", m_daw_range.m_loopstart);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end", "%d", m_daw_range.m_loopend);
    m_daw_range.m_dirty = false;
    xmlTextWriterEndElement(writer);
    printf ("Project::Save: range saved\n");
}

void OProject::SaveZoom(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "zoom");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start", "%d", m_daw_time.m_viewstart);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end", "%d", m_daw_time.m_viewend);
    xmlTextWriterEndElement(writer);
    printf ("Project::Save: zoom saved\n");
}

void OProject::SaveCommands(xmlTextWriterPtr writer) {
    char cv[16];
    for (std::map<std::string, OscCmd*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
        xmlTextWriterStartElement(writer, BAD_CAST "cmd");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "name", "%s", it->second->GetName().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->second->GetPath().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "types", "%s", it->second->GetTypes().data());
        sprintf(cv, "%d", it->second->GetColor().get_red_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "red", "%s", cv);
        sprintf(cv, "%d", it->second->GetColor().get_green_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "green", "%s", cv);
        sprintf(cv, "%d", it->second->GetColor().get_blue_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "blue", "%s", cv);
        sprintf(cv, "%d", it->second->GetColor().get_alpha_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "alpha", "%s", cv);
        xmlTextWriterEndElement(writer);
        printf ("Project::Save: command %s saved\n", it->second->GetPath().data());
        
    }
}

void OProject::SaveTracks(xmlTextWriterPtr writer) {
	char cv[16];
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        OTrackStore* ts = it->second;
        ts->Lock();
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        sprintf(cv, "%d", it->second->m_expanded);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "expand", "%s", cv);
        sprintf(cv, "%d", it->second->m_height);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "height", "%s", cv);

        xmlTextWriterEndElement(writer);
        it->second->SaveData(m_projectFile.data());
        printf ("Project::Save: track %s saved\n", it->first.data());
        ts->Unlock();
    }
}

OTrackStore* OProject::NewTrack(OscCmd* cmd) {
    OTrackStore* ts = new OTrackStore(cmd);
    m_tracks[cmd->GetPath()] = ts;
    return ts;
}

int OProject::GetLoopStart() {
    return m_daw_range.m_loopstart;
}

void OProject::SetBitRate(int rate) {
    m_daw_time.m_bitrate = rate;
}

void OProject::SetMaxMillis(int max_millis) {

    m_daw_time.m_maxmillis = max_millis;
    if (m_daw_range.m_loopend == -1) {
        m_daw_range.m_loopend = max_millis;
    }
    m_daw_time.m_viewend = max_millis;

}

void OProject::SetPlaying(bool val) {
    m_playing = val;
}

OscCmd* OProject::GetCommand(char* path) {
    return m_known_mixer_commands[path];
}

void OProject::AddCommand(OscCmd* cmd) {
    m_known_mixer_commands[cmd->GetPath()] = cmd;
}

void OProject::RemoveCommand(OscCmd* cmd) {
    OTrackStore *ts = m_tracks[cmd->GetPath()];
    m_tracks.erase(cmd->GetPath());
    delete ts;
    m_known_mixer_commands.erase(cmd->GetPath());
    delete cmd;
    m_dirty = true;
}

OTrackStore* OProject::GetTrack(std::string path) {
	return m_tracks[path];
}

std::map<std::string, OTrackStore*> OProject::GetTracks() {
    return m_tracks;
}

void OProject::PlayTrackEntry(OTrackStore* trackstore, track_entry* entry){
    switch (trackstore->m_cmd->GetTypes().data()[0]) {
        case 'f':
            m_mixer->SendFloat(trackstore->m_cmd->GetPath(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(trackstore->m_cmd->GetPath(), entry->val.i);
            break;
    }
}

bool OProject::UpdatePos(OTimer* timer) {
    bool ret_code = false;
    int current = timer->GetPosMillis();


	for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {

        OTrackStore* trackstore = it->second;
        //trackstore->Lock();
        track_entry* entry = trackstore->GetEntry(current);
        if (entry != trackstore->m_playhead) {
        	PlayTrackEntry(trackstore, entry);
        	trackstore->m_playhead = entry;
        	ret_code = true;
        }
        //trackstore->Unlock();

    }
	return ret_code;
}

bool OProject::ProcessPos(OscCmd* cmd, OTimer* timer) {
    bool ret_code = false;
    int current = timer->GetPosMillis();
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
       
        OTrackStore* trackstore = it->second;
        trackstore->Lock();
        track_entry* entry = trackstore->GetEntry(current);

        if (entry != trackstore->m_playhead) {
            if (!trackstore->m_record || (trackstore->m_record && !m_playing)) {
                PlayTrackEntry(trackstore, entry);
            } else {
                if (m_playing && trackstore->m_record) {
                    trackstore->RemoveEntry(entry);
                }
            }
        }
        if (cmd && it->first == cmd->GetPath()) {
            AddEntry(trackstore, cmd, current);
            ret_code = true;
        }
        trackstore->Unlock();
    }
    
    return ret_code;
}

void OProject::AddEntry(OTrackStore* trackstore, OscCmd* cmd, int timepos) {
    if (trackstore) {
        if (trackstore->m_record) {
            track_entry *entry = NULL;
            if (m_playing) {
                if (timepos != trackstore->m_playhead->time) {
                    entry = trackstore->NewEntry();
                    entry->time = timepos;
                    entry->next = NULL;
                    entry->prev = NULL;
                    trackstore->AddTimePoint(entry);
                }
                else {
                    entry = trackstore->m_playhead;
                }
            } else {
                entry = trackstore->GetEntry(timepos);
            }

            if (entry) {
                switch (cmd->GetTypes().data()[0]) {
                    case 'f':
                        entry->val.f = cmd->GetLastFloat();
                        break;
                    case 'i':
                        entry->val.i = cmd->GetLastInt();
                        break;
                }
                trackstore->m_dirty = true;
            }
        }
    }
}

OscCmd* OProject::ProcessConfig(OscCmd* cmd) {
    printf("ProcessConfig %s\n", cmd->GetPath().data());
    for (std::map<std::string, OscCmd*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
        if (it->second->GetConfigRequestName() == cmd->GetPath()) {
            it->second->SetName(cmd->GetLastStr());
            return it->second;
        }
        if (it->second->GetConfigRequestColor() == cmd->GetPath()) {
            it->second->SetColorIndex(cmd->GetLastInt());
            return it->second;
        }        
        
    }
    return NULL;
}
