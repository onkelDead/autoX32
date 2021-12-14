/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <time.h>
#include "IOBackend.h"

time_t t;

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend) {
    if (len == 3) {

        if (data[0] == 0xb0) {
            switch (data[1]) {
                case 1: // PLAY/STOP
                    if (data[2]) {
                        backend->Notify(CTL_PLAYSTOP);
                    }
                    break;
                case 2: // Toggle teach
                    if (data[2])
                        backend->Notify(CTL_TEACH_ON);
                    else
                        backend->Notify(CTL_TEACH_OFF);
                    break;
                case 3:
                    if (data[2]) // button on down
                        time(&t);
                    else {
                        time_t now;
                        time(&now);
                        if (now > t + 1) {
                            backend->Notify(CTL_LOOP_CLEAR);
                        } else {
                            backend->Notify(CTL_LOOP_SET);
                        }
                    }
                    break;
                case 4:
                    if (data[2]) {
                        backend->Notify(CTL_TOGGLE_LOOP);
                    }
                    break;
                case 6:
                    if (data[2]) {
                        backend->Notify(CTL_HOME);
                    }
                    break;
            }
            return 1;
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
    uint8_t s;

    if (data[0] == 0xf1) {
        backend->GetMidiMtc()->QuarterFrame(data[1]);
        if (backend->GetMidiMtc()->m_edge_sec == 1) {
            backend->Notify(MTC_QUARTER_FRAME_SEC);
        }
        if (backend->GetMidiMtc()->m_edge_sec == 2) {
            backend->Notify(MTC_QUARTER_FRAME_SEC1);
            backend->GetMidiMtc()->m_edge_sec = 0;
        } else
            backend->Notify(MTC_QUARTER_FRAME);
    } else if (data[0] == 0xf0) {
        backend->GetMidiMtc()->FullFrame(data);
        backend->Notify(MTC_COMPLETE);
    }
    return 1;
}
