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

#ifndef OTIMER_H
#define OTIMER_H

#include <iostream>
#include <chrono>
#include <functional>
#include <thread>

#include "OTypes.h"
#include "IOTimer.h"
 
class OTimer : public IOTimer {
public:
    OTimer();

    OTimer(std::function<void(void*) > func, const long &interval, void* userData);

    virtual ~OTimer();

    void start();
    void stop();
    void restart();

    bool isRunning();

    OTimer *setFunc(std::function<void(void*) > func);

    OTimer *SetUserData(void*);

    long getInterval();

    OTimer *setInterval(const long &interval);
    
    void SetSecDivide(int);
    
    virtual void SetSamplePos(int);
    virtual int GetSamplePos();
    
    int GetRunTime();
    
    float GetLoad();

    char load[32];
    
    ui_event ue;
    
private:

    struct timeval m_starttime;
    int m_run_time_milli_sec; 
    int m_secdivide;
    
    int m_samplepos;
    
    float m_load;
    
    std::function<void(void*) > m_func;
    long m_interval;

    std::thread m_thread;
    bool m_running = false;

    void* m_userData;
};

#endif /* OTIMER_H */

