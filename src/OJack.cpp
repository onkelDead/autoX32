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

static int mtc_counter = 0;

static uint8_t mtc_time_frames[8];

static uint8_t mmc_command[] = { 0xf0, 0x7f, 0x00, 0x06 };

static jack_midi_data_t midi_play[] = { 0xf0, 0x7f, 0x7e, 0x06, 0x03, 0xf7 };
static bool doPlay = false;
static jack_midi_data_t midi_stop[] = { 0xf0, 0x7f, 0x7e, 0x06, 0x01, 0xf7 };
static bool doStop = false;
static jack_midi_data_t find[] = { 0xf0, 0x7f, 0x7e, 0x06, 0x05, 0xf7 };
static bool doFind = false;
static jack_midi_data_t locate[] = { 0xf0, 0x7f, 0x7e, 0x06, 0x44, 0x06, 0x01, 0, 0, 0, 0, 0, 0xf7 };
static bool doLocate = false;

static int s_hour;
static int s_min;
static int s_sec;
static int s_fr;
static gint s_millis;

static int process_mmc_event(jack_midi_event_t event) {

	if (event.size > 4) {
		if (!memcmp(event.buffer, mmc_command, sizeof(mmc_command))) {
			//printf("MMC: got command with with time %d  ", event.time);
			switch (event.buffer[4]) {
			case 1:
				printf("  Stop\n");
				break;
			case 2:
				printf("  Play\n");
				break;
			case 3:
				printf("  Deferred Play\n");
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
		mtc_time_frames[event.buffer[1] >> 4] = event.buffer[1] & 0x0f;
		mtc_counter++;

		if (mtc_counter == 8) {
			mtc_counter = 0;
			jack->Notify(MTC_COMPLETE);
		}
	} else if (event.buffer[0] == 0xf0) {
		mtc_time_frames[7] = event.buffer[5] >> 4;
		mtc_time_frames[6] = event.buffer[5] & 0x0f;
		mtc_time_frames[5] = event.buffer[6] >> 4;
		mtc_time_frames[4] = event.buffer[6] & 0x0f;
		mtc_time_frames[3] = event.buffer[7] >> 4;
		mtc_time_frames[2] = event.buffer[7] & 0x0f;
		mtc_time_frames[1] = event.buffer[8] >> 4;
		mtc_time_frames[0] = event.buffer[8] & 0x0f;
		jack->Notify(MTC_COMPLETE);
	}

	s_hour = (mtc_time_frames[7] & 0x01) * 16 + (mtc_time_frames[6] & 0x0f);
	s_min = (mtc_time_frames[5] & 0x03) * 16 + (mtc_time_frames[4] & 0x0f);
	s_sec = (mtc_time_frames[3] & 0x03) * 16 + (mtc_time_frames[2] & 0x0f);
	s_fr = (mtc_time_frames[1] & 0x01) * 16 + (mtc_time_frames[0] & 0x0f);

	s_millis = s_hour * 3600000 + s_min * 60000 + s_sec * 1000 + (s_fr * 1000 / 30);

	return 1;
}

static int process(jack_nframes_t nframes, void *arg) {
	int i;
	void *port_buf = jack_port_get_buffer(mmc_in_port, nframes);
	//jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer (output_port, nframes);
	jack_midi_event_t in_event;
	jack_nframes_t event_index = 0;
	jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
	if (event_count > 0) {
		//printf("Ardour MMC in: have %d events\n", event_count);
		for (i = 0; i < event_count; i++) {
			jack_midi_event_get(&in_event, port_buf, i);
			if (!process_mmc_event(in_event)) {
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
			if (!process_mtc_event(in_event, ((OJack*)arg))) {
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
	if (doPlay) {
		unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof(midi_play));
		memcpy(buffer, midi_play, sizeof(midi_play));
		printf("\nplay send\n");
		doPlay = false;
	}
	if (doStop) {
		unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof(midi_stop));
		memcpy(buffer, midi_stop, sizeof(midi_stop));
		printf("\nstop send\n");
		doStop = false;
	}
	if (doFind) {
		unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof(find));
		memcpy(buffer, find, sizeof(find));
		printf("\nfind send\n");
		doFind = false;
	}
	if (doLocate) {
		unsigned char *buffer = jack_midi_event_reserve(port_buf, 0, sizeof(locate));
		memcpy(buffer, locate, sizeof(locate));
		printf("\nlocate send\n");
		doLocate = false;
	}
	return 0;
}

static void jack_shutdown(void *arg) {
	// TODO: fix me, what to do if jack server stops
	exit(1);
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

	if (jack_activate(m_jack_client)) {
		fprintf(stderr, "cannot activate client");
		return;
	}

	jack_connect (m_jack_client, "ardour:MTC out", "autoX32:Ardour MTC in");
	jack_connect (m_jack_client, "ardour:MMC out", "autoX32:Ardour MMC in");
	jack_connect (m_jack_client, "autoX32:Ardour MMC out", "ardour:MMC in");

	doPlay = true;
	usleep(100000);
	doStop = true;
}

void OJack::Play() {
	doPlay = true;
}

void OJack::Stop() {
	doStop = true;
}

void OJack::Locate(gint millis) {
	int mm = (millis % 1000) * 30 / 1000;
	int sec = (millis / 1000) % 60;
	int min = (millis / 60000) % 60;
	int hour = (millis / 3600000);
	locate[7] = hour;
	locate[8] = min;
	locate[9] = sec;
	locate[10] = mm;
	doLocate = true;
}

gint OJack::GetMillis() {
	return s_millis;
}

void OJack::Notify(JACK_EVENT event) {
	m_parent->notify_jack(event);
}

std::string OJack::GetTimeCode() {
	char timecode[32];

	sprintf(timecode, "%02d:%02d:%02d:%02d\n", s_hour, s_min, s_sec, s_fr);
	m_timecode = timecode;

	return m_timecode;

}
