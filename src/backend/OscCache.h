/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscCache.h
 * Author: onkel
 *
 * Created on March 19, 2022, 8:31 AM
 */

#ifndef OSCCACHE_H
#define OSCCACHE_H

#include <map>
#include "IOCacheCallbackHandler.h"
#include "IOscMessage.h"
#include "ISerializer.h"

class OscCache : ISerializer {
public:
    OscCache();
    OscCache(const OscCache& orig);
    virtual ~OscCache();
    
    IOscMessage* AddCacheMessage(const char*, const char*);
    void ReleaseCacheMessage(std::string);
    
    bool ProcessMessage(IOscMessage*);

    bool GetCachedValue(std::string path, float*);
    bool GetCachedValue(std::string path, int*);
    bool GetCachedValue(std::string path, std::string*);
    
    size_t GetCacheSize() const {
        return m_cache.size();
    }

    void SetCallback_handler(IOCacheCallbackHandler* callback_handler) {
        m_callback_handler = callback_handler;
    }
    
    IOscMessage* GetCachedMsg(const char*);
    
    // Implementation ISerializer
    void Save(xmlTextWriterPtr);
    
private:
    
    std::map<std::string, IOscMessage*> m_cache;
    IOCacheCallbackHandler* m_callback_handler;
    
};

#endif /* OSCCACHE_H */

