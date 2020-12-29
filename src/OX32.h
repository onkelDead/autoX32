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
#include "IOX32.h"

#define XBSMAX 512
#define XBRMAX 512


class OX32 : public IOX32 {
public:
    OX32(IOMainWnd*);
    virtual ~OX32();

    /// connect to X32 mixer via OSC
    int Connect(std::string);
    int Disconnect();

    int IsConnected();

    void ProcessOscCmd(char* entry, size_t len);

    virtual void SendFloat(std::string path, float val);
    virtual void SendInt(std::string path, int val);
    virtual void Send(std::string);

private:

    void do_work(IOX32*);

    int m_X32_socket_fd = -1;
    struct sockaddr_in m_Socket; // X socket IP we send/receive
    struct sockaddr *m_SocketPtr = (struct sockaddr*) &m_Socket; // X socket IP pointer we send/receive
    struct timeval m_timeout; // used for select()

    fd_set m_ReceiveFd; 

    char m_ReceiveBuffer[XBRMAX];
    size_t m_ReceiveBufferLen = 0;

    char m_SendBuffer[XBSMAX];
    size_t m_SendBufferLen = 0;
    int m_IsConnected = 0;

    std::thread* m_WorkerThread = nullptr;

    IOMainWnd* m_parent;

    bool on_timeout();
    sigc::connection m_timer;
};


#endif /* SRC_OX32_H_ */
