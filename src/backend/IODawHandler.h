/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IODawHandler.h
 * Author: onkel
 *
 * Created on May 27, 2022, 8:24 AM
 */

#ifndef IODAWHANDLER_H
#define IODAWHANDLER_H

#include "OTypes.h"

class IODawHandler {
public:
    virtual ~IODawHandler(){}
    virtual void notify_daw(DAW_PATH) = 0;
};

#endif /* IODAWHANDLER_H */

