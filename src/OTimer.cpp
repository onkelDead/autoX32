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

#include "OTimer.h"

OTimer::OTimer() {
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
    m_thread = std::thread([&]() {
        while (m_running) {
            auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);
            m_func(m_userData);
            std::this_thread::sleep_until(delta);
        }
    });
    m_thread.detach();
}

void OTimer::stop() {
    m_running = false;
    m_thread.~thread();
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
