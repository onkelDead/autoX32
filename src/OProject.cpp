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
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete it->second;
    }
}

void OProject::SetTracksLayout(IOTracksLayout *layout) {
    m_layout = layout;
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

void OProject::SetDirty() {
    m_dirty = true;
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

gint OProject::GetInteger(xmlNodePtr node, const char* name) {
    xmlChar *keyword = xmlGetProp(node, BAD_CAST name);
    gint result = atoi((const char *) keyword);
    xmlFree(keyword);
    return result;
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
        m_daw_range.m_loopstart = GetInteger(node, "start");
        m_daw_range.m_loopend = GetInteger(node, "end");
        m_daw_range.m_dirty = false;
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/zoom", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_daw_time.m_viewstart = GetInteger(node, "start");
        m_daw_time.m_viewend = GetInteger(node, "end");
    }
    xmlXPathFreeObject(result);


    result = xmlXPathEvalExpression(BAD_CAST "//project/cmd", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        while(node) {
            if (node->type == XML_ELEMENT_NODE) {
                if (strcmp((const char*)node->name, "cmd") != 0)
                    break;
                xmlChar *xmlPath = xmlGetProp(node, BAD_CAST "path");
                xmlChar *xmlTypes = xmlGetProp(node, BAD_CAST "types");

                char* path = strdup((char*) xmlPath);
                char* types = strdup((char*) xmlTypes);
                m_known_mixer_commands[path] = new OscCmd(path, types);
                xmlFree(xmlPath);
                xmlFree(xmlTypes);
                char* name = (char*) xmlGetProp(node, BAD_CAST "name");
                m_known_mixer_commands[path]->SetName(name);
                xmlFree(name);
                Gdk::RGBA color;
                color.set_rgba_u(0, 0, 0, 32768);
                color.set_red_u(GetInteger(node, "red"));
                color.set_green_u(GetInteger(node, "green"));
                color.set_blue_u(GetInteger(node, "blue"));
                color.set_alpha_u(GetInteger(node, "alpha"));
                m_known_mixer_commands[path]->SetColor(color);
                free(path);
                free(types);
            }
            node = node->next; 
        }
    }
    xmlXPathFreeObject(result);

    result = xmlXPathEvalExpression(BAD_CAST "//project/track", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        gint c = 0;
        nodeset = result->nodesetval;
        xmlNodePtr node = *nodeset->nodeTab;
        while(node) {
            if (node->type == XML_ELEMENT_NODE) {
                if (strcmp((const char*)node->name, "track") != 0)
                    break;
                char* path = (char*) xmlGetProp(node, BAD_CAST "path");
                char* expanded = (char*) xmlGetProp(node, BAD_CAST "expand");
                char* height = (char*) xmlGetProp(node, BAD_CAST "height");
                char* layout_index = (char*) xmlGetProp(node, BAD_CAST "layout_index");
                char* visible = (char*) xmlGetProp(node, BAD_CAST "visible");
                
                OscCmd* cmd = m_known_mixer_commands[path];
                OTrackStore *ts = NewTrack(cmd);
                ts->m_expanded = atoi(expanded);
                ts->m_height = atoi(height);
                if (layout_index) {
                    ts->m_index = atoi(layout_index);
                    xmlFree(layout_index);
                }
                else
                    ts->m_index = c++;
                ts->m_visible = visible ? atoi(visible) : true;
                        
                ts->LoadData(m_projectFile.c_str());
                if (visible)
                    xmlFree(visible);
                xmlFree(path);
                xmlFree(expanded);
                xmlFree(height);
            }
            node = node->next;
        }
    }
    xmlXPathFreeObject(result);

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
}

bool OProject::OpenFromArdurRecent() {
    FILE* file_recent;
    char name[256];
    char path[256];
    bool ret_val = false;
    int dummy;

    file_recent = fopen("/home/onkel/.config/ardour5/recent", "r");
    if (file_recent != NULL) {
        dummy = fscanf(file_recent, "%s", name);
        dummy = fscanf(file_recent, "%s", path);
        fclose(file_recent);

        strncat(path, "/autoX32", 32);
        printf("%s\n", path);
        if (access(path, F_OK)) {
            printf("project don't exists\n");
            m_location = path;
            if (mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP) != 0) {
                perror("mkdir() error");
                return ret_val;
            }
            New();
            Save();
        } else {
            Load(path);
            ret_val = true;
        }
    }
    return ret_val;
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
    printf("Project::Save: range saved\n");
}

void OProject::SaveZoom(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "zoom");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start", "%d", m_daw_time.m_viewstart);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end", "%d", m_daw_time.m_viewend);
    xmlTextWriterEndElement(writer);
    printf("Project::Save: zoom saved\n");
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
        printf("Project::Save: command %s saved\n", it->second->GetPath().data());

    }
}

void OProject::SaveTracks(xmlTextWriterPtr writer) {
    char cv[16];
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        gint layout_index = m_layout->GetTrackIndex(it->first);
        OTrackStore* ts = it->second;
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        sprintf(cv, "%d", it->second->m_expanded);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "expand", "%s", cv);
        sprintf(cv, "%d", it->second->m_height);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "height", "%s", cv);
        sprintf(cv, "%d", layout_index);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "layout_index", "%s", cv);
        sprintf(cv, "%d", it->second->m_visible);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "visible", "%s", cv);

        xmlTextWriterEndElement(writer);
        if (it->second->m_dirty) {
            it->second->SaveData(m_projectFile.data());
            printf("Project::Save: track %s saved\n", it->first.data());
        }
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
    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        OTrackStore* ts = it->second;
        ts->m_playing = val;
    }
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
    auto e = m_tracks.find(path);
    if (e != m_tracks.end())
        return m_tracks[path];
    return NULL;
}

std::map<std::string, OTrackStore*> OProject::GetTracks() {
    return m_tracks;
}

bool OProject::PlayTrackEntry(OTrackStore* trackstore, track_entry* entry) {
    if (entry == nullptr)
        return false;
    OscCmd* cmd = trackstore->GetOscCommand();
    switch (cmd->GetTypes().data()[0]) {
        case 'f':
            m_mixer->SendFloat(cmd->GetPath(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(cmd->GetPath(), entry->val.i);
            break;
    }
    return true;
}

bool OProject::UpdatePos(gint current, bool jump) {
    bool ret_code = false;

    for (std::map<std::string, OTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {

        OTrackStore* trackstore = it->second;
        ret_code = PlayTrackEntry(trackstore, trackstore->UpdatePlayhead(current, jump));

    }
    return ret_code;
}

bool OProject::ProcessPos(OTrackStore* trackstore, OscCmd* cmd, gint current) {
    bool ret_code = false;
    if (trackstore) {
        if (trackstore->m_record) {
            trackstore->AddEntry(cmd, current);
        }
        ret_code = true;
    }

    return ret_code;
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
