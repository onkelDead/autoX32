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
#include "OscCmd.h"
#include "OX32.h"

/*
 * Simple C++ Test Suite
 */

static void mixer_callback(OscCmd* oscCmd, void* user_data) {
    std::cout << "check_OscCmd: mixer_callback: path: " << oscCmd->GetPath() << std::endl;
    std::cout << "check_OscCmd: mixer_callback: types: " << oscCmd->GetTypes() << std::endl;
    const char* c = oscCmd->GetTypes().c_str();
    switch(c[0]) {
        case 'f':
            std::cout << "check_OscCmd: mixer_callback: float: " << oscCmd->GetLastFloat() << std::endl;
            break;
        case 'i':
            std::cout << "check_OscCmd: mixer_callback: float: " << oscCmd->GetLastInt() << std::endl;
            break;
        case 's':
            std::cout << "check_OscCmd: mixer_callback: float: " << oscCmd->GetLastStr() << std::endl;
            break;
           
    }
    oscCmd->Parse();

    std::cout << "check_OscCmd: mixer_callback: count path elements: " << oscCmd->NumPathElements() << std::endl;


}

static const unsigned char entry1[28] = {
    0x2f, 0x63, 0x68, 0x2f, 0x31, 0x36, 0x2f, 0x6d, 0x69, 0x78, 0x2f, 0x66, 0x61, 0x64, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x66, 0x00, 0x00, 0x3f, 0x25, 0xe9, 0x7a
};

int test1() {
    std::cout << "test_OscCmd test 1" << std::endl;
    OscCmd* oscCmd = new OscCmd((const char*) entry1, "f");

    std::cout << "check_OscCmd: test1: path: " << oscCmd->GetPath() << std::endl;

    delete oscCmd;
    return EXIT_SUCCESS;
}

int test2() {
    std::cout << "test_OscCmd test 2" << std::endl;
    OX32* x32 = new OX32();

    x32->SetMixerCallback(mixer_callback, NULL);
    x32->Connect("192.168.178.43");
    sleep(1);
    x32->Send("/info");
    sleep(1);
    x32->Disconnect();
    delete x32;
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {

    int result = EXIT_SUCCESS;

    std::cout << "%TEST_STARTED% test1 (test_OscCmd)" << std::endl;
    result |= test1();
    result |= test2();
    std::cout << "%TEST_FINISHED% time=0 test1 (test_OscCmd)" << std::endl;

    return (EXIT_SUCCESS);
}

