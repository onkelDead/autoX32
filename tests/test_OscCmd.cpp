/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   test_OscCmd.cpp
 * Author: onkel
 *
 * Created on March 16, 2022, 8:10 PM
 */

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cassert>

//#include "OscCmd.h"
#include "OX32.h"
#include "OscCache.h"
#include "OCallbackHandler.h"
#include "OscMessage.h"


/*
 * Simple C++ Test Suite
 */


typedef struct func1_t {
    const char** func1;
    int count;
} func1_t;
typedef struct funcs_t {
    func1_t** funcs;
    int count;
} funcs_t;
typedef struct objects {
    int count;
    const char* name;
    funcs_t funcs;
    
} objects;

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
};
func1_t mix_funcs = {
    mix_func,
    32
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
    "/dyn/keysrc",
    "/dyn/mix",
    "/dyn/auto",
    "/dyn/filter/on",
    "/dyn/filter/type",
    "/dyn/filter/f",
};
func1_t dyn_funcs = {
    dyn_func,
    18
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
func1_t gate_funcs {
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
    "/mix/mono"
    "/mix/mlevel"
};
func1_t base_funcs = {
    base_func,
    8
};

func1_t* ch_func[] = {
    &base_funcs,
    &eq_funcs,
    &preamp_funcs,
    &gate_funcs,
    &dyn_funcs,
    &mix_funcs
};

func1_t* bus_func[] = {
    &base_funcs,
    &eq_funcs,    
    &dyn_funcs,
};
func1_t* auxin_func[] = {
    &base_funcs,
    &eq_funcs,
    &mix_funcs
};

funcs_t ch_funcs = {
    ch_func,
    6
};

funcs_t bus_funcs = {
    bus_func,
    3
};
funcs_t auxin_funcs = {
    auxin_func,
    3
};

objects objs[] = {
    {
        32,
        "/ch",
        ch_funcs        
    },
    {
        16,
        "/bus",
        bus_funcs
    },
    {
        8,
        "/auxin",
        auxin_funcs
    }
    
};

int msg_count = 0;
class handler : public IOMessageHandler {
    public:
    int NewMessageCallback(IOscMessage* msg) { 
        msg_count++;
        std::cout << msg_count << " : ";
        switch(msg->GetTypes()[0]) {
            case 's':
                std::cout << msg->GetPath() << " - " << msg->GetVal(0)->GetString() << std::endl;
                break;
            case 'f':
                std::cout << msg->GetPath() << " - " << msg->GetVal(0)->GetFloat() << std::endl;
                break;
            case 'i':
                std::cout << msg->GetPath() << " - " << msg->GetVal(0)->GetInteger() << std::endl;
                break;
        }
        return 0;
    }
    int UpdateMessageCallback(IOscMessage* msg) {
        return 0;
    }      
};


static const unsigned char entry1[28] = {
    0x2f, 0x63, 0x68, 0x2f, 0x31, 0x36, 0x2f, 0x6d, 0x69, 0x78, 0x2f, 0x66, 0x61, 0x64, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x66, 0x00, 0x00, 0x3f, 0x25, 0xe9, 0x7a
};

int test1() {
    std::cout << "test_OscCmd test 1" << std::endl;
//    OscCmd* oscCmd = new OscCmd((const char*) entry1, "f");
//
//    std::cout << "check_OscCmd: test1: path: " << oscCmd->GetPath() << std::endl;
//
//    delete oscCmd;
    return EXIT_SUCCESS;
}

int test2() {
    std::cout << "test_OscCmd test 2" << std::endl;
    OX32* x32 = new OX32();
    OCallbackHandler ch;

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);
    
    float val;
    assert(x32->GetCachedValue("/ch/14/mix/*", &val) == false);
    x32->Send("/ch/14/mix/*");
    sleep(5);
    assert(x32->GetCachedValue("/ch/14/mix/fader", &val) == true);
    std::cout << "test 2: got cached float value " << val << std::endl;
    
    int integer;
    assert(x32->GetCachedValue("/ch/14/config/color", &integer) == false);
    x32->Send("/ch/14/config/color");
    sleep(1);
    assert(x32->GetCachedValue("/ch/14/config/color", &integer) == true);
    std::cout << "test 2: got cached int value " << integer << std::endl;
   
    std::string name;
    assert(x32->GetCachedValue("/ch/14/config/name", &name) == false);
    x32->Send("/ch/14/config/name");
    sleep(1);
    assert(x32->GetCachedValue("/ch/14/config/name", &name) == true);
    std::cout << "test 2: got cached string value " << name << std::endl;
    
    std::cout << "test_OscCmd test 2: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;
    
    x32->Disconnect();
    delete x32;

    return EXIT_SUCCESS;
}

int test3() {
    std::cout << "test_OscCmd test 3" << std::endl;
    OX32* x32 = new OX32();

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);
    
    float val;
    assert(x32->GetCachedValue("/ch/14/mix/fader", &val) == false);
    x32->Send("/ch/14/mix/fader");
    sleep(5);
    assert(x32->GetCachedValue("/ch/14/mix/fader", &val) == true);
    std::cout << "test 3: got cached float value " << val << std::endl;
    
    int integer;
    assert(x32->GetCachedValue("/ch/14/config/color", &integer) == false);
    x32->Send("/ch/14/config/color");
    sleep(1);
    assert(x32->GetCachedValue("/ch/14/config/color", &integer) == true);
    std::cout << "test 3: got cached int value " << integer << std::endl;
   
    std::string name;
    assert(x32->GetCachedValue("/ch/14/config/name", &name) == false);
    x32->Send("/ch/14/config/name");
    sleep(1);
    assert(x32->GetCachedValue("/ch/14/config/name", &name) == true);
    std::cout << "test 3: got cached string value " << name << std::endl;
    
    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;
    
    x32->Disconnect();
    delete x32;

    return EXIT_SUCCESS;
}

int test4() {
    std::cout << "test_OscCmd test 3" << std::endl;
    OX32* x32 = new OX32();
    handler *h = new handler();
    
    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);    
    
    x32->Send("/config/routing/OUT/1-4");
    sleep(1);
    
    x32->Disconnect();
    delete x32;

    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;
    
    return EXIT_SUCCESS;    
}

int test5() {
    OX32* x32 = new OX32();
    handler *h = new handler();
    
    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);    

    for (int o = 0; o < 3; o++) {
        for (int i = 0; i < objs[o].count; i++ ) {
            for (int j = 0; j < objs[o].funcs.count; j++) {
                for (int k = 0; k < objs[o].funcs.funcs[j]->count; k++) {
                    char cmd[64];
                    sprintf(cmd, "%s/%02d%s", objs[o].name, i + 1, objs[o].funcs.funcs[j]->func1[k]);
                    x32->Send(cmd);
                    usleep(100);
                }
            }
        }
    }
    sleep(1);

    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;
    
    x32->Disconnect();
    delete x32;

    
    return EXIT_SUCCESS;    
}

int main(int argc, char** argv) {

    int result = EXIT_SUCCESS;

    std::cout << " test1 (test_OscCmd)" << std::endl;
    //result |= test1();
    result |= test5();
    //result |= test3();
    std::cout << std::endl << "time=0 test1 (test_OscCmd)" << std::endl;

    
    return (EXIT_SUCCESS);
}

