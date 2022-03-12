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

#ifndef SRC_OX32_H_
#define SRC_OX32_H_

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <lo/lo.h>

#include <thread>
#include <mutex>

#include <gtkmm.h>
#include <sigc++/connection.h>

#include "IOMainWnd.h"
#include "IOMixer.h"
#include "OTimer.h"

#define X32_BUFFER_MAX 512

class OX32 : public IOMixer {
public:
    OX32();
    virtual ~OX32();

    /// connect to X32 mixer via OSC
    int Connect(std::string);
    int Disconnect();


    int IsConnected();

    void SetMixerCallback(MixerCallback, void*);


    virtual void SendFloat(std::string path, float val);
    virtual void SendInt(std::string path, int val);
    virtual void Send(std::string);

    bool on_timeout();


private:

    void do_work(IOMixer*);

    void ProcessOscCmd(char* entry, size_t len);

    int m_X32_socket_fd = -1;
    struct sockaddr_in m_Socket;
    struct sockaddr *m_SocketPtr = (struct sockaddr*) &m_Socket;
    struct timeval m_timeout;

    fd_set m_in_fd;

    char m_in_buffer[X32_BUFFER_MAX];
    size_t m_in_length = 0;

    char m_out_buffer[X32_BUFFER_MAX];
    size_t m_out_length = 0;
    int m_IsConnected = 0;

    std::thread* m_WorkerThread = nullptr;

    MixerCallback m_callback;
    void* m_caller;

    OTimer m_timer;
    //sigc::connection m_timer;
};


#endif /* SRC_OX32_H_ */