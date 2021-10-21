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

class OJack {
public:
	OJack() {};
    virtual ~OJack() {};


    void Connect(IOMainWnd* wnd);

    void Play();
    void Stop();

    void Locate(gint);

    gint GetMillis();

    std::string GetTimeCode();

    jack_client_t *m_jack_client;

    gint m_millis;
    std::string m_timecode;

    void Notify(JACK_EVENT event);

    IOMainWnd* m_parent = nullptr;

};


#endif /* SRC_OJACK_H_ */
