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

#include <stdio.h>
#include <string.h>
#include "OX32.h"

OX32::OX32() :
m_X32_socket_fd(-1),
m_ReceiveBufferLen(0),
m_SendBufferLen(0),
m_IsConnected(0),
m_WorkerThread(NULL) {
}

OX32::~OX32() {
    delete m_WorkerThread;
}

int OX32::Connect(std::string host, IOMainWnd* wnd) {
    m_parent = wnd;
    int p_status; // poll status

    if ((m_X32_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("failed to create X32 socket");
        return 1;
    }
    memset(&m_Socket, 0, sizeof (m_Socket)); // Clear struct
    m_Socket.sin_family = AF_INET; // Internet/IP
    m_Socket.sin_addr.s_addr = inet_addr(host.data()); // X32 IP address
    m_Socket.sin_port = htons(10023); // X32 port

    m_timeout.tv_sec = 0;
    m_timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
    FD_ZERO(&m_ReceiveFd);
    FD_SET(m_X32_socket_fd, &m_ReceiveFd);

    lo_message msg_xinfo = lo_message_new();
    lo_message_serialise(msg_xinfo, "/xinfo", m_SendBuffer, &m_SendBufferLen);
    lo_message_free(msg_xinfo);

    socklen_t Xip_len = sizeof (m_Socket);

    time_t connect_timeout;
    time(&connect_timeout);
    m_IsConnected = 1;
    while (m_IsConnected) {
        time_t now;
        time(&now);
        if (sendto(m_X32_socket_fd, m_SendBuffer, m_SendBufferLen, 0, m_SocketPtr, Xip_len) < 0) {
            perror("couldn't send data to X32");
            return 1;
        }
        do {
            FD_ZERO(&m_ReceiveFd);
            FD_SET(m_X32_socket_fd, &m_ReceiveFd);
            p_status = select(m_X32_socket_fd + 1, &m_ReceiveFd, NULL, NULL, NULL);
        } while (0);
        if (p_status < 0) {
            printf("Polling for data failed\n");
            return 1; // exit on receive error
        }
        else if (p_status > 0) { // We have received data - process it!
            m_ReceiveBufferLen = recvfrom(m_X32_socket_fd, m_ReceiveBuffer, XBRMAX, 0, m_SocketPtr, &Xip_len);
            if (strcmp(m_ReceiveBuffer, "/xinfo") == 0) {
                int result;
                lo_message msg = lo_message_deserialise(m_ReceiveBuffer, m_ReceiveBufferLen, &result);
                printf("%s ", m_ReceiveBuffer);
                lo_message_free(msg);
                break; // Connected!
            }
        } // ... else timeout

        if (difftime(now, connect_timeout) > 2) {
            return 1;
        }
        printf(".");
        fflush(stdout);
    }
    printf(" Done!\n");
    m_IsConnected = 1;

    sigc::slot<bool> my_slot = sigc::mem_fun(*this, &OX32::on_timeout);
    m_timer = Glib::signal_timeout().connect(my_slot, 1000);

    m_WorkerThread = new std::thread([this] {
        do_work(this);
    });
    return 0;
}

bool OX32::on_timeout() {
    socklen_t Xip_len = sizeof (m_Socket);
    lo_message msg = lo_message_new();
    lo_message_serialise(msg, "/xremote", m_SendBuffer, &m_SendBufferLen);
    if (sendto(m_X32_socket_fd, m_SendBuffer, m_SendBufferLen, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        perror("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
    return true;
}

int OX32::Disconnect() {

    m_timer.disconnect();
    m_IsConnected = 0;
    return 0;
}

void OX32::do_work(IOX32* caller) {
    socklen_t Xip_len = sizeof (m_Socket);
    int p_status; // poll status
    struct timeval t_rec;

    while (m_IsConnected) {

        do {
            FD_ZERO(&m_ReceiveFd);
            FD_SET(m_X32_socket_fd, &m_ReceiveFd);
            p_status = select(m_X32_socket_fd + 1, &m_ReceiveFd, NULL, NULL, NULL);
        } while (0);

        if (p_status > 0) {
            if ((m_ReceiveBufferLen = recvfrom(m_X32_socket_fd, m_ReceiveBuffer, XBRMAX, 0, m_SocketPtr, &Xip_len)) > 0) {
                gettimeofday(&t_rec, NULL); // get precise time
                ProcessOscCmd(m_ReceiveBuffer, m_ReceiveBufferLen);
            }
        }
    }
    if (m_X32_socket_fd) close(m_X32_socket_fd);
}

void OX32::ProcessOscCmd(char* entry, size_t len) {
    int result;
    lo_message msg = lo_message_deserialise(entry, len, &result);
    int argc = lo_message_get_argc(msg);
    lo_arg **argv = lo_message_get_argv(msg);
    OscCmd* cmd = new OscCmd(entry, lo_message_get_types(msg));

    if (argc > 0) {
        switch (cmd->m_types.data()[0]) {
            case 'f':
                cmd->last_float = argv[0]->f;
                break;
            case 'i':
                cmd->last_int = argv[0]->i;
                break;
            case 's':
                strcpy(cmd->last_str, &argv[0]->s);
                break;
        }
    }
    m_parent->notify_mixer(cmd);
    lo_message_free(msg);
}

int OX32::IsConnected() {
    return m_IsConnected;
}

void OX32::SendFloat(std::string path, float val) {
    socklen_t Xip_len = sizeof (m_Socket);
    lo_message msg = lo_message_new();
    lo_message_add_float(msg, val);
    lo_message_serialise(msg, path.data(), m_SendBuffer, &m_SendBufferLen);
    if (sendto(m_X32_socket_fd, m_SendBuffer, m_SendBufferLen, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        perror("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}

void OX32::SendInt(std::string path, int val) {
    lo_message msg = lo_message_new();
    lo_message_add_int32(msg, val);
    lo_message_serialise(msg, path.data(), m_SendBuffer, &m_SendBufferLen);
    if (sendto(m_X32_socket_fd, m_SendBuffer, m_SendBufferLen, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        perror("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}

void OX32::Send(std::string path) {
    lo_message msg = lo_message_new();
    lo_message_serialise(msg, path.data(), m_SendBuffer, &m_SendBufferLen);
    if (sendto(m_X32_socket_fd, m_SendBuffer, m_SendBufferLen, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        perror("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}
