/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OQueue.cpp
 * Author: onkel
 * 
 * Created on December 26, 2020, 8:16 AM
 */

#include "OQueue.h"

OQueue::OQueue() {
}

OQueue::OQueue(const OQueue& orig) {
}

OQueue::~OQueue() {
}

void OQueue::push(OscCmd* t) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(t);
}

void OQueue::front_pop(OscCmd** result) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        *result = NULL;
        return;
    }
    *result = m_queue.front();
    m_queue.pop();
}
