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

#ifndef SRC_ODAW_H_
#define SRC_ODAW_H_

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

#include "IOMainWnd.h"

#include <thread>
#include <mutex>

#include <gtkmm.h>

#define FEEDBACK_MASTER 16
#define FEEDBACK_TIMECODE 64
#define FEEDBACK_TRANSPORT_POSITION_SAMPLES 1024
#define FEEDBACK_REPLY 16384

#define XBSMAX 512
#define XBRMAX 512

class ODAW {
public:
    ODAW();
    virtual ~ODAW();

    std::string GetTimeCode();
    gint GetCurrentSample();

    gint GetMaxSamples();
    gint GetBitRate();

    void ProcessCmd(const char*, lo_message);

    void Play();
    void Stop();
    void Test();
    void SetPosition(gint, bool);

    int connect(const char* host, const char* port, const char* replyport, IOMainWnd*);
    int disconnect();

    void ShortMessage(const char* cmd);

    int GetKeepOn();
    void SetKeepOn(int);

private:
    int m_keep_on;

    lo_server_thread m_server;
    lo_address m_client;

    IOMainWnd* m_parent;
    std::string timecode;
    gint m_sample;
    gint m_bitrate;
    gint m_maxsamples;
};

#endif /* SRC_ODAW_H_ */
