/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscCache.cpp
 * Author: onkel
 * 
 * Created on March 19, 2022, 8:31 AM
 */

#include "OscMessage.h"
#include "OscCache.h"

OscCache::OscCache() {
}

OscCache::OscCache(const OscCache& orig) {
}

OscCache::~OscCache() {
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        delete (OscMessage*)it->second;
    }
    m_cache.clear();
}

bool OscCache::ProcessMessage(IOscMessage* msg) {
    
    // Message not known in cache
    if (!m_cache.contains(msg->GetPath())) {
        IOscMessage* new_msg = new OscMessage(msg->GetPath().c_str(), msg->GetTypes());
        new_msg->SetVal(msg->GetVal(0));
        m_cache[msg->GetPath()] = new_msg;
        m_callback_handler->NewMessageCallback(new_msg);
        return false;
    }
    else {
        m_callback_handler->UpdateMessageCallback(msg);
        return true;
    }
}

bool OscCache::GetCachedValue(std::string path, float* result) {
    if (!m_cache.contains(path)) {
        return false;
    }
    else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetFloat();
        return true;
    }
}

bool OscCache::GetCachedValue(std::string path, int* result) {
    if (!m_cache.contains(path)) {
        return false;
    }
    else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetInteger();
        return true;
    }
}

bool OscCache::GetCachedValue(std::string path, std::string* result) {
    if (!m_cache.contains(path)) {
        return false;
    }
    else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetString();
        return true;
    }
}

IOscMessage* OscCache::AddCacheMessage(const char* path, const char* types) {
    return m_cache[path] = new OscMessage(path, types);
}

void OscCache::ReleaseCacheMessage(std::string path) {
    m_cache.erase(path);
}

IOscMessage* OscCache::GetCachedMsg(const char* path) {
    if (m_cache.contains(path))
        return m_cache[path];
    return nullptr;
}

void OscCache::Save(xmlTextWriterPtr writer) {
    //char cv[16];
    for (std::map<std::string, IOscMessage*>::iterator it = m_cache.begin(); it != m_cache.end(); ++it) {
        xmlTextWriterStartElement(writer, BAD_CAST "cmd");
//        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "name", "%s", it->second->GetName().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", it->second->GetPath().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "types", "%s", it->second->GetTypes());
//        sprintf(cv, "%d", it->second->GetColor().get_red_u());
//        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "red", "%s", cv);
//        sprintf(cv, "%d", it->second->GetColor().get_green_u());
//        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "green", "%s", cv);
//        sprintf(cv, "%d", it->second->GetColor().get_blue_u());
//        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "blue", "%s", cv);
//        sprintf(cv, "%d", it->second->GetColor().get_alpha_u());
//        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "alpha", "%s", cv);
        xmlTextWriterEndElement(writer);
        printf("Project::Save: command %s saved\n", it->second->GetPath().data());

    }    
}