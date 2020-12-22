/*
  Copyright 2020 Detlef Urban <onkel@paraair.de>

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

#include <sys/time.h>
#include "OTimer.h"

OTimer::OTimer() : m_eclapse(0) {
}

OTimer::OTimer(std::function<void(void*) > func, const long &interval, void* userData) {
    m_userData = userData;
    m_func = func;
    m_interval = interval;
}

OTimer::~OTimer(){
    stop();
}

void OTimer::start() {
    m_running = true;
    gettimeofday(&m_starttime, NULL);
    m_thread = std::thread([&]() {
        while (m_running) {
            
            struct timeval now;
            struct timeval diff;
            gettimeofday(&now, NULL);
            timersub(&now, &m_starttime, &diff );

            m_eclapse = diff.tv_usec / 1000 + diff.tv_sec * 1000;

            auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);
            m_func(m_userData);

            struct timeval post;
            struct timeval duration;
            
            gettimeofday(&post, NULL);
            timersub(&post, &now, &duration );
            
            m_load = ((float)duration.tv_usec / 1000.) / ((float)m_interval) * 100;
            
            m_samplepos += m_secdivide * m_interval;
            std::this_thread::sleep_until(delta);
        }
    });
    m_thread.detach();
}

void OTimer::stop() {
    m_running = false;
    m_thread.~thread();
    m_load = 0.;
}

void OTimer::restart() {
    stop();
    start();
}

bool OTimer::isRunning() {
    return m_running;
}

OTimer* OTimer::setFunc(std::function<void(void*) > func) {
    m_func = func;
    return this;
}

OTimer *OTimer::SetUserData(void* userData) {
    m_userData = userData;
    return this;
}

long OTimer::getInterval() {
    return m_interval;
}

OTimer* OTimer::setInterval(const long &interval) {
    m_interval = interval;
    return this;
}

void OTimer::SetSecDivide(int val) {
    m_secdivide = val;
}

void OTimer::SetSamplePos(int val) {
    //printf("SetSamplePos: %d %d\n", val, m_samplepos - val);
    m_samplepos = val;
    
}

int OTimer::GetSamplePos() {
    return m_samplepos;
}

float OTimer::GetLoad() {
    return m_load;
}