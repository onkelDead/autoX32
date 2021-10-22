/*
 * jack.h
 *
 *  Created on: Oct 20, 2021
 *      Author: onkel
 */

#ifndef SRC_OJACK_H_
#define SRC_OJACK_H_

#include <gtkmm.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include "OTypes.h"
#include "IOMainWnd.h"

class OJackMtc {
public:

	void FullFrame(uint8_t* frame_data);
	void QuarterFrame(uint8_t data);
	gint GetMillis();
	std::string GetTimeCode();

private:
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;
	uint8_t frame = 0;
	uint8_t subframe = 0;

	uint8_t n1 = 0;
	uint8_t n2 = 0;
	std::string timecode;
	gint millis;
	bool lock_millis = false;
};


class OJack {
public:
	OJack() : m_millis(0), m_jack_client(NULL) {};
    virtual ~OJack() {};


    void Connect(IOMainWnd* wnd);

    void Play();
    void Stop();

    void Locate(gint);

    gint GetMillis();

    std::string GetTimeCode();

    OJackMtc m_jackMtc;

    jack_client_t *m_jack_client;

    gint m_millis;
    std::string m_timecode;

    void Notify(JACK_EVENT event);

    IOMainWnd* m_parent = nullptr;

};


#endif /* SRC_OJACK_H_ */
