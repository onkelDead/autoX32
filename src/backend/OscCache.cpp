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

#ifdef CACHE_MEASURE_ELPASE        
#include <chrono>
#endif

#include <unistd.h>
#include <thread>
#include <mutex>
#include "OscMessage.h"
#include "OscCache.h"

const char* mix_func[] = {
    "/mix/01/on",
    "/mix/02/on",
    "/mix/03/on",
    "/mix/04/on",
    "/mix/05/on",
    "/mix/06/on",
    "/mix/07/on",
    "/mix/08/on",
    "/mix/09/on",
    "/mix/10/on",
    "/mix/11/on",
    "/mix/12/on",
    "/mix/13/on",
    "/mix/14/on",
    "/mix/15/on",
    "/mix/16/on",
    "/mix/01/level",
    "/mix/02/level",
    "/mix/03/level",
    "/mix/04/level",
    "/mix/05/level",
    "/mix/06/level",
    "/mix/07/level",
    "/mix/08/level",
    "/mix/09/level",
    "/mix/10/level",
    "/mix/11/level",
    "/mix/12/level",
    "/mix/13/level",
    "/mix/14/level",
    "/mix/15/level",
    "/mix/16/level",
    "/mix/01/pan",
    "/mix/01/type",
    "/mix/03/pan",
    "/mix/03/type",
    "/mix/05/pan",
    "/mix/05/type",
    "/mix/07/pan",
    "/mix/07/type",
    "/mix/09/pan",
    "/mix/09/type",
    "/mix/11/pan",
    "/mix/11/type",
    "/mix/13/pan",
    "/mix/13/type",
    "/mix/15/pan",
    "/mix/15/type",
};
func1_t mix_funcs = {
    mix_func,
    48
};

const char* dyn_func[] = {
    "/dyn/on",
    "/dyn/mode",
    "/dyn/det",
    "/dyn/env",
    "/dyn/thr",
    "/dyn/ratio",
    "/dyn/knee",
    "/dyn/mgain",
    "/dyn/attack",
    "/dyn/hold",
    "/dyn/release",
    "/dyn/pos",
    "/dyn/mix",
    "/dyn/filter/on",
    "/dyn/filter/type",
    "/dyn/filter/f",
    "/dyn/auto",
    "/dyn/keysrc",
};
func1_t dyn_funcs = {
    dyn_func,
    18
};
func1_t dyn_mtx_funcs = {
    dyn_func,
    17
};
const char* gate_func[] = {
    "/gate/on",
    "/gate/mode",
    "/gate/thr",
    "/gate/range",
    "/gate/attack",
    "/gate/hold",
    "/gate/release",
    "/gate/keysrc",
    "/gate/filter/on",
    "/gate/filter/type",
    "/gate/filter/f",
};
func1_t gate_funcs{
    gate_func,
    11
};

const char* preamp_func[] = {
    "/preamp/trim",
    "/preamp/invert",
    "/preamp/hpon",
    "/preamp/hpslope",
    "/preamp/hpf",
};
func1_t preamp_funcs = {
    preamp_func,
    5
};

const char* eq_func[] = {
    "/eq/on",
    "/eq/1/type",
    "/eq/1/f",
    "/eq/1/g",
    "/eq/1/q",
    "/eq/2/type",
    "/eq/2/f",
    "/eq/2/g",
    "/eq/2/q",
    "/eq/3/type",
    "/eq/3/f",
    "/eq/3/g",
    "/eq/3/q",
    "/eq/4/type",
    "/eq/4/f",
    "/eq/4/g",
    "/eq/4/q",
};
func1_t eq_funcs = {
    eq_func,
    17
};

const char* base_func[] = {
    "/config/name",
    "/config/icon",
    "/config/color",
    "/mix/on",
    "/mix/fader",
    "/mix/pan",
    "/mix/mono",
    "/mix/mlevel"
};
func1_t base_funcs = {
    base_func,
    8
};

func1_t base_mtx_funcs = {
    base_func,
    5
};

func1_t base_short_funcs = {
    base_func,
    3
};

const char* grp_func[] = {
    "/grp/dca",
    "/grp/mute"
};
func1_t grp_funcs = {
    grp_func,
    2
};

const char* insert_func[] = {
    "/insert/pos",
    "/insert/on",
    "/insert/sel"
};
func1_t insert_funcs = {
    insert_func,
    3
};

func1_t* ch_func[] = {
    &base_funcs,
    &eq_funcs,
    &preamp_funcs,
    &gate_funcs,
    &dyn_funcs,
    &mix_funcs,
    &grp_funcs,
    &insert_funcs
};

func1_t* bus_func[] = {
    &base_funcs,
    &eq_funcs,
    &dyn_funcs,
    &grp_funcs,
    &insert_funcs
};
func1_t* auxin_func[] = {
    &base_funcs,
    &eq_funcs,
    &mix_funcs
};
func1_t* matrix_func[] = {
    &base_mtx_funcs,
    &eq_funcs,
    &dyn_mtx_funcs,
    &insert_funcs,
};

func1_t* fxrtn_func[] = {
    &base_funcs,
    &eq_funcs,
    &mix_funcs,
    &grp_funcs
};

func1_t* dca_func[] = {
    &base_short_funcs,
};

func1_t* main_st_func[] = {
    &base_short_funcs,
    &eq_funcs,
    &dyn_mtx_funcs
};

funcs_t ch_funcs = {
    ch_func,
    8
};
funcs_t bus_funcs = {
    bus_func,
    5
};
funcs_t auxin_funcs = {
    auxin_func,
    3
};

funcs_t matrix_funcs = {
    matrix_func,
    4
};

funcs_t fxrtn_funcs = {
    fxrtn_func,
    4
};

funcs_t dca_funcs = {
    dca_func,
    1
};

funcs_t main_st_funcs = {
    main_st_func,
    3
};

object_t obj[] = {
    {
        32,
        "/ch",
        ch_funcs,
        2
    },
    {
        16,
        "/bus",
        bus_funcs,
        2
    },
    {
        8,
        "/auxin",
        auxin_funcs,
        2
    },
    {
        6,
        "/mtx",
        matrix_funcs,
        2
    },
    {
        8,
        "/fxrtn",
        fxrtn_funcs,
        2
    },
    {
        8,
        "/dca",
        dca_funcs,
        1
    },
    {
        1,
        "/main/st",
        main_st_funcs,
        0
    }, 
};

objects_t objs{
    obj,
    7
};

OscCache::OscCache() {
}

OscCache::OscCache(const OscCache& orig) {
}

OscCache::~OscCache() {
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        delete (OscMessage*) it->second;
    }
    m_cache.clear();
}

bool OscCache::NewMessage(IOscMessage* msg) {
#ifdef CACHE_MEASURE_ELPASE        
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::microseconds;
    auto t1 = high_resolution_clock::now();
#endif        
    IOscMessage* new_msg = AddCacheMessage(msg->GetPath().c_str(), msg->GetTypes());

    new_msg->SetVal(msg->GetVal(0));
    m_cache[msg->GetPath()] = new_msg;
    m_callback_handler->NewMessageCallback(new_msg);
#ifdef CACHE_MEASURE_ELPASE        
    auto t2 = high_resolution_clock::now();
    duration<double, std::micro> ms_double = t2 - t1;
    std::cout << "elapse: " << ms_double.count() << "µs\n";
#endif
    return false;

}

bool OscCache::ProcessMessage(IOscMessage* msg) {

    m_callback_handler->UpdateMessageCallback(msg);
    return true;
}

bool OscCache::GetCachedValue(std::string path, float* result) {
    if (!m_cache.contains(path)) {
        return false;
    } else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetFloat();
        return true;
    }
}

bool OscCache::GetCachedValue(std::string path, int* result) {
    if (!m_cache.contains(path)) {
        return false;
    } else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetInteger();
        return true;
    }
}

bool OscCache::GetCachedValue(std::string path, std::string* result) {
    if (!m_cache.contains(path)) {
        return false;
    } else {
        IOscMessage* m = m_cache[path];
        *result = m->GetVal(0)->GetString();
        return true;
    }
}

IOscMessage* OscCache::AddCacheMessage(const char* path, const char* types) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache[path] = new OscMessage(path, types);
}

void OscCache::ReleaseCacheMessage(std::string path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    IOscMessage* msg = m_cache[path];
    if (msg) {
        msg->SetTrackstore(nullptr);
    }
}

IOscMessage* OscCache::GetCachedMsg(const char* path) {
    if (m_cache.contains(path))
        return m_cache[path];
    return nullptr;
}

void OscCache::Save(xmlTextWriterPtr writer) {
    char val[32];
    setlocale( LC_ALL, "" ); 
    for (std::map<std::string, IOscMessage*>::iterator it = m_cache.begin(); it != m_cache.end(); ++it) {
        IOscMessage* msg = it->second;
        xmlTextWriterStartElement(writer, BAD_CAST "cmd");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "path", "%s", msg->GetPath().data());
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "types", "%s", msg->GetTypes());
        msg->GetVal(0)->ToString(msg->GetTypes()[0], val);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "value", "%s", val);
        xmlTextWriterEndElement(writer);
    }
    
}

void OscCache::ReadAllFromMixer(IOMixer* x32) {
    for (int o = 0; o < objs.count; o++) {
        for (int i = 0; i < objs.objs[o].count; i++) {
            for (int j = 0; j < objs.objs[o].funcs.count; j++) {
                for (int k = 0; k < objs.objs[o].funcs.funcs[j]->count; k++) {
                    char cmd[64];
                    switch (objs.objs[o].num_idx_digits) {
                        case 0:
                            sprintf(cmd, "%s%s", objs.objs[o].name, objs.objs[o].funcs.funcs[j]->func1[k]);
                            break;
                        case 1:
                            sprintf(cmd, "%s/%01d%s", objs.objs[o].name, i + 1, objs.objs[o].funcs.funcs[j]->func1[k]);
                            break;
                        case 2:
                            sprintf(cmd, "%s/%02d%s", objs.objs[o].name, i + 1, objs.objs[o].funcs.funcs[j]->func1[k]);
                            break;
                    }
                    x32->Send(cmd);
                    if (!x32->IsConnected()) {
                        std::cerr << "OscCache::ReadAllFromMixer failed." << std::endl;
                        return;
                    }
                    while (x32->GetCachedMessage(cmd) == nullptr) {
                        usleep(100);
                    }
                }
            }
        }
    }
    std::cout << "Cache loaded with " << m_cache.size() << " elements." << std::endl;
}

void OscCache::WriteAllToMixer(IOMixer* x32) {
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        IOscMessage* msg = it->second;
        switch (msg->GetTypes()[0]) {
            case 's':
                x32->SendString(msg->GetPath(), msg->GetVal(0)->GetString());
                break;
            case 'i':
                x32->SendInt(msg->GetPath(), msg->GetVal(0)->GetInteger());
                break;
            case 'f':
                x32->SendFloat(msg->GetPath(), msg->GetVal(0)->GetFloat());
                break;
        }
        usleep(200);
    }
}

void OscCache::Dump() {
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        IOscMessage* msg = it->second;
        std::cout << "Send " << msg->GetPath() << " : " << msg->GetTypes()[0] << " : ";
        
        switch (msg->GetTypes()[0]) {
            case 's':
                std::cout << msg->GetVal(0)->GetString() << std::endl;
                break;
            case 'i':
                std::cout << msg->GetVal(0)->GetInteger() << std::endl;
                break;
            case 'f':
                std::cout << msg->GetVal(0)->GetFloat() << std::endl;
                break;
        }
        usleep(500);
    }
}