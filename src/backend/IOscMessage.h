/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOscMessage.h
 * Author: onkel
 *
 * Created on March 20, 2022, 10:18 AM
 */

#ifndef IOSCMESSAGE_H
#define IOSCMESSAGE_H

#include "OscValue.h"

class IOTrackStore;

class IOscMessage {
public:
    virtual const std::string GetPath() = 0;
    virtual OscValue* GetVal(int index) const = 0;
    virtual void SetVal(OscValue*) = 0;
    virtual const char* GetTypes() const = 0;
    virtual bool IsConfig() const = 0;
    virtual std::string GetConfigRequestName() = 0;
    virtual std::string GetConfigRequestColor() = 0;
    virtual void SetTrackstore(IOTrackStore* Trackstore) = 0;
    virtual inline IOTrackStore* GetTrackstore() const = 0;
    virtual void Print() = 0;
};


#endif /* IOSCMESSAGE_H */

