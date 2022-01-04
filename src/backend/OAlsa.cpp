/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OAlsa.cpp
 * Author: onkel
 * 
 * Created on December 12, 2021, 10:14 AM
 */

#include <queue>
#include "OAlsa.h"

static bool doLocate = false;
static uint8_t locate[] = {0xf0, 0x7f, 0x7e, 0x06, 0x44, 0x06, 0x01, 0, 0, 0, 0, 0, 0xf7};

static uint8_t midi_playstop[] = {0xf0, 0x7f, 0x7e, 0x06, 0x03, 0xf7};

snd_seq_client_info_t* info;
snd_seq_port_info_t* port_info;

static int in_port_mmc, in_port_mtc, out_port_mmc;

void process_event(snd_seq_event_t* ev, OAlsa* obj) {
#if 0
    printf("ev->type: %d\n", ev->type);
    printf("    source %d %x\n", ev->source.client, ev->source.port);
    printf("    dest   %d %x\n", ev->dest.client, ev->dest.port);
    printf("    size   %d\n", ev->data.ext.len);
    printf("          ");
#endif

    uint8_t qf[] = { 0xf1, 0x00 };
    
    if (ev->dest.port == 0 && ev->type == SND_SEQ_EVENT_SYSEX) {
        process_mmc_event((uint8_t*) ev->data.ext.ptr, ev->data.ext.len, obj);
        return;
    }
    
    if (ev->dest.port == 1 && ev->type == SND_SEQ_EVENT_SYSEX) {
        process_mtc_event((uint8_t*) ev->data.ext.ptr, obj);
        return;
    }
    
    if (ev->dest.port == 1 && ev->type == SND_SEQ_EVENT_QFRAME) {
        qf[1] = ev->data.raw8.d[8];
        process_mtc_event(qf, obj);
        return;
    }
    
//    printf("unknown type: %d\n", ev->type);

}

void process(void* user_data) {
    snd_seq_event_t* ev;

    OAlsa* data = (OAlsa*) user_data;

    if (poll(data->pfd, data->npfd, 0) > 0) {
        snd_seq_event_input(data->alsa_client, &ev);
        process_event(ev, data);
    }
}

OAlsa::OAlsa() {
}

OAlsa::~OAlsa() {
}

void OAlsa::Connect(IOMainWnd* wnd) {
    m_parent = wnd;

    snd_seq_client_info_malloc(&info);
    snd_seq_port_info_malloc(&port_info);

    snd_seq_open(&alsa_client, "hw", SND_SEQ_OPEN_DUPLEX, 0);
    snd_seq_set_client_name(alsa_client, "autoX32");

    m_client_id = snd_seq_client_id(alsa_client);

    in_port_mmc = snd_seq_create_simple_port(alsa_client, "autoX32 MMC:in",
            SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
            SND_SEQ_PORT_TYPE_APPLICATION);

    in_port_mtc = snd_seq_create_simple_port(alsa_client, "autoX32 MTC:in",
            SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
            SND_SEQ_PORT_TYPE_APPLICATION);

    out_port_mmc = snd_seq_create_simple_port(alsa_client, "autoX32 MMC:out",
            SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
            SND_SEQ_PORT_TYPE_APPLICATION);

    int loop = 0;
    npfd = snd_seq_poll_descriptors_count(alsa_client, POLLIN);
    pfd = (struct pollfd *) alloca(npfd * sizeof (struct pollfd));
    snd_seq_poll_descriptors(alsa_client, pfd, npfd, POLLIN);

    m_timer = new OTimer(process, 0, (void*) this);

}

void OAlsa::Start() {
    m_timer->start();
}

std::string OAlsa::GetTimeCode() {
    return m_midi_mtc.GetTimeCode();
}

int OAlsa::GetMillis() {
    return m_midi_mtc.GetMillis();
}

void OAlsa::Play() {
    midi_playstop[4] = 0x03;

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port_mmc);
    ev.source.client = m_client_id;
    ev.flags = SND_SEQ_EVENT_LENGTH_VARIABLE;
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_sysex(&ev, sizeof (midi_playstop), midi_playstop);

    int err = snd_seq_event_output_direct(alsa_client, &ev);
    if (err < 0)
        printf("Play: %s\n", snd_strerror(err));
    ControllerShowPlay();
}

void OAlsa::Stop() {
    midi_playstop[4] = 0x01;

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port_mmc);
    ev.source.client = m_client_id;
    ev.flags = SND_SEQ_EVENT_LENGTH_VARIABLE;
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_sysex(&ev, sizeof (midi_playstop), midi_playstop);

    int err = snd_seq_event_output_direct(alsa_client, &ev);
    if (err < 0)
        printf("Stop: %s\n", snd_strerror(err));
    ControllerShowStop();
}

void OAlsa::Locate(gint millis) {
    int mm = (millis / 4) % 30;
    int sec = (millis / 120) % 60;
    int min = (millis / 7200) % 60;
    int hour = (millis / 432000);
    locate[7] = hour;
    locate[8] = min;
    locate[9] = sec;
    locate[10] = mm;

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port_mmc);
    ev.source.client = m_client_id;
    ev.flags = SND_SEQ_EVENT_LENGTH_VARIABLE;
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_sysex(&ev, sizeof (locate), locate);

    int err = snd_seq_event_output_direct(alsa_client, &ev);
    if (err < 0)
        printf("Play: %s\n", snd_strerror(err));
}

void OAlsa::Shuffle(bool) {
    
}

void OAlsa::SetFrame(gint frame) {
    m_midi_mtc.SetFrame(frame);
}

void OAlsa::QuarterFrame(uint8_t q) {
    
}


void OAlsa::ControllerShowPlay() {
//    ctl_command* c = new ctl_command;
//    c->len = 3;
//    c->buf[0] = 0x8b;
//    c->buf[1] = 1;
//    c->buf[2] = 4;
//    ctl_out.push(c);
}

void OAlsa::ControllerShowStop() {
//    ctl_command* c = new ctl_command;
//    c->len = 3;
//    c->buf[0] = 0x8b;
//    c->buf[1] = 1;
//    c->buf[2] = 0;    
//    ctl_out.push(c);
}

void OAlsa::ControllerShowRecOn() {
//    ctl_command* c = new ctl_command;
//    c->len = 3;
//    c->buf[0] = 0xb9;
//    c->buf[1] = 2;
//    c->buf[2] = 4;    
//    ctl_out.push(c);
}

void OAlsa::ControllerShowRecOff() {
//    ctl_command* c = new ctl_command;
//    c->len = 3;
//    c->buf[0] = 0x8b;
//    c->buf[1] = 1;
//    c->buf[2] = 0;    
//    ctl_out.push(c);
}

void OAlsa::ControllerShowTeachMode(bool) {
    
}

void OAlsa::ControllerShowLCDName(std::string) {
    
}

void OAlsa::ControllerShowLevel(float f) {
    
}

void OAlsa::ControlerShowMtcComplete(uint8_t s) {
    
}

void OAlsa::ControlerShowMtcQuarter(uint8_t q) {
    
}

void OAlsa::ControllerShowScrub() {

}

void OAlsa::LoopStart() {
    m_loop_state = true;
//    ctl_out.push(CTL_COMMAND(0xba, 1, 4));
//    ctl_out.push(CTL_COMMAND(0xba, 0, 0));
}

void OAlsa::LoopEnd() {
    m_loop_state = false;
//    ctl_out.push(CTL_COMMAND(0xba, 1, 0));
//    ctl_out.push(CTL_COMMAND(0xba, 0, 4));
}

bool OAlsa::GetLoopState() {
    return m_loop_state;
}

void OAlsa::SetLoopState(bool state) {
//    ctl_out.push(CTL_COMMAND(0xba, 1, 0));
//    ctl_out.push(CTL_COMMAND(0xba, 0, 0));
    m_loop_state = state;
}

void OAlsa::Notify(JACK_EVENT event) {
    m_parent->notify_jack(event);
}
