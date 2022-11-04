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

#include <jack/jack.h>
#include <jack/midiport.h>
#include "OTypes.h"
#include "IOBackend.h"
#include "IOJackHandler.h"
#include "OConfig.h"

class OJack : public IOBackend {
public:
    OJack(OConfig* config) {
        m_config = config;
    };
    virtual ~OJack() {};

    int Connect(IOJackHandler* wnd);
    void Disconnect();
    void ReconnectPorts();

    void Play();
    void Stop();
    
    void ControllerReset();
    
    void ControllerShowPlay(bool val);
    void ControllerShowDrop(bool val);
    void ControllerShowRec(bool);
    void ControllerShowTeach(bool val);
    void ControllerShowSelect(bool val);        
    void ControllerShowTeachMode(bool);
    void ControllerShowLCDName(std::string, int color);
    void ControllerShowLevel(float);
    void ControllerShowStepMode(bool val);
    
    void ControlerShowMtcComplete(uint8_t);
    void ControlerShowMtcQuarter(uint8_t);
    void ControllerShowScrub();
    void ControllerShowMarker();
    void ControllerShowCycle();
    
    void ControllerShowWheelMode(bool);
    
    void ControllerCustom(uint8_t c, uint8_t a, uint8_t b);
    void ControllerShowActive(bool);
    
    void Locate(int);
    void Shuffle(bool);

    OMidiMtc* GetMidiMtc() { return &m_midi_mtc; }
    
    int GetFrame();
    void SetFrame(int);
    void QuarterFrame(uint8_t);
    
    std::string GetTimeCode();
    uint8_t* GetTimeDiggits();
    
    void LoopStart();
    void LoopEnd();
    bool GetLoopState();
    void SetLoopState(bool);

    jack_client_t *m_jack_client = nullptr;

    std::string m_timecode;

    void Notify(JACK_EVENT event);

    IOJackHandler* m_parent = nullptr;
    
    bool m_reconnect_mtc_out = true;
    bool m_reconnect_mmc_out = true;
    bool m_reconnect_mmc_in = true;
    bool m_reconnect_ctl_out = false;
    bool m_reconnect_ctl_in = false;

    OConfig *m_config;
    
private:
    bool m_loop_state = false;

    OMidiMtc m_midi_mtc;

    
};


#endif /* SRC_OJACK_H_ */
