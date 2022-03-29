/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOMessageHandler.h
 * Author: onkel
 *
 * Created on March 19, 2022, 11:31 AM
 */

#ifndef IOMESSAGEHANDLER_H
#define IOMESSAGEHANDLER_H

class IOscMessage;

class IOMessageHandler {
public:
    virtual int NewMessageCallback(IOscMessage*) = 0;
    virtual int UpdateMessageCallback(IOscMessage*) = 0;
};

#endif /* IOMESSAGEHANDLER_H */

