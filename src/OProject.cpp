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
#include "IOMixer.h"

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
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete (OTrackStore*)it->second;
    }
}

void OProject::SetTracksLayout(IOTracksLayout *layout) {
    m_layout = layout;
}

void OProject::SetMixer(IOMixer* mixer) {
    m_mixer = mixer;
}

bool OProject::GetDirty() {
    bool dirty = true; //m_dirty;

    dirty |= m_daw_range.m_dirty;
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        dirty |= it->second->IsDirty();
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

    doc = xmlParseFile(m_projectFile.data());
    if (doc == nullptr) {
        m_mixer->ReadAll();
        return;
    }
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
                xmlChar *xmlValue = xmlGetProp(node, BAD_CAST "value");

                char* path = strdup((char*) xmlPath);
                char* types = strdup((char*) xmlTypes);
                char* val = strdup((char*) xmlValue);
//                m_known_mixer_commands[path] = m_mixer->AddCacheMessage(path, types);
                xmlFree(xmlPath);
                xmlFree(xmlTypes);
                xmlFree(xmlValue);
                char* name = (char*) xmlGetProp(node, BAD_CAST "name");
                m_mixer->AddCacheMessage(path, types, val);
                //m_known_mixer_commands[path]->SetName(name);
                xmlFree(name);
//                Gdk::RGBA color;
//                color.set_rgba_u(0, 0, 0, 32768);
//                color.set_red_u(GetInteger(node, "red"));
//                color.set_green_u(GetInteger(node, "green"));
//                color.set_blue_u(GetInteger(node, "blue"));
//                color.set_alpha_u(GetInteger(node, "alpha"));
                //m_known_mixer_commands[path]->SetColor(color);
                free(path);
                free(types);
                free(val);
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
                char* path = (char*) xmlGetProp(node, BAD_CAST "pat"
                "h");
                char* expanded = (char*) xmlGetProp(node, BAD_CAST "expand");
                char* height = (char*) xmlGetProp(node, BAD_CAST "height");
                char* layout_index = (char*) xmlGetProp(node, BAD_CAST "layout_index");
                char* visible = (char*) xmlGetProp(node, BAD_CAST "visible");
                
                IOscMessage* msg = m_mixer->GetCachedMessage(path);
                IOTrackStore *ts = NewTrack(msg);
                
                ts->GetLayout()->m_expanded = atoi(expanded);
                ts->GetLayout()->m_height = atoi(height);
                if (layout_index) {
                    ts->GetLayout()->m_index = atoi(layout_index);
                    xmlFree(layout_index);
                }
                else
                    ts->GetLayout()->m_index = c++;
                ts->GetLayout()->m_visible = visible ? atoi(visible) : true;
                        
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
    m_mixer->WriteAll();
}

bool OProject::OpenFromArdurRecent() {
    FILE* file_recent;
    char path[256];
    bool ret_val = false;

    file_recent = fopen("/home/onkel/.config/ardour5/recent", "r");
    if (file_recent != NULL) {
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
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete (OTrackStore*)it->second;
    }
//    for (std::map<std::string, IOscMessage*>::iterator it = m_known_mixer_commands.begin(); it != m_known_mixer_commands.end(); ++it) {
//        delete (OscMessage*)it->second;
//    }
    m_tracks.clear();
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
    m_mixer->Save(writer);
}

void OProject::SaveTracks(xmlTextWriterPtr writer) {
    char cv[16];
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        gint layout_index = m_layout->GetTrackIndex(it->first);
        IOTrackStore* ts = it->second;
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        sprintf(cv, "%d", ts->GetLayout()->m_expanded);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "expand", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_height);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "height", "%s", cv);
        sprintf(cv, "%d", layout_index);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "layout_index", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_visible);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "visible", "%s", cv);

        xmlTextWriterEndElement(writer);
        if (ts->IsDirty()) {
            ts->SaveData(m_projectFile.data());
            printf("Project::Save: track %s saved\n", it->first.data());
        }
    }
}

IOTrackStore* OProject::NewTrack(IOscMessage* msg) {
    IOTrackStore* ts = new OTrackStore(msg);
    m_tracks[msg->GetPath()] = ts;
    return ts;
}

int OProject::GetLoopStart() {
    return m_daw_range.m_loopstart;
}

int OProject::GetLoopEnd() {
    return m_daw_range.m_loopend;
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
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        ts->SetPlaying(val);
    }
}

IOTrackStore* OProject::GetTrack(std::string path) {
    auto e = m_tracks.find(path);
    if (e != m_tracks.end())
        return m_tracks[path];
    return NULL;
}

std::map<std::string, IOTrackStore*> OProject::GetTracks() {
    return m_tracks;
}
