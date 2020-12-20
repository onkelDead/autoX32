
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

long OTimer::getInterval() {
    return m_interval;
}

OTimer* OTimer::setInterval(const long &interval) {
    m_interval = interval;
    return this;
}
