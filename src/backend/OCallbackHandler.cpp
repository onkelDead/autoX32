/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCallbackHandler.cpp
 * Author: onkel
 * 
 * Created on March 19, 2022, 9:43 AM
 */

#include <iostream>
#include "OCallbackHandler.h"

OCallbackHandler::OCallbackHandler() {
}

OCallbackHandler::OCallbackHandler(const OCallbackHandler& orig) {
}

OCallbackHandler::~OCallbackHandler() {
}

int OCallbackHandler::NewMessageCallback(IOscMessage* msg) {
    
    std::cout << "OCallbackHandler: NewMessageCallback called." << std::endl;
    return 0;
}

int OCallbackHandler::UpdateMessageCallback(IOscMessage*) {
    std::cout << "OCallbackHandler: NewValueCallback called." << std::endl;
    return 0;
}