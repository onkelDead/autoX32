/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   test_trackstore.cpp
 * Author: onkel
 *
 * Created on December 8, 2021, 9:01 AM
 */

#include <stdlib.h>
#include <iostream>
#include "../src/OTrackStore.h"

/*
 * Simple C++ Test Suite
 */

#define TEST_EQUAL(a, b, c, d) \
    if ((a) != (b)) { \
        c = EXIT_FAILURE; \
        std::cout << "Failed: " << d << std::endl; \
    }

int test1() {
    int result = EXIT_SUCCESS;
    
    std::cout << "test_trackstore test 1" << std::endl;
    OscCmd* cmd = new OscCmd("test-path", "test-types");
    OTrackStore* ts = new OTrackStore(cmd);
    ts->m_record = true;
    ts->m_playing = true;
    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
    track_entry* e = ts->NewEntry(1);
    ts->AddTimePoint(e);
    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
    ts->AddEntry(cmd, 2);
    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
    TEST_EQUAL(ts->GetCountEntries(), 3, result, "CountEntries != 3");
    TEST_EQUAL(ts->GetOscCommand(), cmd, result, "GetOscCommand != cmd");
    TEST_EQUAL(e, ts->GetEntry(1), result, "e != GetEntry(1)");
    ts->UpdatePlayhead(1, true);
    TEST_EQUAL(e, ts->GetPlayhead(), result, "e != Playhead");
    ts->SaveData("/tmp/test_trackstore");
    OscCmd* cmd1 = new OscCmd("test-path", "test-types");
    OTrackStore* ts1 = new OTrackStore(cmd1);
    std::cout << "Load Data" << std::endl;
    ts1->LoadData("/tmp/test_trackstore");
    std::cout << "Count entries ts1 " << ts1->GetCountEntries() << std::endl;
    TEST_EQUAL(ts->GetCountEntries(), ts1->GetCountEntries(), result, "ts->GetEntries != ts1->GetEnbtries");
    ts->RemoveEntry(e);
    TEST_EQUAL(ts->GetCountEntries(), ts1->GetCountEntries() - 1, result, "ts->GetEntries != ts1->GetEnbtries");
    delete ts;
    delete cmd;
    delete ts1;
    delete cmd1;
    ts = NULL;
    cmd = NULL;
    return result;
}

void test2() {
    std::cout << "test_trackstore test 2" << std::endl;
    std::cout << "%TEST_FAILED% time=0 testname=test2 (test_trackstore) message=error message sample" << std::endl;
}

int main(int argc, char** argv) {
    int result = EXIT_SUCCESS;

    std::cout << "TEST_STARTED test1 (test_trackstore)" << std::endl;
    result |= test1();
    std::cout << "TEST_FINISHED test1 (test_trackstore)" << std::endl;


    return (result);
}

