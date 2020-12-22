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

OProject::OProject() : m_mixer(0) {
    m_dirty = false;
    m_playing = false;
    m_lock_playhead = false;
    m_daw_range.m_loopend = -1;
    m_daw_range.m_loopstart = 0;
    m_daw_range.m_dirty = false;
    m_daw_time.m_bitrate = 0;
    m_daw_time.m_maxsamples = 1;
    m_daw_time.scale = 1;
    m_daw_time.m_viewstart = 0;
    m_daw_time.m_viewend = -1;
}

OProject::OProject(std::string location) : m_mixer(0) {

    m_daw_time.m_bitrate = 0;
    m_daw_time.m_maxsamples = 1;
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
        for (int i = 0; i < nodeset->nodeMax; i++) {
            xmlNodePtr node = nodeset->nodeTab[i];
            if (node) {
                const char* cv;
                const char* path = (char*) xmlGetProp(node, BAD_CAST "path");
                m_known_mixer_commands[path] = new OscCmd(path, (char*) xmlGetProp(node, BAD_CAST "types"));
                const char* name = (char*) xmlGetProp(node, BAD_CAST "name");
                m_known_mixer_commands[path]->m_name = strdup(name);
                cv = (char*) xmlGetProp(node, BAD_CAST "red");
                m_known_mixer_commands[path]->m_color.set_red_u(atoi(cv));
                cv = (char*) xmlGetProp(node, BAD_CAST "green");
                m_known_mixer_commands[path]->m_color.set_green_u(atoi(cv));
                cv = (char*) xmlGetProp(node, BAD_CAST "blue");
                m_known_mixer_commands[path]->m_color.set_blue_u(atoi(cv));
            }
        }
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/track", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        for (int i = 0; i < nodeset->nodeMax; i++) {
            xmlNodePtr node = nodeset->nodeTab[i];
            if (node) {
                const char* path = (char*) xmlGetProp(node, BAD_CAST "path");
                OscCmd* cmd = m_known_mixer_commands[path];
                OTrackStore *ts = NewTrack(cmd);
                ts->LoadData(m_projectFile.data());
            }
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
//    m_location = "";
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
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "name", "%s", it->second->m_name.data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->second->GetPathStr().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "types", "%s", it->second->m_types.data());
        sprintf(cv, "%d", it->second->m_color.get_red_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "red", "%s", cv);
        sprintf(cv, "%d", it->second->m_color.get_green_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "green", "%s", cv);
        sprintf(cv, "%d", it->second->m_color.get_blue_u());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "blue", "%s", cv);
        xmlTextWriterEndElement(writer);
        printf ("Project::Save: command %s saved\n", it->second->GetPathStr().data());
        
    }
}

void OProject::SaveTracks(xmlTextWriterPtr writer) {
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        xmlTextWriterEndElement(writer);
        it->second->SaveData(m_projectFile.data());
        printf ("Project::Save: track %s saved\n", it->first.data());
    }
}

OTrackStore* OProject::NewTrack(OscCmd* cmd) {
    OTrackStore* ts = new OTrackStore(cmd);
    m_tracks[cmd->GetPathStr()] = ts;
    return ts;
}

gint OProject::GetLoopStart() {
    return m_daw_range.m_loopstart;
}

void OProject::SetBitRate(gint rate) {
    m_daw_time.m_bitrate = rate;
}

void OProject::SetMaxSamples(gint max_samples) {

    m_daw_time.m_maxsamples = max_samples;
    if (m_daw_range.m_loopend == -1) {
        m_daw_range.m_loopend = max_samples;
    }
    m_daw_time.m_viewend = max_samples;
}

void OProject::SetPlaying(bool val) {
    m_playing = val;
}

bool OProject::AnyTouch() {
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        if (it->second->m_touch)
            return true;
    }
    return false;
}

std::map<std::string, OscCmd*> OProject::GetMixerCommands() {
    return m_known_mixer_commands;
}

OscCmd* OProject::GetCommand(char* path) {
    return m_known_mixer_commands[path];
}

void OProject::AddCommand(OscCmd* cmd) {
    m_known_mixer_commands[cmd->GetPathStr()] = cmd;
}

void OProject::RemoveCommand(OscCmd* cmd) {
    OTrackStore *ts = m_tracks[cmd->GetPathStr()];
    m_tracks.erase(cmd->GetPathStr());
    delete ts;
    m_known_mixer_commands.erase(cmd->GetPathStr());
    delete cmd;
    m_dirty = true;
}

std::map<std::string, OTrackStore*> OProject::GetTracks() {
    return m_tracks;
}

void OProject::PlayTrackEntry(OTrackStore* trackstore, track_entry* entry){
    switch (trackstore->m_cmd->m_types.data()[0]) {
        case 'f':
            m_mixer->SendFloat(trackstore->m_cmd->GetPathStr(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(trackstore->m_cmd->GetPathStr(), entry->val.i);
            break;
    }
    trackstore->m_playhead = entry;
}

bool OProject::ProcessPos(OscCmd* cmd, OTimer* timer) {
    bool ret_code = false;
    int current = timer->GetSamplePos();
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        
        OTrackStore* trackstore = it->second;

        
        track_entry* entry = trackstore->GetEntry(current);

        if (entry != trackstore->m_playhead) {
            if (trackstore->m_record && m_playing) {

            }
            if (!trackstore->m_record || (trackstore->m_record && !m_playing)) {
                PlayTrackEntry(trackstore, entry);
            } else {
                if (m_playing && trackstore->m_record) {
                    trackstore->RemoveEntry(entry);
                }
            }
        }
        if (cmd && it->first == cmd->GetPathStr()) {
            AddEntry(trackstore, cmd, current);
            ret_code = true;
        }
    }
    

    
    return ret_code;
}

void OProject::AddEntry(OTrackStore* trackstore, OscCmd* cmd, int samplepos) {
    if (trackstore) {
        if (trackstore->m_record) {
            track_entry *entry = NULL;
            if (m_playing) {
                if (samplepos != trackstore->m_playhead->sample) {
                    entry = new track_entry;
                    entry->sample = samplepos;
                    entry->next = NULL;
                    entry->prev = NULL;
                    trackstore->AddSamplePoint(entry);
                }
                else {
                    entry = trackstore->m_playhead;
                }
            } else {
                entry = trackstore->GetEntry(samplepos);
            }

            if (entry) {
                switch (cmd->m_types.data()[0]) {
                    case 'f':
                        entry->val.f = cmd->last_float;
                        break;
                    case 'i':
                        entry->val.i = cmd->last_int;
                        break;
                }
                trackstore->m_dirty = true;
            }
        }
    }
}

OscCmd* OProject::ProcessConfig(OscCmd* cmd) {
    printf("ProcessConfig %s\n", cmd->m_path.data());
    for (std::map<std::string, OscCmd*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
        if (it->second->GetConfigName() == cmd->GetPathStr()) {
            it->second->m_name = cmd->last_str;
            return it->second;
        }
        if (it->second->GetConfigColor() == cmd->GetPathStr()) {
            it->second->SetColorIndex(cmd->last_int);
            return it->second;
        }        
        
    }
    return NULL;
}