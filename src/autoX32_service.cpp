/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <signal.h>
#include <stdlib.h>

#include "IOMixer.h"
#include "IODawHandler.h"
#include "IOBackend.h"
#include "IOProject.h"
#include "OService.h"
#include "OX32.h"
#include "ODAW.h"
#include "OJack.h"

OConfig *config;
OService *service;
IOMixer *mixer;
ODAW *daw;
IOBackend *backend;



static void signal_handler(int sig) {
    daw->Disconnect();
    mixer->Disconnect();
    backend->Disconnect();            

    delete service;
    delete backend;
    delete daw;
    delete mixer;
    delete config;    
    exit(0);
}

int main_service(int argc, char** argv) {
    
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);    
    
    
    config = new OConfig();
    service = new OService();
    
    
    mixer = new OX32();
    service->SetMixer(mixer);
    if (mixer->Connect(config->get_string(SETTINGS_MIXER_HOST))) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << config->get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete mixer;
        delete config;
        return 1;
    }
    mixer->SetMessageHandler(service);
    
    if (!service->CheckArdourRecent())
        mixer->ReadAll();
    
    backend = new OJack(config);
    
    daw = new ODAW();
    service->SetDaw(daw);
    if (daw->Connect(config->get_string(SETTINGS_DAW_HOST), config->get_string(SETTINGS_DAW_PORT), config->get_string(SETTINGS_DAW__REPLAY_PORT), service)) {
        std::cerr << "autoX32_service ERROR: unable to connect to mixer at address " << config->get_string(SETTINGS_MIXER_HOST) << std::endl;
        delete daw;
        delete mixer;
        delete config;
        return 1;
    }
    
    service->SetBackend(backend);
    
    service->StartProcessing();
    
    service->Save();
    
    daw->Disconnect();
    mixer->Disconnect();
            
    delete service;
    delete backend;
    delete daw;
    delete mixer;
    delete config;
    
    return 0;
}