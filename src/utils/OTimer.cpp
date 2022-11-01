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
#include "OTypes.h"
#include "OMainWnd.h"

OTimer::OTimer() {
    m_stopped = true;
}

OTimer::OTimer(IOTimerEvent* task_function, const long &interval, IOTimerEvent *userData) :
    m_interval(interval), m_stopped(true), m_userData(userData) {
    assert(task_function != nullptr);
}

OTimer::~OTimer() {
    if (m_running) 
        stop();
}

void OTimer::start() {
    if (m_running == true) {
        std::cout << "Timer already running, do nothing." << std::endl;
        return;
    }
    m_running = true;

    gettimeofday(&m_starttime, NULL);

    m_thread = new std::thread([&]() {
        while (m_running) {
            auto now = std::chrono::steady_clock::now();
            auto delta = now + std::chrono::milliseconds(m_interval);

            // task execution
            m_task_function->OnTimer(m_userData);

            std::this_thread::sleep_until(delta);
        }
        m_stopped = true;
    });
}

void OTimer::stop() {
    if (m_running) {
        m_running = false;
        while (!m_stopped);
        m_thread->join();
        delete m_thread;
    }
    else {
        std::cout << "Timer not running, do nothing." << std::endl;
    }
}

void OTimer::restart() {
    stop();
    start();
}

bool OTimer::isRunning() {
    return m_running;
}

OTimer* OTimer::setFunc(IOTimerEvent* func) {
    m_task_function = func;
    return this;
}

OTimer* OTimer::SetUserData(void *userData) {
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