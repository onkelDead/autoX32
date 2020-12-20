
#ifndef OTIMER_H
#define OTIMER_H

#include <iostream>
#include <chrono>
#include <functional>
#include <thread>

class OTimer {

public:
	OTimer();

	OTimer(std::function<void(void*)> func, const long &interval, void* userData);

        virtual ~OTimer();

	void start();
	void stop();
	void restart();
	
	bool isRunning();


	OTimer *setFunc(std::function<void(void*)> func);
        
	long getInterval();

	OTimer *setInterval(const long &interval);

private:
	std::function<void(void*)> m_func;
	long m_interval;

	std::thread m_thread;
	bool m_running = false;
        
        void* m_userData;
};

#endif /* OTIMER_H */

