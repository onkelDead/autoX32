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

#define TIMEVAL_MILLIS(x) ((x).tv_usec / 1000 + (x).tv_sec * 1000)

OTimer::OTimer() {
}

OTimer::OTimer(std::function<void(void*)> task_function, const long &interval, void *userData) :
		m_userData(userData), m_task_function(task_function), m_interval(interval) {
}

OTimer::~OTimer() {
	stop();
}

void OTimer::start() {
	m_running = true;

	ue.what = UI_EVENTS::load;
	ue.with = nullptr;

	int gap = 0;

	gettimeofday(&m_starttime, NULL);

	m_thread = std::thread([&]() {
		while (m_running) {
			struct timeval now;
			struct timeval post;
			struct timeval duration;
			auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);
			gettimeofday(&now, NULL);

			if (m_active) {
				struct timeval diff_since_start;
				timersub(&now, &m_starttime, &diff_since_start);
				m_run_time_milli_sec = TIMEVAL_MILLIS(diff_since_start);
			}

			// time synch
			if (m_timerequest != 0) {
				m_posmillis = m_timerequest;
				m_timerequest = 0;
			}

			// task execution
			m_task_function(m_userData);
			gettimeofday(&post, NULL);
			timersub(&post, &now, &duration);
			m_load = (float) TIMEVAL_MILLIS(duration) / (float) m_interval;

			// update current local time
			if (m_active) {
				m_posmillis += m_interval;
			}


			std::this_thread::sleep_until(delta);
		}
	});
	m_stopped = true;
	m_thread.detach();
}

void OTimer::stop() {
	m_running = false;
	while(!m_stopped);
//	m_thread.~thread();
	m_load = 0.;
}

void OTimer::restart() {
	stop();
	start();
}

bool OTimer::isRunning() {
	return m_running;
}

int OTimer::GetStepCount() {
	int result = m_stepcount;
	m_stepcount = 0;
	return result;
}

void OTimer::SetTimeRequest(int request) {
	m_timerequest = request;
}

OTimer* OTimer::setFunc(std::function<void(void*)> func) {
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

void OTimer::SetSecDivide(int val) {
	m_secdivide = val;
}

void OTimer::SetPosMillis(int millis) {
	m_posmillis = millis;
}

int OTimer::GetPosMillis() {
	return m_posmillis;
}

float OTimer::GetLoad() {
	return m_load;
}

int OTimer::GetRunTime() {
	return m_run_time_milli_sec;
}

void OTimer::SetActive(bool val) {
	m_active = val;
}

bool OTimer::GetActive() {
	return m_active;
}
