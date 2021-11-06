/*
 * jack.c
 *
 *  Created on: Oct 20, 2021
 *      Author: onkel
 */



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include <src/OJack.h>

jack_port_t *mmc_in_port;
jack_port_t *mmc_out_port;
jack_port_t *mtc_port;
jack_port_t *ctl_in_port;
jack_port_t *ctl_out_port;

static uint8_t mmc_command[] = {0xf0, 0x7f, 0x00, 0x06};



static jack_midi_data_t midi_play[] = {0xf0, 0x7f, 0x7e, 0x06, 0x03, 0xf7};
static bool doPlay = false;
static jack_midi_data_t midi_stop[] = {0xf0, 0x7f, 0x7e, 0x06, 0x01, 0xf7};
static bool doStop = false;
static jack_midi_data_t find[] = {0xf0, 0x7f, 0x7e, 0x06, 0x05, 0xf7};
static bool doFind = false;
static jack_midi_data_t locate[] = {0xf0, 0x7f, 0x7e, 0x06, 0x44, 0x06, 0x01, 0, 0, 0, 0, 0, 0xf7};
static bool doLocate = false;

static bool ctl_play = false;

static bool doShowPlay = false;
static jack_midi_data_t ctl_show_play[] = { 0xb8, 1, 4 };
static bool doShowStop = false;
static jack_midi_data_t ctl_show_stop[] = { 0xb8, 1, 0 };

static bool doShowTeachOn = false;
static jack_midi_data_t ctl_show_teach_on[] = { 0xb9, 2, 4 };
static bool doShowTeachOff = false;
static jack_midi_data_t ctl_show_teach_off[] = { 0xb9, 2, 0 };

static int process_ctl_event(jack_midi_event_t event, OJack* jack) {
    if (event.size == 3) {

        if (event.buffer[0] == 0xb0) {
            switch (event.buffer[1]) {
                case 1: // PLAY/STOP
                    if (event.buffer[2]) {
                        jack->Notify(CTL_PLAYSTOP);
                    }
                    break;
                case 2: // Toggle teach
                    if (event.buffer[2])
                        jack->Notify(CTL_TEACH_ON);
                    else
                        jack->Notify(CTL_TEACH_OFF);
                    break;
            }
            return 1;
        }
    }
    return 0;
}

static int process_mmc_event(jack_midi_event_t event, OJack* jack) {

    if (event.size > 4) {
        if (!memcmp(event.buffer, mmc_command, sizeof (mmc_command))) {
            //printf("MMC: got command with with time %d  ", event.time);
            switch (event.buffer[4]) {
                case 1:
                    printf("  Stop\n");
                    jack->Notify(MMC_STOP);
                    break;
                case 2:
                    printf("  Play\n");
                    jack->Notify(MMC_PLAY);
                    break;
                case 3:
                    printf("  Deferred Play\n");
                    jack->Notify(MMC_PLAY);
                    
                    break;
                case 0x44:
                    printf("  Locate %02d:%02d:%02d:%02d\n", event.buffer[7], event.buffer[8], event.buffer[9], event.buffer[10]);
                    break;
            }
            return 1;
        }
    }
    return 0;
}

static int process_mtc_event(jack_midi_event_t event, OJack* jack) {
    uint8_t s;

    if (event.buffer[0] == 0xf1) {
        jack->m_jackMtc.QuarterFrame(event.buffer[1]);
        if (jack->m_jackMtc.m_edge_sec == 1) {
            jack->Notify(MTC_QUARTER_FRAME_SEC);
        }
        if (jack->m_jackMtc.m_edge_sec == 2) {
            jack->Notify(MTC_QUARTER_FRAME_SEC1);
            jack->m_jackMtc.m_edge_sec = 0;
        } else
            jack->Notify(MTC_QUARTER_FRAME);
    } else if (event.buffer[0] == 0xf0) {
        jack->m_jackMtc.FullFrame(event.buffer);
        jack->Notify(MTC_COMPLETE);
    }
    return 1;
}

static int process(jack_nframes_t nframes, void *arg) {
    int i;
    void *port_buf = jack_port_get_buffer(mmc_in_port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {
        //printf("Ardour MMC in: have %d events\n", event_count);
        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_mmc_event(in_event, ((OJack*) arg))) {
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
        //printf("Ardour MTC in: have %d events\n", event_count);
        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_mtc_event(in_event, ((OJack*) arg))) {
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
    if (doPlay) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (midi_play));
        memcpy(buffer, midi_play, sizeof (midi_play));
        printf("\nplay send\n");
        doPlay = false;
    }
    if (doStop) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (midi_stop));
        memcpy(buffer, midi_stop, sizeof (midi_stop));
        printf("\nstop send\n");
        doStop = false;
    }
    if (doFind) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (find));
        memcpy(buffer, find, sizeof (find));
        printf("\nfind send\n");
        doFind = false;
    }
    if (doLocate) {
        unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof (locate));
        memcpy(buffer, locate, sizeof (locate));
        printf("\nlocate send\n");
        doLocate = false;
    }

    if (doShowPlay) {
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, sizeof (ctl_show_play));
        
        memcpy(buffer, ctl_show_play, sizeof (ctl_show_play));
        printf("\nshow play\n");
        doShowPlay = false;
    }

    if (doShowStop) {
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, sizeof (ctl_show_stop));
        memcpy(buffer, ctl_show_stop, sizeof (ctl_show_stop));
        printf("\nshow stop\n");
        doShowStop = false;
    }
    
    if (doShowTeachOn) {
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, sizeof (ctl_show_teach_on));
        memcpy(buffer, ctl_show_teach_on, sizeof (ctl_show_teach_on));
        printf("\nshow teach on\n");
        doShowTeachOn = false;
    }
    if (doShowTeachOff) {
        unsigned char *buffer = jack_midi_event_reserve(ctl_buf, 0, sizeof (ctl_show_teach_off));
        memcpy(buffer, ctl_show_teach_off, sizeof (ctl_show_teach_off));
        printf("\nshow teach off\n");
        doShowTeachOff = false;
    }

    
    port_buf = jack_port_get_buffer(ctl_in_port, nframes);
    event_count = jack_midi_get_event_count(port_buf);
    if (event_count > 0) {

        for (i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);
            if (!process_ctl_event(in_event, ((OJack*) arg))) {
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

static void jack_shutdown(void *arg) {
    // TODO: fix me, what to do if jack server stops
    exit(1);
}

// JackMtc

void OJackMtc::FullFrame(uint8_t *frame_data) {
    hour = frame_data[5] & 0x1f;
    min = frame_data[6];
    sec = frame_data[7];
    frame = frame_data[8];
    subframe = 0;
}

void OJackMtc::QuarterFrame(uint8_t data) {
    lock_millis = true;
    subframe++;
    if (subframe == 4) {
        subframe = 0;
        frame++;
    }
    if (frame == 30) {
        frame = 0;
        sec++;
        m_edge_sec = true;
    }
    if (sec == 60) {
        sec = 0;
        min++;
    }
    if (min == 60) {
        min = 0;
        hour++;
    }

    lock_millis = false;
}

gint OJackMtc::GetMillis() {
    while (lock_millis);
    return hour * 432000 + min * 7200 + sec * 120 + (frame * 4) + (subframe);
}

std::string OJackMtc::GetTimeCode() {
    char t[32];

    sprintf(t, "%02d:%02d:%02d:%02d", hour, min, sec, frame);
    timecode = t;
    return timecode;
}


// class OJack

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

    if (jack_activate(m_jack_client)) {
        fprintf(stderr, "cannot activate client");
        return;
    }

    jack_connect(m_jack_client, "ardour:MTC out", "autoX32:Ardour MTC in");
    jack_connect(m_jack_client, "ardour:MMC out", "autoX32:Ardour MMC in");
    jack_connect(m_jack_client, "autoX32:Ardour MMC out", "ardour:MMC in");
    jack_connect(m_jack_client, "netjack:capture_1", "autoX32:Onkel Controller in");
    jack_connect(m_jack_client, "autoX32:Onkel Controller out", "netjack:playback_1");

//    doPlay = true;
//    usleep(100000);
//    doStop = true;
}

void OJack::Play() {
    doPlay = true;
}

void OJack::Stop() {
    doStop = true;
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

gint OJack::GetMillis() {
    return m_jackMtc.GetMillis();
}

void OJack::Notify(JACK_EVENT event) {
    m_parent->notify_jack(event);
}

std::string OJack::GetTimeCode() {
    return m_jackMtc.GetTimeCode();
}

void OJack::ControllerShowPlay() {
    doShowPlay = true;
}

void OJack::ControllerShowStop() {
    doShowStop = true;
}


void OJack::ControllerShowTeachOn() {
    doShowTeachOn = true;
}

void OJack::ControllerShowTeachOff() {
    doShowTeachOff = true;
}