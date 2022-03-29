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


static void message_callback(char* entry, size_t len, void* oCache) {
    int result;
    lo_message msg = lo_message_deserialise(entry, len, &result);
    int argc = lo_message_get_argc(msg);
    lo_arg **argv = lo_message_get_argv(msg);
    OscMessage *cmd = new OscMessage(entry, lo_message_get_types(msg));

    int i = 0;
    while(char c = cmd->GetTypes()[i]) {
        cmd->SetVal(new OscValue(*argv[i], c));
        i++;
    }
    
    ((OscCache*)oCache)->ProcessMessage(cmd);
    
    lo_message_free(msg);    
}

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
//    OscCache* oCache = new OscCache();
    //x32->SetMessageHandler(&ch);

//    x32->SetMsg_callback(message_callback, (void*)oCache);
    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);
    
    float val;
    assert(x32->GetCachedValue("/ch/14/mix/fader", &val) == false);
    x32->Send("/ch/14/mix/fader");
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

int main(int argc, char** argv) {

    int result = EXIT_SUCCESS;

    std::cout << "%TEST_STARTED% test1 (test_OscCmd)" << std::endl;
    //result |= test1();
    result |= test2();
    //result |= test3();
    std::cout << "%TEST_FINISHED% time=0 test1 (test_OscCmd)" << std::endl;

    return (EXIT_SUCCESS);
}

