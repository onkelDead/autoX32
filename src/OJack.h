/*
  Copyright 2021 Detlef Urban <onkel@paraair.de>

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
        void SetFrame(gint);
	gint GetMillis();
	std::string GetTimeCode();
        bool m_edge_sec = false;

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
    void ReconnectPorts();

    void Play();
    void ControllerShowPlay();
    void Stop();
    void ControllerShowStop();
    
    void ControllerShowTeachOn();
    void ControllerShowTeachOff();

    void Locate(gint);

    gint GetMillis();
    void SetFrame(gint);

    std::string GetTimeCode();
    
    void LoopStart();
    void LoopEnd();
    bool GetLoopState();
    void SetLoopState(bool);

    OJackMtc m_jackMtc;

    jack_client_t *m_jack_client;

    gint m_millis;
    std::string m_timecode;

    void Notify(JACK_EVENT event);

    IOMainWnd* m_parent = nullptr;
    
    bool m_reconnect_mtc_out = true;
    bool m_reconnect_mmc_out = true;
    bool m_reconnect_mmc_in = true;
    bool m_reconnect_ctl_out = false;
    bool m_reconnect_ctl_in = false;
    
private:
    bool m_loop_state = false;

};


#endif /* SRC_OJACK_H_ */
