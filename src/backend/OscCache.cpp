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
        delete it->second;
    }
    m_cache.clear();
}

bool OscCache::ProcessMessage(IOscMessage* msg) {
    if (!m_cache.contains(msg->GetPath())) {
        m_cache[msg->GetPath()] = msg;
        m_callback_handler->NewMessageCallback(msg);
        return false;
    }
    else {
        
        *m_cache[msg->GetPath()]->GetVal(0) = *msg->GetVal(0);
        m_callback_handler->UpdateMessageCallback(m_cache[msg->GetPath()]);
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
    IOscMessage* msg = m_cache[path];
    m_cache.erase(path);
}