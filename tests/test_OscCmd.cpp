/*
  Copyright 2022 Detlef Urban <onkel@paraair.de>

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
#include <vector>
#include <cassert>

#include "ODAW.h"
#include "OX32.h"
#include "OscCache.h"
#include "OCallbackHandler.h"
#include "OscMessage.h"
#include "OTrackStore.h"
#include "IODawHandler.h"
#include "IOBackend.h"
#include "OJack.h"
#include "OProject.h"

#define TEST_PROJECT_FILE "/home/onkel/test_autoX32"

int msg_count = 0;

static void test_equal(auto a, auto b, std::string success) {
    assert(a == b);
    std::cout << success << " PASSED." << std::endl;
}

static void test_not_equal(auto a, auto b, std::string success) {
    assert(a != b);
    std::cout << success << " PASSED." << std::endl;
}

static void test_greater(auto a, auto b, std::string success) {
    assert(a > b);
    std::cout << success << " PASSED." << std::endl;
}

class TestMessageHandler : public IOMessageHandler {
public:

    int NewMessageCallback(IOscMessage* msg) {
        if (m_show_ew_messages) {
            msg_count++;
            std::cout << msg_count << " : ";
            switch (msg->GetTypes()[0]) {
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
        }
        return 0;
    }

    int UpdateMessageCallback(IOscMessage* msg) {
        std::cout << "UpdateMessageCallback..." << std::endl;
        return 0;
    }

    void ProcessSelectMessage(int f) {
    }
    
    bool m_show_ew_messages = false;
};

class DawHandler : public IODawHandler {
public:

    virtual void notify_daw(DAW_PATH) {
        
        notify_daw_counter++;
    }
    
    int notify_daw_counter = 0;
};

class MainWnd_mock : public IOJackHandler {
public:

    void EditTrack(std::string) {};
    
    int GetPosFrame() {
        return 0;
    }

    virtual OConfig* GetConfig() {
        return &m_config;
    }    

    virtual void PublishUiEvent(E_OPERATION, void*) {};

    virtual void SelectTrack(std::string, bool) {};

    virtual void TrackViewDown(std::string){};

    virtual void TrackViewHide(std::string) {};

    virtual void TrackViewUp(std::string){};

    virtual void notify_jack(JACK_EVENT event){
//        std::cout << "notify_jack " << event << std::endl;
    };

    virtual void notify_overview(){};

    virtual void remove_track(std::string path){};

    virtual ~MainWnd_mock() {

    }
    
    OConfig m_config;

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
    TestMessageHandler *h = new TestMessageHandler();

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
    IOMixer* x32 = new OX32();
    TestMessageHandler *h = new TestMessageHandler();

    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);

    x32->ReadAll();

    sleep(1);

    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;

    x32->Disconnect();
    delete x32;


    return EXIT_SUCCESS;
}

int test_project_save() {
    IOMixer* x32 = new OX32();
    OProject* project = new OProject();
    TestMessageHandler *h = new TestMessageHandler();

    project->SetMixer(x32);
    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);

    x32->ReadAll();

    sleep(1);

    project->Save("/home/onkel/test-autoX32");

    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;

    x32->Disconnect();
    delete x32;
    delete project;


    return EXIT_SUCCESS;
}

int test_project_load() {
    IOMixer* x32 = new OX32();
    OProject* project = new OProject();
    TestMessageHandler *h = new TestMessageHandler();

    project->SetMixer(x32);
    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);

    project->Load("/home/onkel/test-autoX32");

    x32->WriteAll();

    sleep(1);


    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;

    x32->Disconnect();
    delete x32;
    delete project;


    return EXIT_SUCCESS;
}

int test_select_track(int argc, char**argv) {
    IOMixer* x32 = new OX32();
    OProject* project = new OProject();
    TestMessageHandler *h = new TestMessageHandler();

    project->SetMixer(x32);
    x32->SetMessageHandler(h);

    assert(x32->Connect("192.168.178.43") == 0);
    sleep(1);

    //    x32->SendFloat("/ch/13/mix/fader", atof(argv[1]));
    x32->SendInt("/-stat/selidx", atoi(argv[1]));

    sleep(1);


    std::cout << "test_OscCmd test 3: finally the cache contained " << x32->GetCacheSize() << " elements." << std::endl;

    x32->Disconnect();
    delete x32;
    delete project;


    return EXIT_SUCCESS;
}

int test_mixer() {
    IOMixer* x32 = new OX32();
    TestMessageHandler *handler = new TestMessageHandler();
    IOTrackStore* trackstore;
    
    std::cout << "test_OscCmd: Test Mixer started." << std::endl;
    
    // check connect
    test_equal(x32->Connect("192.168.178.43"), 0,
            "Connect X32");
    
    // check IsConnected
    test_equal(x32->IsConnected(), 1, "IsConnected");

    x32->SetMessageHandler(handler);    
    
    // check ReadAll
    x32->ReadAll();
    
    handler->m_show_ew_messages = true;
    
    IOscMessage* msg = x32->GetCachedMessage("/ch/09/mix/fader");

    trackstore = new OTrackStore(msg);
    test_not_equal(trackstore, nullptr, "new TrackStore");
    
    trackstore->SetRecording(true);
    trackstore->SetPlaying(true);
    
    
    trackstore->ProcessMsg(msg, 1);
    trackstore->ProcessMsg(msg, 2);
    trackstore->ProcessMsg(msg, 3);

    test_equal(trackstore->GetCountEntries(), 4, "count entries is 4");
    
    trackstore->SetRecording(false);

    trackstore->ProcessMsg(msg, 4);
    trackstore->ProcessMsg(msg, 5);
    trackstore->ProcessMsg(msg, 6);    

    test_equal(trackstore->GetCountEntries(), 4, "count entries is 4");
    
    test_equal(trackstore->GetPlayhead()->time, 3, "playhead is 3");

    trackstore->SetPlaying(false);
    trackstore->UpdatePos(2, true);
    
    test_equal(trackstore->GetPlayhead()->time, 2, "playhead is 2");
    
    // check disconnect
    x32->Disconnect();
    
    // check IsConnected
    test_equal(x32->IsConnected(), 0, "Disconnected X32");
    
    delete trackstore;
    delete x32;
    delete handler;
    
    std::cout << "test_OscCmd: Test Mixer finished." << std::endl;
    
    return EXIT_SUCCESS;
}

int test_daw() {
    ODAW* daw = new ODAW();
    DawHandler* dawHandler = new DawHandler();
    
    std::cout << "test_OscCmd: Test DAW started." << std::endl;

    test_equal(daw->Connect("127.0.0.1", "3819", "38200", dawHandler ), 0, "Connect DAW");
    sleep(1);
    
    test_equal(dawHandler->notify_daw_counter, 2, "notify_daw_counter: ");
    
    std::cout << "session: " << daw->GetSessionName() << std::endl;
    std::cout << "bitrate: " << daw->GetBitRate() << std::endl;
    std::cout << "maxframes: " << daw->GetMaxFrames() << std::endl;
    std::cout << "samples: " << daw->GetSample() << std::endl;
    
    test_not_equal(daw->GetBitRate(), 0, "GetBitRate");
    test_not_equal(daw->GetMaxFrames(), 0, "GetMaxFrames");
    test_not_equal(daw->GetSample(), 0, "GetSample");
    
    test_equal(daw->Disconnect(), 0, "Disconnect DAW");
    
    delete daw;
    delete dawHandler;

    std::cout << "test_OscCmd: Test DAW finished." << std::endl;
    
    return EXIT_SUCCESS;
}

int test_backend() {
    std::cout << "test_OscCmd: Test Backend started." << std::endl;
        
    OConfig config;
    IOBackend* backend = new OJack(&config);
    IOJackHandler *wnd = new MainWnd_mock();
    
    backend->Connect(wnd);
    sleep(1);
    backend->Locate(20000);
    sleep(1);
    test_equal(backend->GetFrame(), 20000, "GetFrame() "); 
    test_equal(backend->GetTimeCode(), "00:02:46:20", "GetTimeCode");
    
    backend->Play(false);
    sleep(1);
    backend->Stop(false);
    sleep(1);
    test_greater(backend->GetFrame(), 20000, "GetFrame() "); 
    
    backend->Disconnect();
    
    delete wnd;
    delete backend;

    std::cout << "test_OscCmd: Test Backend finished." << std::endl;
    
    return EXIT_SUCCESS;
}

int test_project() {
    std::cout << "test_OscCmd: Test Project started." << std::endl;
    
    TestMessageHandler *handler = new TestMessageHandler();    
    IOProject* project = new OProject();
    project->Load(TEST_PROJECT_FILE);
    
    IOMixer* x32 = new OX32();
    
    test_equal(x32->Connect("192.168.178.43"), 0, "Connect X32");
    x32->SetMessageHandler(handler);        
    project->SetMixer(x32);
    x32->ReadAll();
    
    IOTrackStore *trackstore = project->NewTrack(x32->GetCachedMessage("/ch/09/mix/fader"));
    
    project->Save(TEST_PROJECT_FILE);
    project->Close();
    project->Load(TEST_PROJECT_FILE);
    project->Close();
    
    x32->Disconnect();
    
    delete project;
    delete x32;
    delete handler;
    
    std::cout << "test_OscCmd: Test Project finished." << std::endl;
    
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {

    int result = EXIT_SUCCESS;

    if (argc > 0) {
        for (int i = 1; i < argc; i++) {
            switch (argv[i][0])  {
                case '1':
                    result |= test_mixer();
                    break;
                case '2':
                    result |= test_daw();
                    break;
                case '3':
                    result |= test_backend();
                    break;
                case '4':
                    result |= test_project();
                    break;
                default:
                    std::cout << "Unknown test index " << argv[i][0] << std::endl;
                    return EXIT_FAILURE;
            }
            sleep(1);
        }
    }
    else {
        result |= test_mixer();
        result |= test_daw();
        result |= test_backend();
        result |= test_project();
    }

    return (EXIT_SUCCESS);
}

