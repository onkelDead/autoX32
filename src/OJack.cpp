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

#include <src/OJack.h>
#include <queue>

jack_port_t *mmc_in_port;
jack_port_t *mmc_out_port;
jack_port_t *mtc_port;
jack_port_t *ctl_in_port;
jack_port_t *ctl_out_port;

static uint8_t mmc_command[13];

static jack_midi_data_t midi_playstop[] = {0xf0, 0x7f, 0x7e, 0x06, 0x03, 0xf7};

static bool doLocate = false;
static jack_midi_data_t locate[] = {0xf0, 0x7f, 0x7e, 0x06, 0x44, 0x06, 0x01, 0, 0, 0, 0, 0, 0xf7};

static int process(jack_nframes_t nframes, void *arg) {
    
    OJack* jack = (OJack*) arg;
    
    jack->ReconnectPorts();
    
    int i;
    void *port_buf = jack_port_get_buffer(mmc_in_port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {
        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_mmc_event(in_event.buffer, in_event.size, ((OJack*) arg))) {
                printf("    event %d time is %d size is %ld\n    ", i, in_event.time, in_event.size);
                for (int j = 0; j < in_event.size; j++) {
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
                for (int j = 0; j < in_event.size; j++) {
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
        uint8_t c = jack->mmc_out.front();
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (midi_playstop));
        memcpy(buffer, midi_playstop, sizeof (midi_playstop));
        buffer[4] = c;
        jack->mmc_out.pop();
    }

    if (doLocate) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (locate));
        memcpy(buffer, locate, sizeof (locate));
        doLocate = false;
    }

    if (!jack->ctl_out.empty()) {
        int c = jack->ctl_out.front();
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, 3);
        buffer[0] = (c >> 16) & 0xff;
        buffer[1] = (c >> 8) & 0xff;
        buffer[2] = (c) & 0xff;
        jack->ctl_out.pop();
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
                for (int j = 0; j < in_event.size; j++) {
                    printf("%02x ", in_event.buffer[j]);
                }
                printf("\n");
#endif                
            }
        }
    }
    return 0;
}

void OJack::Start() {
    
}

static void jack_shutdown(void *arg) {
    // TODO: fix me, what to do if jack server stops
    exit(1);
}

// class OJack
void on_port_connect(jack_port_id_t a, jack_port_id_t b, int connect, void* arg) {
    
    OJack* jack = (OJack*) arg;
    
    if (connect)    
        printf("Connect ");
    else
        printf("Disconnect ");
    printf("Connect '%s' to '%s'\n", jack_port_name(jack_port_by_id(jack->m_jack_client, a)), jack_port_name(jack_port_by_id(jack->m_jack_client, b)));
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

void OJack::Connect(IOMainWnd* wnd) {

    m_parent = wnd;

    if ((m_jack_client = jack_client_open("autoX32", JackNullOption, NULL)) == 0) {
        fprintf(stderr, "jack server not running?\n");
        return;
    }

    jack_set_process_callback(m_jack_client, process, this);

    jack_on_shutdown(m_jack_client, jack_shutdown, 0);

    mmc_in_port = jack_port_register(m_jack_client, "Ardour MMC in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    mmc_out_port = jack_port_register(m_jack_client, "Ardour MMC out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    mtc_port = jack_port_register(m_jack_client, "Ardour MTC in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    ctl_in_port = jack_port_register(m_jack_client, "Onkel Controller in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    ctl_out_port = jack_port_register(m_jack_client, "Onkel Controller out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

    
    jack_set_client_registration_callback(m_jack_client, on_register_client, this);
    jack_set_port_registration_callback(m_jack_client, on_register_port, this);
    jack_set_port_connect_callback(m_jack_client, on_port_connect, this);
    
    if (jack_activate(m_jack_client)) {
        fprintf(stderr, "cannot activate client");
        return;
    }

//    jack_connect(m_jack_client, "ardour:MTC out", "autoX32:Ardour MTC in");
    jack_connect(m_jack_client, "ardour:MMC out", "autoX32:Ardour MMC in");
    jack_connect(m_jack_client, "autoX32:Ardour MMC out", "ardour:MMC in");
    jack_connect(m_jack_client, "OSerialBridge:out", "autoX32:Onkel Controller in");
    jack_connect(m_jack_client, "autoX32:Onkel Controller out", "OSerialBridge:in");
    
    ctl_out.push(CTL_COMMAND(0xb8, 0, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xb9, 1, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xba, 2, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbb, 0, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbc, 1, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbd, 2, 64));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xb8, 0, 0));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xb9, 1, 0));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xba, 2, 0));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbb, 0, 0));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbc, 1, 0));
    usleep(100000);
    ctl_out.push(CTL_COMMAND(0xbd, 2, 0));

//    Play();
//    usleep(100000);
//    Stop();
}

void OJack::Play() {
    mmc_out.push(0x03);
    ControllerShowPlay();
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
        jack_connect(m_jack_client, "autoX32:Ardour MMC out", "ardour:MMC in");
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

void OJack::Stop() {
    mmc_out.push(0x01);
    ControllerShowStop();
}

void OJack::Locate(gint millis) {
    int mm = (millis / 4) % 30;
    int sec = (millis / 120) % 60;
    int min = (millis / 7200) % 60;
    int hour = (millis / 432000);
    locate[7] = hour;
    locate[8] = min;
    locate[9] = sec;
    locate[10] = mm;
    doLocate = true;
}

int OJack::GetMillis() {
    return m_midi_mtc.GetMillis();
}

void OJack::SetFrame(gint frame) {
    m_midi_mtc.SetFrame(frame);
}

void OJack::Notify(JACK_EVENT event) {
    m_parent->notify_jack(event);
}

std::string OJack::GetTimeCode() {
    return m_midi_mtc.GetTimeCode();
}

void OJack::ControllerShowPlay() {
    ctl_out.push(CTL_COMMAND(0xb8, 1, 4));
}

void OJack::ControllerShowStop() {
    ctl_out.push(CTL_COMMAND(0xb8, 1, 0));
}

void OJack::ControllerShowTeachOn() {
    ctl_out.push(CTL_COMMAND(0xb9, 2, 4));
}

void OJack::ControllerShowTeachOff() {
    ctl_out.push(CTL_COMMAND(0xb9, 2, 0));
}

void OJack::LoopStart() {
    m_loop_state = true;
    ctl_out.push(CTL_COMMAND(0xba, 1, 4));
    ctl_out.push(CTL_COMMAND(0xba, 0, 0));
}

void OJack::LoopEnd() {
    m_loop_state = false;
    ctl_out.push(CTL_COMMAND(0xba, 1, 0));
    ctl_out.push(CTL_COMMAND(0xba, 0, 4));
}

bool OJack::GetLoopState() {
    return m_loop_state;
}

void OJack::SetLoopState(bool state) {
    ctl_out.push(CTL_COMMAND(0xba, 1, 0));
    ctl_out.push(CTL_COMMAND(0xba, 0, 0));
    m_loop_state = state;
}
