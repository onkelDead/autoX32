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

OService *service;

static void signal_handler(int sig) {
    service->Save();
    delete service;
    exit(0);
}

int main_service(int argc, char** argv) {
    
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);    
    
    service = new OService();
    
    if (service->InitMixer()) {
        std::cerr << "Failed to initialize mixer" << std::endl;
        delete service;
        return EXIT_FAILURE;
    }
    
    if (service->InitBackend()) {
        std::cerr << "Failed to initialize Backend" << std::endl;
        delete service;
        return EXIT_FAILURE;
    }

    if (service->InitDaw()) {
        std::cerr << "Failed to initialize DAW" << std::endl;
        delete service;
        return EXIT_FAILURE;
    }
    
    service->StartProcessing();
    
    service->Save();
    
    delete service;
    
    return EXIT_SUCCESS;
}