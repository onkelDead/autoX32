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
#include "IOBackend.h"
#include "IOMainWnd.h"


class OJack : public IOBackend {
public:
    OJack() : m_millis(0), m_jack_client(NULL) {};
    virtual ~OJack() {};

    void Connect(IOMainWnd* wnd);
    void Start();
    void ReconnectPorts();

    void Play();
    void Stop();
    void ControllerShowPlay();
    void ControllerShowStop();
    void ControllerShowRecOn();
    void ControllerShowRecOff();
    void ControllerShowTeachMode(bool);
        
    void ControllerShowLCDName(std::string);
    void ControllerShowLevel(float);
    
    void ControlerShowMtcComplete(uint8_t);
    void ControlerShowMtcQuarter(uint8_t);
    void ControllerShowScrub();
    
    void ControllerShowWheelMode();
    
    void Locate(gint);
    void Shuffle(bool);

    OMidiMtc* GetMidiMtc() { return &m_midi_mtc; }
    
    int GetMillis();
    void SetFrame(gint);
    void QuarterFrame(uint8_t);
    
    std::string GetTimeCode();
    uint8_t* GetTimeDiggits();
    
    void LoopStart();
    void LoopEnd();
    bool GetLoopState();
    void SetLoopState(bool);


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

    OMidiMtc m_midi_mtc;
    
};


#endif /* SRC_OJACK_H_ */
