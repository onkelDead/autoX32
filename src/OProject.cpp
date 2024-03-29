/*
  Copyright 2020 Detlef Ursn <onkel@paraair.de>

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

#include <filesystem>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "OProject.h"
#include "OTrackStore.h"
#include "IOMixer.h"

OProject::OProject() {

}

OProject::~OProject() {
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete (OTrackStore*)it->second;
    }
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

void OProject::LockPlayhead(bool val) {
    m_lock_playhead = val;
}

int OProject::GetInteger(xmlNodePtr node, const char* name) {
    xmlChar *keyword = xmlGetProp(node, BAD_CAST name);
    if (keyword) {
        int result = atoi((const char *) keyword);
        xmlFree(keyword);
        return result;
    }
    return 0;
}

int OProject::Load(std::string location) {
    std::string name = basename(location.data());

    setlocale( LC_ALL, "" ); 
    
    std::string projectFile = location;
    projectFile.append("/").append(name.append(".xml").data());

    xmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;

    LoadCache(location);
    
    doc = xmlParseFile(projectFile.data());
    if (doc == nullptr) {
        return 1;
    }
    context = xmlXPathNewContext(doc);
    
    result = xmlXPathEvalExpression(BAD_CAST "//project/states", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        nodeset = result->nodesetval;
        xmlNodePtr node = nodeset->nodeTab[0];
        m_lock_teach = GetInteger(node, "lock_teach") == 0 ? false : true;
        m_teach_active = GetInteger(node, "teach_active") == 0 ? false : true;
        m_step_mode = GetInteger(node, "step_mode") == 0 ? false : true;
        m_wheel_mode = GetInteger(node, "wheel_mode") == 0 ? false : true;
    }
    xmlXPathFreeObject(result);    
    
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

    result = xmlXPathEvalExpression(BAD_CAST "//project/track", context);
    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        int c = 0;
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
                IOTrackStore *trackstore = NewTrack(msg);
                trackstore->GetLayout()->m_expanded = atoi(expanded);
                trackstore->GetLayout()->m_height = atoi(height);
                if (layout_index) {
                    trackstore->GetLayout()->m_index = atoi(layout_index);
                    xmlFree(layout_index);
                }
                else
                    trackstore->GetLayout()->m_index = c++;
                trackstore->GetLayout()->m_visible = visible ? atoi(visible) : true;
                        
                trackstore->LoadData(location.c_str());
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
    return 0;
}

void OProject::LoadCache(std::string location) {
    m_mixer->Load(location);
}

void OProject::Save(std::string location) {
    std::string name = basename(location.data());

    std::string projectFile = location;
    projectFile.append("/").append(name.append(".xml").data());

    setlocale( LC_ALL, "" ); 

    xmlTextWriterPtr writer;

    if (!std::filesystem::exists(location)) {
        std::filesystem::create_directory(location);
    }
    
    writer = xmlNewTextWriterFilename(projectFile.data(), 0);
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    xmlTextWriterStartElement(writer, BAD_CAST "project");
    SaveStates(writer);
    SaveRange(writer);
    SaveZoom(writer);
    SaveCache(location);
    SaveTracks(writer, location);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
    m_dirty = false;
}

void OProject::Close() {
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        delete (OTrackStore*)it->second;
    }

    m_tracks.clear();
    m_daw_range.m_dirty = false;
}

daw_range* OProject::GetTimeRange() {
    return &m_daw_range;
}

daw_time* OProject::GetDawTime() {
    return &m_daw_time;
}

void OProject::SaveStates(xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST "states");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "lock_teach", "%d", m_lock_teach);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "teach_active", "%d", m_teach_active);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "step_mode", "%d", m_step_mode);
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "wheel_mode", "%d", m_wheel_mode);
    xmlTextWriterEndElement(writer);
    printf("Project::Save: states saved\n");    
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

void OProject::SaveCache(std::string location) {
    m_mixer->Save(location);
}

void OProject::SaveTracks(xmlTextWriterPtr writer, std::string location) {
    char cv[16];
    int index = 0;
    
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        xmlTextWriterStartElement(writer, BAD_CAST "track");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->first.data());
        sprintf(cv, "%d", ts->GetLayout()->m_expanded);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "expand", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_height);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "height", "%s", cv);
        sprintf(cv, "%d", index++);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "layout_index", "%s", cv);
        sprintf(cv, "%d", ts->GetLayout()->m_visible);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "visible", "%s", cv);

        xmlTextWriterEndElement(writer);
        if (ts->IsDirty()) {
            ts->SaveData(location.data());
            printf("Project::Save: track %s saved\n", it->first.data());
        }
    }
}

IOTrackStore* OProject::NewTrack(IOscMessage* msg) {
    std::cout << "New track : " << msg->GetPath() << std::endl;
    IOTrackStore* trackstore = new OTrackStore(msg);
    m_tracks[msg->GetPath()] = trackstore;
    std::string conf_name = trackstore->GetConfigRequestName();
    m_mixer->AddCacheMessage(conf_name.c_str(), "s")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);
    conf_name = trackstore->GetConfigRequestColor();
    m_mixer->AddCacheMessage(conf_name.c_str(), "i")->SetTrackstore(trackstore);
    m_mixer->Send(conf_name);      
    return trackstore;
}

void OProject::RemoveTrack(std::string path) {
    IOTrackStore* track = m_tracks[path];
    if (track == m_selectedTrack) {
        m_selectedTrack = nullptr;
        
    }
    track->GetMessage()->SetTrackstore(nullptr);
    delete track;
    m_tracks.erase(path);
    
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

void OProject::SetMaxFrames(int max_frames) {
    m_daw_time.m_maxframes = max_frames;
    if (m_daw_range.m_loopend == -1) {
        m_daw_range.m_loopend = max_frames;
    }
    m_daw_time.m_viewend = max_frames;

}

void OProject::SetPlaying(bool val) {
    m_playing = val;
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        ts->SetPlaying(val);
        if (ts->IsRecording() && !val) {
            ts->SetRecording(false);
        }
    }        
}

void OProject::StopRecord() {
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;
        ts->SetRecording(false);
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

bool OProject::UpdatePos(int current_time, bool seek) {
    bool ret = false;

#ifdef PERF_OProjectUpdatePos
    auto start = std::chrono::steady_clock::now();
#endif
    
    for (std::map<std::string, IOTrackStore*>::iterator it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        IOTrackStore* ts = it->second;

        track_entry* e = ts->UpdatePos(current_time, seek);
        if (e) {
            ret |= PlayTrackEntry(ts, e) && ts == m_selectedTrack;
        }
    }   
    
#ifdef PERF_OProjectUpdatePos
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "OProject::UpdatePos::Time " << seek << " difference = " << duration << "[µs]" << std::endl;    
#endif
   
    return ret;
}

bool OProject::PlayTrackEntry(IOTrackStore* trackstore, track_entry* entry) {
    if (entry == nullptr || trackstore == nullptr)
        return false;
    IOscMessage* cmd = trackstore->GetMessage();
    switch (cmd->GetTypes()[0]) {
        case 'f':
            m_mixer->SendFloat(cmd->GetPath(), entry->val.f);
            break;
        case 'i':
            m_mixer->SendInt(cmd->GetPath(), entry->val.i);
            break;
        case 's':
            m_mixer->SendString(cmd->GetPath(), &entry->val.s);
            break;            
    }
    return true;    
}

std::string OProject::GetNextTrackPath() {
    std::map<std::string, IOTrackStore*>::iterator it;
    IOTrackStore* ts;
    
    if (m_tracks.size() == 0) return "";
    if (m_selectedTrack == nullptr) {
        it = m_tracks.begin();
        ts = it->second;
        return ts->GetPath();
    }
     
    for (it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        ts = it->second;
        if (ts == m_selectedTrack) {
            ++it;
            if (it == m_tracks.end())
                ts = m_tracks.begin()->second;
            else 
                ts = it->second;
            break;
        }
    }
    return ts->GetPath();
}

std::string OProject::GetPrevTrackPath() {
    std::map<std::string, IOTrackStore*>::iterator it;
    IOTrackStore* ts;    
    if (m_tracks.size() == 0) return "";
    if (m_selectedTrack == nullptr) {
        it = m_tracks.end();
        it--;
        ts = it->second;
        return ts->GetPath();
    }   
    for (it = m_tracks.begin(); it != m_tracks.end(); ++it) {
        ts = it->second;
        if (ts == m_selectedTrack) {
            if (it == m_tracks. begin()) {
                it = m_tracks.end();
                it--;
                ts = it->second;
            }
            else 
                it--;
                ts = it->second;
            break;
        }
    }
    return ts->GetPath();    
}