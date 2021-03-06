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

#include "OMainWnd.h"
#include "ODAW.h"

void daw_err_handler(int num, const char *msg, const char *where) {
	fprintf(stderr, "ARDOUR_ERROR %d: %s at %s\n", num, msg, where);
}

int daw_handler(const char *path, const char *types, lo_arg **argv, int argc,
		lo_message data, void *user_data) {
	ODAW *daw = (ODAW*) user_data;
	if (daw->GetKeepOn() == 2) {
		daw->SetKeepOn(1);
	}
	daw->ProcessCmd(path, data);
	return 0;
}

ODAW::ODAW() {
}

ODAW::~ODAW() {

}

int ODAW::disconnect() {
	if (m_client)
		lo_address_free(m_client);
	m_keep_on = 0;
	lo_server_thread_free(m_server);
	return 0;
}

void ODAW::ShortMessage(const char *cmd) {
	lo_message msg = lo_message_new();
	lo_send_message(m_client, cmd, msg);
	lo_message_free(msg);
}

int ODAW::GetKeepOn() {
	return m_keep_on;
}

void ODAW::SetKeepOn(int val) {
	m_keep_on = val;
}

int ODAW::connect(const char *host, const char *port, const char *replyport,
		IOMainWnd *wnd) {

	m_parent = wnd;

	m_server = lo_server_thread_new(replyport, daw_err_handler);
	if (m_server == NULL) {
		return 1;
	}
	lo_server_thread_add_method(m_server, NULL, NULL, daw_handler, this);
	lo_server_thread_start(m_server);

	m_client = lo_address_new(host, port);

	lo_message msg = lo_message_new();
	lo_message_add_int32(msg, 0);
	lo_message_add_int32(msg, 0);
	lo_message_add_int32(msg,FEEDBACK_MASTER + FEEDBACK_HMSMS
					+ FEEDBACK_TRANSPORT_POSITION_SAMPLES + FEEDBACK_REPLY);
	lo_message_add_int32(msg, 1);

	gint ret = lo_send_message(m_client, "/set_surface", msg);
	if (ret == -1) {
		fprintf(stderr, "OSC client error %d: %s on %s\n",
				lo_address_errno(m_client), lo_address_errstr(m_client),
				lo_address_get_hostname(lo_message_get_source(msg)));
	}
	lo_message_free(msg);
	time_t connect_timeout;
	time(&connect_timeout);

	m_keep_on = 2;

	while (m_keep_on == 2) {
		time_t now;
		time(&now);

		if (difftime(now, connect_timeout) > 2) {
			printf("\ntimeout during connect to DAW\n");
			return 1;
		}
	}
	m_keep_on = 1;

	return 0;
}

void ODAW::ProcessCmd(const char *entry, lo_message msg) {
	DAW_PATH c = DAW_PATH::unknown;

//	printf("DAW: %s ", entry);
//	lo_message_pp(msg);

	if (0 == strcmp("/transport_play", entry)) {
		int argc = lo_message_get_argc(msg);
		if (argc == 1) {
			lo_arg **argv = lo_message_get_argv(msg);
			if (argv[0]->i)
				c = DAW_PATH::play;
		}
	}

	if (0 == strcmp("/transport_stop", entry)) {
		int argc = lo_message_get_argc(msg);
		if (argc == 1) {
			lo_arg **argv = lo_message_get_argv(msg);
			if (argv[0]->i)
				c = DAW_PATH::stop;
		}
	}

	if (0 == strcmp("/position/smpte", entry)) {
		int argc = lo_message_get_argc(msg);
		if (argc == 1) {
			lo_arg **argv = lo_message_get_argv(msg);
			//timecode.assign((char*) argv[0]);
			c = DAW_PATH::smpte;
		}
	}
	if (0 == strcmp("/position/samples", entry)) {
		int argc = lo_message_get_argc(msg);
		if (argc == 1) {
			lo_arg **argv = lo_message_get_argv(msg);
			m_sample = atoi(&argv[0]->s);
			c = DAW_PATH::samples;
		}
	}

	if (0 == strcmp("/reply", entry)) {
		lo_arg **argv = lo_message_get_argv(msg);
		int argc = lo_message_get_argc(msg);
		if (argc > 1) {
			m_bitrate = argv[1]->i;
		}
		if (argc > 2) {
			m_maxmillis = (int)argv[2]->i / (m_bitrate / 1000);
		}
		c = DAW_PATH::reply;
	}
	if (0 == strcmp("/position/time", entry)) {
		lo_arg **argv = lo_message_get_argv(msg);
		int argc = lo_message_get_argc(msg);
		if (argc == 1) {
			timecode.assign((char*) argv[0]);
			SetMillisFromTime((char*)argv[0]);
		}
		c = DAW_PATH::timestr;
	}
	if (c != DAW_PATH::unknown) {
		m_parent->notify_daw(c);
	}
}

std::string ODAW::GetTimeCode() {
	return timecode;

}

gint ODAW::GetCurrentSample() {
	return m_sample;
}

gint ODAW::GetMaxMillis() {
	return m_maxmillis;
}

gint ODAW::GetBitRate() {
	return m_bitrate;
}

void ODAW::SetMillisFromTime(char* timestr) {
	// 00:00:00.000
	timestr[2] = '\0';
	int h = atoi(timestr);
	timestr[5] = '\0';
	int m = atoi(timestr+3);
	timestr[8] = '\0';
	int s = atoi(timestr+6);
	int mm = atoi(timestr+9);

	m_millis = mm + s * 1000 + m *60000 + h * 3600000;
}

int ODAW::GetMilliSeconds() {
	return m_millis;
}

void ODAW::Play() {
	ShortMessage("/transport_play");
}

void ODAW::Stop() {
	ShortMessage("/transport_stop");
}

void ODAW::Test() {
	lo_message msg = lo_message_new();
	lo_message_add_int32(msg, 48000);
	lo_message_add_int32(msg, 0);
	lo_send_message(m_client, "/locate", msg);
	lo_message_free(msg);
}

void ODAW::SetPosition(gint pos, bool play) {
	lo_message msg = lo_message_new();
	lo_message_add_int32(msg, pos);
	lo_message_add_int32(msg, play ? 1 : 0);
	lo_send_message(m_client, "/locate", msg);
	lo_message_free(msg);
}
