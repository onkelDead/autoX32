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

#include <time.h>
#include "IOBackend.h"

static auto time_f6 = std::chrono::steady_clock::now();

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend) {
    if (len == 3) {

        if (data[0] == 0xb0) {
            switch (data[1]) {
                case CTL_BUTTON_PLAY: // PLAY
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_PLAY);
                    }
                    break;
                case CTL_BUTTON_STOP: // STOP
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_STOP);
                    }
                    break;

                case CTL_BUTTON_TEACH: // Toggle teach
                    if (data[2] == 0x7f)
                        backend->Notify(CTL_TEACH_PRESS);
                    else 
                        backend->Notify(CTL_TEACH_RELEASE);
                    break;
                case CTL_BUTTON_F1:
                    if (data[2]) {
                        backend->Notify(CTL_TEACH_MODE);
                    }
                    break;
                case CTL_BUTTON_F2:
                    if (data[2]) {
                        backend->Notify(CTL_STEP_MODE);
                    }
                    break;
                case CTL_FADER_TOUCH:
                    if (backend->m_fader_touched) {
                        backend->m_fader_val = data[2];
                        backend->Notify(CTL_FADER);
                    }
                    break;
                case CTL_BUTTON_F6:
                    if (data[2]) {
                        time_f6 = std::chrono::steady_clock::now();
                    }
                    if (!data[2]) {
                        if (time_f6 + std::chrono::milliseconds(1000) < std::chrono::steady_clock::now())
                            backend->Notify(CTL_SHUTDOWN);
                        else
                            backend->Notify(CTL_SAVE);
                    }
                    break;
                case CTL_BUTTON_MARKER:  //Marker
                    backend->m_marker = data[2] != 0;
                    backend->Notify(CTL_MARKER);
                    break;
                case CTL_CURSOR_UP:
                    if (data[2])
                        backend->Notify(CTL_PREV_TRACK);
                    break;
                case CTL_CURSOR_DOWN:
                    if (data[2])
                        backend->Notify(CTL_NEXT_TRACK);
                    break;
                case CTL_BUTTON_SCRUB:
                    if (data[2]) {
                        backend->Notify(CTL_WHEEL_MODE);
                    }
                    break;
                case CTL_BUTTON_START:
                    if (data[2]) {
                        if (!backend->m_marker)
                            backend->Notify(CTL_HOME);
                        else
                            backend->Notify(CTL_LOOP_START);
                    }
                    break;
                case CTL_BUTTON_END:
                    if (data[2]) {
                        if (!backend->m_marker)
                            backend->Notify(CTL_END);
                        else
                            backend->Notify(CTL_LOOP_END);
                    }
                    break;
                case CTL_BUTTON_CYCLE:
                    if (data[2]) {
                        backend->m_cycle = !backend->m_cycle;
                        backend->Notify(CTL_LOOP);
                    }
                    break;
                case CTL_BUTTON_SELECT:
                    if (data[2]) {
                        backend->Notify(CTL_UNSELECT);
                    }
                    break; 
                case CTL_BUTTON_REC:
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_TOGGLE_REC);
                    }
                    break;  
                case CTL_WHEEL_LEFT_RIGHT:
                    if (data[2] == 0x01) {
                        backend->Notify(CTL_WHEEL_LEFT);
                    }
                    if (data[2] == 0x41) {
                        backend->Notify(CTL_WHEEL_RIGHT);
                    }                    
                    break;
                case CTL_BUTTON_DROP:
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_DROP_TRACK);
                    }
                    break;
                case CTL_BUTTON_KNOB:
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_KNOB);
                    }
                    break;
                default:
                    printf("uncaught 0xb0 0x%02x\n", data[1]);
                    break;
            }
            return 1;
        }

        if (data[0] == 0x80) {
            switch (data[1]) {
                case 0x65:
                    backend->Notify(CTL_SCRUB_OFF);
                    break;
                case 0x6e:
                    backend->m_fader_touched = false;
                    backend->Notify(CTL_TOUCH_RELEASE);
                    break;
                case 0x5f:
                    backend->Notify(CTL_TEACH_RELEASE);
                    break;
                default:
                    printf("uncaught 0x80 %02x\n", data[1]);
                    break;                    
            }
        }
        if (data[0] == 0x90) {
            if (data[1] == 0x6e) {
                backend->m_fader_touched = true;
            }
        }
    }
    return 0;
}

int process_mmc_event(uint8_t* data, size_t len, IOBackend* backend) {

    if (len > 4) {
        switch (data[4]) {
            case 1:
                backend->Notify(MMC_STOP);
                break;
            case 2:
                backend->Notify(MMC_PLAY);
                break;
            case 3:
                backend->Notify(MMC_PLAY);

                break;
            case 0x44:
                break;
        }
        return 1;
    }
    return 0;
}

int process_mtc_event(uint8_t* data, IOBackend* backend) {

    if (data[0] == 0xf1) {
        backend->QuarterFrame(data[1]);
        backend->Notify(MTC_QUARTER_FRAME);
    } else if (data[0] == 0xf0) {
        backend->GetMidiMtc()->FullFrame(data);
        backend->Notify(MTC_COMPLETE);
    }
    return 1;
}
