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
    { 0xB0, 0x16, 0x41}
};

ctl_command s_stop_off = {
    3,
    { 0xB0, 0x16, 0x00}
};

ctl_command s_play_on = {
    3,
    { 0xB0, 0x17, 0x41}
};
ctl_command s_play_off = {
    3,
    { 0xB0, 0x17, 0x00}
};

ctl_command s_record = {
    3,
    { 0xB0, 0x06, 0x41}
};

ctl_command s_tech_on = {
    3,
    { 0xB0, 0x18, 0x41}
};
ctl_command s_teach_off = {
    3,
    { 0xB0, 0x18, 0x00}
};
ctl_command s_f1_on = {
    3,
    { 0xB0, 0x07, 0x41}
};
ctl_command s_f1_off = {
    3,
    { 0xB0, 0x07, 0x00}
};
ctl_command s_scrub_on = {
    3,
    { 0x90, 0x65, 0x7f}
};
ctl_command s_scrub_off = {
    3,
    { 0x90, 0x65, 0x00}
};
ctl_command s_wheel_mode_on = {
    3,
    { 0xB0, 0x20, 0x40}
};
ctl_command s_wheel_mode_off = {
    3,
    { 0xB0, 0x20, 0x00}
};

ctl_command s_select_on = {
    3,
    { 0xB0, 0x03, 0x41}
};
ctl_command s_select_off = {
    3,
    { 0xB0, 0x03, 0x00}
};

ctl_command s_level = {
    3, 
    { 0xB0, 0, 0}
};

ctl_command s_lcd_1 = {
    23,
    {0, }
};

ctl_command s_lcd_2 = {
    15,
    {0, }
};

ctl_command s_mtc_full[8] = {
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
};

ctl_command s_mtc_quarter = {
    3, {0, }
};

ctl_command s_marker = {
    3, {0xb0, 0x0d, 0x00 }
};
ctl_command s_cycle = {
    3, {0xb0, 0x0f, 0x00 }
};


#define SevenSeg_0 0b00111111
#define SevenSeg_1 0b00000110
#define SevenSeg_2 0b01011011
#define SevenSeg_3 0b01001111
#define SevenSeg_4 0b01100110
#define SevenSeg_5 0b01101101
#define SevenSeg_6 0b01111101
#define SevenSeg_7 0b00000111
#define SevenSeg_8 0b01111111
#define SevenSeg_9 0b01100111

uint8_t Nibble2Seven[] = { SevenSeg_0, SevenSeg_1, SevenSeg_2, SevenSeg_3, SevenSeg_4, SevenSeg_5, SevenSeg_6, SevenSeg_7, SevenSeg_8, SevenSeg_9};

ctl_command s_7seg = {
    21, {0xf0, 0x00, 0x20, 0x32, 0x41, 0x37, 
            0x00, 0x00,         // Assignment
            0x00, 0x00, 0x00,   // Hours
            0x00, 0x00,         // Minutes
            0x00, 0x00,         // Seconds
            0x00, 0x00, 0x00,   // Frames
            0x00, 0x00,         // Dots
            0xf7 },
};

ctl_command s_custom = {
    3, {0, }
};

int process_ctl_event(uint8_t* data, size_t len, IOBackend* backend) {
    if (len == 3) {

        if (data[0] == 0xb0) {
            switch (data[1]) {
                case 0x17: // PLAY
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_PLAY);
                    }
                    break;
                case 0x16: // STOP
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_STOP);
                    }
                    break;

                case 0x18: // Toggle teach
                    if (data[2] == 0x7f)
                        backend->Notify(CTL_TEACH_ON);
                    else 
                        backend->Notify(CTL_TEACH_OFF);
                    break;
                case 0x07:
                    if (data[2]) {
                        backend->Notify(CTL_TEACH_MODE);
                    }
                    break;
                case 0x46:
                    if (backend->m_fader_touched) {
                        backend->m_fader_val = data[2];
                        backend->Notify(CTL_FADER);
                    }
                    break;
                case 0x0c:
                    if (data[2]) {
                        backend->Notify(CTL_SHUTDOWN);
                    }
                    break;
                case 0x0d:  //Marker
                    backend->m_marker = data[2] != 0;
                    backend->Notify(CTL_MARKER);
                    break;
                case 0x1e:
                    if (data[2])
                        backend->Notify(CTL_PREV_TRACK);
                    break;
                case 0x22:
                    if (data[2])
                        backend->Notify(CTL_NEXT_TRACK);
                    break;
                case 0x20:
                    if (data[2]) {
                        backend->m_wheel_mode = !backend->m_wheel_mode;
                        backend->Notify(CTL_WHEEL_MODE);
                    }
                    break;
                case 0x14:
                    if (data[2]) {
                        if (!backend->m_marker)
                            backend->Notify(CTL_HOME);
                        else
                            backend->Notify(CTL_LOOP_START);
                    }
                    break;
                case 0x15:
                    if (data[2]) {
                        if (!backend->m_marker)
                            backend->Notify(CTL_END);
                        else
                            backend->Notify(CTL_LOOP_END);
                    }
                    break;
                case 0x0f:
                    if (data[2]) {
                        backend->m_cycle = !backend->m_cycle;
                        backend->Notify(CTL_LOOP);
                    }
                    break;
                case 0x03:
                    if (data[2]) {
                        backend->Notify(CTL_UNSELECT);
                    }
                    break; 
                case 0x08:
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_TOGGLE_SOLO);
                    }
                    break;  
                case 0x06:
                    if (data[2] == 0x7f) {
                        backend->Notify(CTL_TOGGLE_REC);
                    }
                    break;  
                case 0x58:
                    if (data[2] == 0x01) {
                        if (!backend->m_wheel_mode)
                            backend->Notify(CTL_JUMP_BACKWARD);
                        else
                            backend->Notify(CTL_PREV_TRACK);
                    }
                    if (data[2] == 0x41) {
                        if (!backend->m_wheel_mode)
                            backend->Notify(CTL_JUMP_FORWARD);
                        else
                            backend->Notify(CTL_NEXT_TRACK);
                    }                    
                    break;
                default:
                    printf("uncaught 0xb0 %02x\n", data[1]);
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
                    backend->Notify(CTL_TEACH_OFF);
                    break;
//                case 0x64:
//                    backend->m_wheel_mode = false;
//                    backend->Notify(CTL_WHEEL_MODE);
//                    break;  
                default:
                    printf("uncaught 0x80 %02x\n", data[1]);
                    break;                    
            }
        }
        if (data[0] == 0xe0) {
//            if (backend->m_fader_touched) {
//                uint16_t v = data[2] << 7;
//                v += (data[1] << 1);
//                backend->m_fader_val = v >> 7;
//                backend->Notify(CTL_FADER);
//            }
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
//        if (backend->GetMidiMtc()->m_edge_sec == 1) {
//            backend->Notify(MTC_QUARTER_FRAME_SEC);
//        }
//        if (backend->GetMidiMtc()->m_edge_sec == 2) {
//            backend->Notify(MTC_QUARTER_FRAME_SEC1);
//            backend->GetMidiMtc()->m_edge_sec = 0;
//        } else
            backend->Notify(MTC_QUARTER_FRAME);
    } else if (data[0] == 0xf0) {
        backend->GetMidiMtc()->FullFrame(data);
        backend->Notify(MTC_COMPLETE);
    }
    return 1;
}
