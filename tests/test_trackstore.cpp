/*
 Copyright 2021 Detlef Urban <onkel@paraair.de>

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


#include <stdlib.h>
#include <iostream>
#include "IOTrackStore.h"

#define TEST2_NUM_ENTRIES 100000

#define TEST_EQUAL(a, b, c, d) \
    if ((a) != (b)) { \
        c = EXIT_FAILURE; \
        std::cout << "Failed: " << a << " != " << b << std::endl; \
    }

int test1() {
    int result = EXIT_SUCCESS;
    
//    std::cout << "test_trackstore test 1" << std::endl;
//    OscCmd* cmd = new OscCmd("test-path", "test-types");
//    OTrackStore* ts = new OTrackStore(cmd);
//    ts->SetRecording(true);
//    ts->SetPlaying(true);
//    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
//    track_entry* e = ts->NewEntry(1);
//    ts->AddTimePoint(e);
//    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
//    ts->AddEntry(cmd, 2);
//    std::cout << "Count entries ts " << ts->GetCountEntries() << std::endl;
//    TEST_EQUAL(ts->GetCountEntries(), 3, result, "CountEntries != 3");
//    TEST_EQUAL(ts->GetOscCommand(), cmd, result, "GetOscCommand != cmd");
//    TEST_EQUAL(e, ts->GetEntryAtPosition(1), result, "e != GetEntry(1)");
//    ts->UpdatePlayhead(1, true);
//    TEST_EQUAL(e, ts->GetPlayhead(), result, "e != Playhead");
//    ts->SaveData("/tmp/test.xml");
//    OscCmd* cmd1 = new OscCmd("test-path", "test-types");
//    OTrackStore* ts1 = new OTrackStore(cmd1);
//    std::cout << "Load Data" << std::endl;
//    ts1->LoadData("/tmp/test.xml");
//    std::cout << "Count entries ts1 " << ts1->GetCountEntries() << std::endl;
//    TEST_EQUAL(ts->GetCountEntries(), ts1->GetCountEntries(), result, "ts->GetEntries != ts1->GetEnbtries");
//    ts->RemoveEntry(e);
//    TEST_EQUAL(ts->GetCountEntries(), ts1->GetCountEntries() - 1, result, "ts->GetEntries != ts1->GetEnbtries");
//    delete ts;
//    delete cmd;
//    delete ts1;
//    delete cmd1;
//    ts = NULL;
//    cmd = NULL;
//    
//    unlink("/tmp/test-path.dat");
    return result;
}

int test2() {
    int result = EXIT_SUCCESS;
//    std::cout << "test_trackstore test 2" << std::endl;
//    std::cout << "TEST2_NUM_ENTRIES = " << TEST2_NUM_ENTRIES << std::endl;
//    std::cout << "measure time for adding TEST2_NUM_ENTRIES entries with incremental positions" << std::endl;
//    
//    // prepare track store
//    OscCmd* cmd = new OscCmd("test-path", "test-types");
//    OTrackStore* ts = new OTrackStore(cmd);
//    ts->SetRecording(true);
//    ts->SetPlaying(true);
//
//    // store begin time
//    clock_t begin = clock();
//    
//    for (int i = 1; i <= TEST2_NUM_ENTRIES; i++) {
//        ts->AddEntry(cmd, i);
//    }
//    
//    TEST_EQUAL(TEST2_NUM_ENTRIES+1, ts->GetCountEntries(), result, "TEST2_NUM_ENTRIES != ts->GetCountEntries()")
//    
//    clock_t end = clock();
//    double time_spent = (double)(end - begin); //in microseconds
//    
//    std::cout << "elapse time: " << time_spent << " µsec." << std::endl;
//    
//    int count = 0;
//    int errors = 0;
//    
//    ts->CheckData(&count, &errors);
//    std::cout << "CheckData: count " << count << " entries " << std::endl;    
//    TEST_EQUAL(errors, 0, result, "ts->CheckData() failed with errors.")
//    
//    delete ts;
//    delete cmd;
//    
//    cmd = new OscCmd("test-path", "test-types");
//    ts = new OTrackStore(cmd);
//    ts->SetRecording(true);
//    ts->SetPlaying(true);    
//    
//    std::cout << std::endl << "measure time for adding TEST2_NUM_ENTRIES entries with random positions" << std::endl;
//    begin = clock();
//    
//    for (int i = 1; i <= TEST2_NUM_ENTRIES; i++) {
//        ts->UpdatePlayhead(rand(), true);
//    }
//    end = clock();
//    time_spent = (double)(end - begin); //in microseconds
//    std::cout << "elapse time: " << time_spent << " µsec." << std::endl;
//
//    std::cout << std::endl << "measure time for TEST2_NUM_ENTRIES update positions" << std::endl;
//    begin = clock();
//    
//    for (int i = 1; i <= TEST2_NUM_ENTRIES; i++) {
//        ts->UpdatePlayhead(rand(), true);
//    }
//    end = clock();
//    time_spent = (double)(end - begin); //in microseconds
//    std::cout << "elapse time: " << time_spent << " µsec." << std::endl;
//
//    count = 0;
//    errors = 0;
//    
//    ts->CheckData(&count, &errors);
//    std::cout << "CheckData: count " << count << " entries " << std::endl;
//    TEST_EQUAL(errors, 0, result, "ts->CheckData() failed with errors.")
//    
//    
//    delete ts;
//    delete cmd;
//    
    return result;
}

int main(int argc, char** argv) {
    int result = EXIT_SUCCESS;

    std::cout << "TEST_STARTED test1 (test_trackstore)" << std::endl;
    result |= test1();
    std::cout << "TEST_FINISHED test1 (test_trackstore)" << std::endl;


    std::cout << "TEST_STARTED test2 (test_trackstore)" << std::endl;
    result |= test2();
    std::cout << "TEST_FINISHED test2 (test_trackstore)" << std::endl;
    
    return (result);
}

