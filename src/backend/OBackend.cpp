/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <time.h>
#include "IOBackend.h"

time_t t;

ctl_command s_stop_on = {
    3,
    { 0x90, 0x5d, 0x41}, false
};

ctl_command s_stop_off = {
    3,
    { 0x90, 0x5d, 0x00}, false
};

ctl_command s_play_on = {
    3,
    { 0x90, 0x5e, 0x41}, false
};
ctl_command s_play_off = {
    3,
    { 0x90, 0x5e, 0x00}, false
};
ctl_command s_rec_on = {
    3,
    { 0x90, 0x5f, 0x41}, false
};
ctl_command s_rec_off = {
    3,
    { 0x90, 0x5f, 0x00}, false
};
ctl_command s_f1_on = {
    3,
    { 0x90, 0x36, 0x40}, false
};
ctl_command s_f1_off = {
    3,
    { 0x90, 0x36, 0x00}, false
};
ctl_command s_scrub_on = {
    3,
    { 0x90, 0x65, 0x7f}, false
};
ctl_command s_scrub_off = {
    3,
    { 0x90, 0x65, 0x00}, false
};
ctl_command s_wheel_mode_on = {
    3,
    { 0x90, 0x64, 0x40}, false
};
ctl_command s_wheel_mode_off = {
    3,
    { 0x90, 0x64, 0x00}, false
};

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend) {
    if (len == 3) {

        if (data[0] == 0x90) {
            switch (data[1]) {
                case 0x5e: // PLAY
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_PLAY);
                    }
                    break;
                case 0x5d: // STOP
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_STOP);
                    }
                    break;

                case 0x5f: // Toggle teach
                    if (data[2] == 0x7f)
                        backend->Notify(CTL_TEACH_ON);
                    break;
                case 0x36:
                    if (data[2]) {
                        backend->Notify(CTL_TEACH_MODE);
                    }
                    break;
                case 0x65:
                    if (data[2]) {
                        backend->Notify(CTL_SCRUB_ON);
                    }
                    break;
                case 0x68:
                    backend->m_fader_touched = true;
                    break;
                case 0x60:
                    if (data[2])
                        backend->Notify(CTL_PREV_TRACK);
                    break;
                case 0x61:
                    if (data[2])
                        backend->Notify(CTL_NEXT_TRACK);
                    break;
                case 0x64:
                    backend->m_wheel_mode = !backend->m_wheel_mode;
                    backend->Notify(CTL_WHEEL_MODE);
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
                case 0x5b:
                    if (data[2]) {
                        backend->Notify(CTL_HOME);
                    }
                    break;
            }
            return 1;
        }

        if (data[0] == 0x80) {
            switch (data[1]) {
                case 0x65:
                    backend->Notify(CTL_SCRUB_OFF);
                    break;
                case 0x68:
                    backend->m_fader_touched = false;
                    backend->Notify(CTL_TOUCH_RELEASE);
                    break;
                case 0x5f:
                    backend->Notify(CTL_TEACH_OFF);
                    break;
//                case 0x64:
//                    backend->m_wheel_mode = false;
//                    backend->Notify(CTL_WHEEL_MODE);
//                    break;                    
            }
        }
        if (data[0] == 0xe0) {
            if (backend->m_fader_touched) {
                uint16_t v = data[2] << 7;
                v += (data[1] << 1);
                backend->m_fader_val = v >> 7;
                backend->Notify(CTL_FADER);
            }
        }
        if (data[0] == 0xb0) {
            if (data[1] = 0x3c) {
                if (data[2] == 0x41) {
                    if (!backend->m_wheel_mode)
                        backend->Notify(CTL_JUMP_BACKWARD);
                    else
                        backend->Notify(CTL_PREV_TRACK);
                }
                if (data[2] == 0x01) {
                    if (!backend->m_wheel_mode)
                        backend->Notify(CTL_JUMP_FORWARD);
                    else
                        backend->Notify(CTL_NEXT_TRACK);
                }
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
    uint8_t s;

    if (data[0] == 0xf1) {
        backend->QuarterFrame(data[1]);
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
