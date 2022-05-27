/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OTimerEvent.h
 * Author: onkel
 *
 * Created on May 26, 2022, 5:24 PM
 */

#ifndef OTIMEREVENT_H
#define OTIMEREVENT_H

class IOTimerEvent {
public:
    virtual void OnTimer(void*) = 0;
    
};

#endif /* OTIMEREVENT_H */

