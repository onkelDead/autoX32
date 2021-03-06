/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OQueue.h
 * Author: onkel
 *
 * Created on December 26, 2020, 8:16 AM
 */

#ifndef OQUEUE_H
#define OQUEUE_H

#include <queue>
#include <mutex>

#include "OTypes.h"

class OQueue {
public:
    OQueue();
    OQueue(const OQueue& orig);
    virtual ~OQueue();
    
    void push(ui_event*);
    void front_pop(ui_event**);
    
private:

    std::queue<ui_event*> m_queue;
    mutable std::mutex m_mutex;
    
};

#endif /* OQUEUE_H */

