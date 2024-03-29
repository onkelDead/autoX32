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

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "OJack.h"
#include <queue>
#include <iostream>

jack_port_t *mmc_in_port;
jack_port_t *mmc_out_port;
jack_port_t *mtc_port;
jack_port_t *ctl_in_port;
jack_port_t *ctl_out_port;

static jack_midi_data_t midi_playstop[] = {0xf0, 0x7f, 0x00, 0x06, 0x03, 0xf7};

static bool doLocate = false;
static jack_midi_data_t locate[] = {0xf0, 0x7f, 0x7e, 0x06, 0x44, 0x06, 0x01, 0, 0, 0, 0, 0, 0xf7};
static bool doShuffle = false;
static jack_midi_data_t shuffle[] = {0xF0, 0x7F, 0x7e, 0x06, 0x47, 0x03, 0b00000000, 0x00, 0x00, 0xF7};

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

static ctl_command s_drop = {
    3,
    { 0xB0, CTL_BUTTON_DROP, 0x00}
};

static ctl_command s_stop = {
    3,
    { 0xB0, CTL_BUTTON_STOP, 0x41}
};

static ctl_command s_play = {
    3,
    { 0xB0, CTL_BUTTON_PLAY, 0x41}
};

static ctl_command s_record = {
    3,
    { 0xB0, CTL_BUTTON_REC, 0x41}
};

static ctl_command s_teach = {
    3,
    { 0xB0, CTL_BUTTON_TEACH, 0x0}
};

static ctl_command s_f1 = {
    3,
    { 0xB0, CTL_BUTTON_F1, 0x00}
};

static ctl_command s_f2 = {
    3,
    { 0xB0, CTL_BUTTON_F2, 0x00}
};

static ctl_command s_f6 = {
    3,
    { 0xB0, CTL_BUTTON_F6, 0x0}
};

static ctl_command s_scrub = {
    3,
    { 0x90, 0x65, 0x7f}
};

static ctl_command s_wheel_mode = {
    3,
    { 0xB0, CTL_BUTTON_SCRUB, 0x00}
};

static ctl_command s_select = {
    3,
    { 0xB0, CTL_BUTTON_SELECT, 0x00}
};

static ctl_command s_level = {
    3, 
    { 0xB0, 0, 0}
};

static ctl_command s_lcd_1 = {
    23,
    {0, }
};

static ctl_command s_lcd_2 = {
    15,
    {0, }
};

static ctl_command s_mtc_full[8] = {
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
    { 3, {0, } },
};

static ctl_command s_mtc_quarter = {
    3, {0, }
};

static ctl_command s_marker = {
    3, {0xb0, CTL_BUTTON_MARKER, 0x00 }
};
static ctl_command s_cycle = {
    3, {0xb0, CTL_BUTTON_CYCLE, 0x00 }
};


uint8_t Nibble2Seven[] = { SevenSeg_0, SevenSeg_1, SevenSeg_2, SevenSeg_3, SevenSeg_4, SevenSeg_5, SevenSeg_6, SevenSeg_7, SevenSeg_8, SevenSeg_9};

static ctl_command s_7seg = {
    21, {0xf0, 0x00, 0x20, 0x32, 0x41, 0x37, 
            0x00, 0x00,         // Assignment
            0x00, 0x00, 0x00,   // Hours
            0x00, 0x00,         // Minutes
            0x00, 0x00,         // Seconds
            0x00, 0x00, 0x00,   // Frames
            0x00, 0x00,         // Dots
            0xf7 },
};

static ctl_command s_custom = {
    3, {0, }
};


static int process(jack_nframes_t nframes, void *arg) {

    OJack* jack = (OJack*) arg;

    unsigned int i;
    void *port_buf = jack_port_get_buffer(mmc_in_port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {
        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_mmc_event(in_event.buffer, in_event.size, ((OJack*) arg))) {
                printf("    event %d time is %d size is %ld\n    ", i, in_event.time, in_event.size);
                for (size_t j = 0; j < in_event.size; j++) {
                    printf("%02x ", in_event.buffer[j]);
                }
                printf("\n");
            }
        }
    }

    port_buf = jack_port_get_buffer(mtc_port, nframes);
    event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {
        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_mtc_event(in_event.buffer, ((OJack*) arg))) {
                printf("    event %d time is %d size is %ld\n    ", i, in_event.time, in_event.size);
                for (size_t j = 0; j < in_event.size; j++) {
                    printf("%02x ", in_event.buffer[j]);
                }
                printf("\n");
            }
        }
    }

    port_buf = jack_port_get_buffer(mmc_out_port, nframes);
    jack_midi_clear_buffer(port_buf);
    void* ctl_buf = jack_port_get_buffer(ctl_out_port, nframes);
    jack_midi_clear_buffer(ctl_buf);

    if (!jack->mmc_out.empty()) {
        uint8_t c;
        jack->mmc_out.front_pop(&c);
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (midi_playstop));
        memcpy(buffer, midi_playstop, sizeof (midi_playstop));
        buffer[4] = c;
    }

    if (doLocate) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (locate));
        memcpy(buffer, locate, sizeof (locate));
        doLocate = false;
    }

    if (doShuffle) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (shuffle));
        memcpy(buffer, shuffle, sizeof (shuffle));
        doShuffle = false;
    }

    while (!jack->ctl_out.empty()) {
        ctl_command* c;
        jack->ctl_out.front_pop(&c);
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, c->len);
        for (int i = 0; i < c->len; i++) {
            buffer[i] = c->buf[i];
        }
    }

    port_buf = jack_port_get_buffer(ctl_in_port, nframes);
    event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {

        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_ctl_event(in_event.buffer, in_event.size, ((OJack*) arg))) {
#if 0               
                printf("Onkel Controller in: have %d events\n", event_count);
                printf("    event %d time is %d size is %ld\n    ", i, in_event.time, in_event.size);
                for (size_t j = 0; j < in_event.size; j++) {
                    printf("%02x ", in_event.buffer[j]);
                }
                printf("\n");
#endif                
            }
        }
    }
    return 0;
}

static void jack_shutdown(void *arg) {
    // TODO: fix me, what to do if jack server stops
    exit(1);
}

// class OJack

void on_port_connect(jack_port_id_t a, jack_port_id_t b, int connect, void* arg) {

    OJack* jack = (OJack*) arg;

    const char* port_name_a = jack_port_name(jack_port_by_id(jack->m_jack_client, a));
    const char* port_name_b = jack_port_name(jack_port_by_id(jack->m_jack_client, b));

    if (connect) {
        if (strcmp(port_name_b, ONKEL_C_IN_PORT_NAME) == 0) {
            jack->m_config->set_string("controller_in_port", port_name_a);
        }
        if (strcmp(port_name_a, ONKEL_C_OUT_PORT_NAME) == 0) {
            jack->m_config->set_string("controller_out_port", port_name_b);
        }
        if (strcmp(port_name_a, ONKEL_D_MMC_OUT_PORT_NAME) == 0) {
            jack->m_config->set_string("mmc_out_port", port_name_b);
        }
        if (strcmp(port_name_b, ONKEL_D_MMC_IN_PORT_NAME) == 0) {
            jack->m_config->set_string("mmc_in_port", port_name_a);
        }
        if (strcmp(port_name_b, ONKEL_D_MTC_IN_PORT_NAME) == 0) {
            jack->m_config->set_string("mtc_in_port", port_name_a);
        }
        
    }
    if (connect)
        std::cout << "Connect ";
    else
        std::cout << "Disconnect ";
    std::cout << "'" << port_name_a << "' to '" << port_name_b << '"' << std::endl;
}

void on_register_client(const char* name, int reg, void *arg) {
    if (reg)
        printf("Register ");
    else
        printf("Unregister ");
    printf("Client '%s'\n", name);

    return;
}

void on_register_port(jack_port_id_t port, int reg, void *arg) {
    OJack* jack = (OJack*) arg;

    const char* port_name = jack_port_name(jack_port_by_id(jack->m_jack_client, port));

    if (reg)
        printf("Register ");
    else
        printf("Unregister ");
    printf(" Port '%s'\n", port_name);

    if (reg) {
        if (strcmp("ardour:MTC out", port_name) == 0)
            jack->m_reconnect_mtc_out = true;
        if (strcmp("ardour:MMC out", port_name) == 0)
            jack->m_reconnect_mmc_out = true;
        if (strcmp("ardour:MMC in", port_name) == 0)
            jack->m_reconnect_mmc_in = true;
        if (strcmp("OSerialBridge:out", port_name) == 0)
            jack->m_reconnect_ctl_out = true;
        if (strcmp("OSerialBridge:in", port_name) == 0)
            jack->m_reconnect_ctl_in = true;

    }
}

int OJack::Connect(IOJackHandler* wnd) {
    int errcode;
    
    m_parent = wnd;

    if ((m_jack_client = jack_client_open("autoX32", JackNullOption, NULL)) == 0) {
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }
    std::cout << "Jack: client created." << std::endl;

    jack_set_client_registration_callback(m_jack_client, on_register_client, this);
    jack_set_port_registration_callback(m_jack_client, on_register_port, this);
    jack_set_port_connect_callback(m_jack_client, on_port_connect, this);

    jack_set_process_callback(m_jack_client, process, this);

    jack_on_shutdown(m_jack_client, jack_shutdown, 0);

    mmc_in_port = jack_port_register(m_jack_client, ONKEL_D_MMC_IN_PORT, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    mmc_out_port = jack_port_register(m_jack_client, ONKEL_D_MMC_OUT_PORT, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    mtc_port = jack_port_register(m_jack_client, ONKEL_D_MTC_IN_PORT, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    ctl_in_port = jack_port_register(m_jack_client, ONKEL_C_IN_PORT, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    ctl_out_port = jack_port_register(m_jack_client, ONKEL_C_OUT_PORT, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

    if (jack_activate(m_jack_client)) {
        std::cerr << "cannot activate client: errorcode " << errcode << std::endl;
        return 1;
    }

    std::string ctrl_in_port = m_parent->GetConfig()->get_string("controller_in_port");
    std::cout << "Jack: connect '" << ONKEL_C_IN_PORT_NAME << "' to '" << ctrl_in_port << std::endl;
    if ((errcode = jack_connect(m_jack_client, ctrl_in_port.c_str(), ONKEL_C_IN_PORT_NAME)) != 0) {
        std::cerr << "ERROR: OJack::Connect() on " << ONKEL_C_IN_PORT_NAME << " failed with error code " << errcode << std::endl;
    }
    
    std::string ctrl_out_port = m_parent->GetConfig()->get_string("controller_out_port");
    std::cout << "Jack: connect '" << ONKEL_C_OUT_PORT_NAME << "' to '" << ctrl_out_port << std::endl;
    if ((errcode = jack_connect(m_jack_client, ONKEL_C_OUT_PORT_NAME, ctrl_out_port.c_str())) != 0) {
        std::cerr << "ERROR: OJack::Connect() on " << ONKEL_C_OUT_PORT_NAME << " failed with error code " << errcode << std::endl;
    }

    std::string mmc_in_port = m_parent->GetConfig()->get_string("mmc_in_port");
    std::cout << "Jack: connect '" << ONKEL_D_MMC_IN_PORT_NAME << "' to '" << mmc_in_port << std::endl;
    if ((errcode = jack_connect(m_jack_client, mmc_in_port.c_str(), ONKEL_D_MMC_IN_PORT_NAME)) != 0) {
        std::cerr << "ERROR: OJack::Connect() on " << ONKEL_D_MMC_IN_PORT_NAME << " failed with error code " << errcode << std::endl;
    }

    std::string mtc_in_port = m_parent->GetConfig()->get_string("mtc_in_port");
    std::cout << "Jack: connect '" << ONKEL_D_MTC_IN_PORT_NAME << "' to '" << mtc_in_port << std::endl;
    if ((errcode = jack_connect(m_jack_client, mtc_in_port.c_str(), ONKEL_D_MTC_IN_PORT_NAME)) != 0) {
        std::cerr << "ERROR: OJack::Connect() on " << ONKEL_D_MTC_IN_PORT_NAME << " failed with error code " << errcode << std::endl;
    }
    
    std::string mmc_out_port = m_parent->GetConfig()->get_string("mmc_out_port");
    std::cout << "Jack: connect '" << ONKEL_D_MMC_OUT_PORT_NAME << "' to '" << mmc_out_port << std::endl;
    if ((errcode = jack_connect(m_jack_client, ONKEL_D_MMC_OUT_PORT_NAME, mmc_out_port.c_str())) != 0) {
        std::cerr << "ERROR: OJack::Connect() on " << ONKEL_D_MMC_OUT_PORT_NAME << " failed with error code " << errcode << std::endl;
    }
    
    return 0;
}

void OJack::ControllerReset() {
    ControllerClearMtc();
    ControllerShowPlayState(E_TRANSPORT_STATE::OFF);
    ControllerShowTeach(false);
    ControllerShowRec(false);
    ControllerShowTeachMode(false);
    ControllerShowStepMode(false);
    ControllerShowWheelMode(false);
    ControllerShowCycle(false);
    ControllerShowMarker(false);
    ControllerShowSelect(0);
    ControllerShowScrub();
    ControllerShowLCDName("", 0);
    ControllerShowDrop(false);
    ControllerShowLevel(0.0);
    ControllerShowActive(false);
    
    while (!ctl_out.empty()) {
        usleep(20);
    }
}

void OJack::Disconnect() {
    jack_deactivate(m_jack_client);
    jack_client_close(m_jack_client);
}

void OJack::Play(bool mmc) {
    if (mmc)
        mmc_out.push(0x03);
    ControllerShowPlayState(E_TRANSPORT_STATE::PLAY);
}

void OJack::ReconnectPorts() {
    if (m_reconnect_mtc_out) {
        m_reconnect_mtc_out = false;
        jack_connect(m_jack_client, "ardour:MTC out", "autoX32:Ardour MTC in");
    }

    if (m_reconnect_mmc_out) {
        m_reconnect_mmc_out = false;
        jack_connect(m_jack_client, "ardour:MMC out", "autoX32:Ardour MMC in");
    }

    if (m_reconnect_mmc_in) {
        m_reconnect_mmc_in = false;
        if (!jack_connect(m_jack_client, "autoX32:Ardour MMC out", "ardour:MMC in"))
            Notify(MMC_RESET);
    }

    if (m_reconnect_ctl_in) {
        jack_connect(m_jack_client, "OSerialBridge:out", "autoX32:Onkel Controller in");
        m_reconnect_ctl_in = false;
    }
    if (m_reconnect_ctl_out) {
        jack_connect(m_jack_client, "autoX32:Onkel Controller out", "OSerialBridge:in");
        m_reconnect_ctl_out = false;
    }
}

void OJack::Stop(bool mmc) {
    if (mmc)
        mmc_out.push(0x01);
    ControllerShowPlayState(E_TRANSPORT_STATE::STOP);
}

void OJack::Locate(int frame) {
    int mm = (frame / 4) % 30;
    int sec = (frame / 120) % 60;
    int min = (frame / 7200) % 60;
    int hour = (frame / 432000);
    locate[7] = hour;
    locate[8] = min;
    locate[9] = sec;
    locate[10] = mm;
    doLocate = true;
}

void OJack::Shuffle(bool s) {
    m_shuffle_speed = s
            ? std::max(-7, m_shuffle_speed - 1)
            : std::min(7, m_shuffle_speed + 1);

    shuffle[6] = 0x00;
    if (m_shuffle_speed != 0) {

        if (m_shuffle_speed < 0)
            shuffle[6] |= 0x40;

        shuffle[6] |= (abs(m_shuffle_speed));
        doShuffle = true;
        Play(false);
    } else {
        Stop(false);
    }
    printf("Shuffle speed %d %d %02x\n", s, m_shuffle_speed, shuffle[6]);

}

int OJack::GetFrame() {
    return m_midi_mtc.GetFrame();
}

void OJack::SetFrame(int frame) {
    m_midi_mtc.SetFrame(frame);
}
//                m_backend->ControllerShowMarker();

void OJack::QuarterFrame(uint8_t q) {
    int s = m_midi_mtc.QuarterFrame(q);
    ControlerShowMtcComplete(s);
}

void OJack::Notify(JACK_EVENT event) {
    m_parent->notify_jack(event);
}

std::string OJack::GetTimeCode() {
    return m_midi_mtc.GetTimeCode();
}

uint8_t* OJack::GetTimeDiggits() {
    return m_midi_mtc.diggit;
}

void OJack::ControllerShowPlayState(E_TRANSPORT_STATE val) {
    switch(val) {
        case E_TRANSPORT_STATE::PLAY:
            s_stop.buf[2] = 0x00;
            s_play.buf[2] = 0x41;
            break;
        case E_TRANSPORT_STATE::STOP:
            s_stop.buf[2] = 0x41;
            s_play.buf[2] = 0x00;
            break;
        default:
            s_stop.buf[2] = 0x00;
            s_play.buf[2] = 0x00;
            break;
    }
    ctl_out.push(&s_stop);
    ctl_out.push(&s_play);
}

void OJack::ControllerShowDrop(bool val) {
    s_drop.buf[2] = val ? 0x40 : 0x00;
    ctl_out.push(&s_drop);
}

void OJack::ControllerShowTeach(bool val) {
    s_teach.buf[2] = val ? 0x41 : 0x00;
    ctl_out.push(&s_teach);
}

void OJack::ControllerShowMarker(bool val) {
    s_marker.buf[2] = val ? 0x41 : 0x00;
    ctl_out.push(&s_marker);
}

void OJack::ControllerShowCycle(bool val) {
    s_cycle.buf[2] = val ? 0x7f : 0x00;
    ctl_out.push(&s_cycle);
}

void OJack::ControllerShowTeachMode(bool val) {
    s_f1.buf[2] = val ? 0x41 : 0;
    ctl_out.push(&s_f1);
}

void OJack::ControllerShowSelect(bool val) {
    s_select.buf[2] = val ? 0x41 : 0;
    ctl_out.push(&s_select);
}


void OJack::ControllerShowActive(bool val) {
    s_f6.buf[2] = val ? 0x40 : 0;
    ctl_out.push(&s_f6);
}

void OJack::ControllerShowRec(bool val) {
    ctl_command *c = &s_record;
    c->buf[2] = val ? 0x7f : 0x00;
    ctl_out.push(c);
}

void OJack::ControllerShowLCDName(std::string name, int color) {
    const char* s = name.c_str();

    uint8_t syext[] = {0xF0, 0x00, 0x20, 0x32, 0x41, 0x4C, 0x00, 0x04, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xF7};

    ctl_command* c = &s_lcd_1;
    memcpy(syext + 8, s, std::min((size_t)14, strlen(s)));
    syext[7] = color;
    c->len = sizeof (syext);
    memcpy(c->buf, syext, 23);

    ctl_out.push(c);
}

void OJack::ControllerShowLevel(float f) {
    if (!m_fader_touched) {
        ctl_command* c = &s_level;
        c->buf[1] = 0x46;
        c->buf[2] = (127 * f);
        ctl_out.push(c);
    }
}

void OJack::ControllerClearMtc() {

    int i;
    ctl_command* sysex = &s_7seg;    
    for (i = 8; i >= 0; i--) {
        sysex->buf[16 - i] = 0;
    }
    ctl_out.push(sysex);    
}

void OJack::ControlerShowMtcComplete(uint8_t s) {

    int i;
    ctl_command* sysex = &s_7seg;
    
    uint8_t* tc = GetTimeDiggits();
    uint8_t highest = 0;
    for (int i = s; i < 8; i++) {
        uint8_t d = tc[i / 2];
        uint8_t e = (i & 1) ? d / 10 : d % 10;
        
        sysex->buf[16 - i] = Nibble2Seven[e];
        if (e) 
            highest = i;
    }
    for (i = 8; i > highest; i--) {
        sysex->buf[16 - i] = 0;
    }
    ctl_out.push(sysex);
}

void OJack::ControlerShowMtcQuarter(uint8_t q) {
    ctl_command* c0 = &s_mtc_quarter;
    c0->len = 2;
    c0->buf[0] = 0xd0;
    c0->buf[1] = q;
}

void OJack::ControllerShowScrub() {
    s_scrub.buf[2] = m_scrub ? 0x41 : 0;
    ctl_out.push(&s_scrub);
}

void OJack::ControllerShowWheelMode(bool val) {
    s_wheel_mode.buf[2] = val ? 0x41 : 0;
    ctl_out.push(&s_wheel_mode);
}

void OJack::ControllerShowStepMode(bool val) {
    s_f2.buf[2] = val ? 0x41 : 0x00;
    ctl_out.push(&s_f2);
}

void OJack::ControllerCustom(uint8_t com, uint8_t a, uint8_t b) {
    ctl_command* c = &s_custom;
    c->len = 3;
    c->buf[0] = com;
    c->buf[1] = a;
    c->buf[2] = b;
    ctl_out.push(c);
    
}

void OJack::LoopStart() {
    m_loop_state = true;
}

void OJack::LoopEnd() {
    m_loop_state = false;
}

bool OJack::GetLoopState() {
    return m_loop_state;
}

void OJack::SetLoopState(bool state) {
    m_loop_state = state;
}

