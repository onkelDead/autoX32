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

OX32::OX32() {
}

OX32::~OX32() {
}

int OX32::Connect(std::string host) {
    int p_status; // poll status

    if ((m_X32_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        printf("failed to create X32 socket");
        return 1;
    }
    memset(&m_Socket, 0, sizeof (m_Socket));
    m_Socket.sin_family = AF_INET;
    m_Socket.sin_addr.s_addr = inet_addr(host.data());
    m_Socket.sin_port = htons(10023);

    m_timeout.tv_sec = 0;
    m_timeout.tv_usec = 500000;
    FD_ZERO(&m_in_fd);
    FD_SET(m_X32_socket_fd, &m_in_fd);

    lo_message msg_xinfo = lo_message_new();
    lo_message_serialise(msg_xinfo, "/xinfo", m_out_buffer, &m_out_length);
    lo_message_free(msg_xinfo);

    socklen_t ip_len = sizeof (m_Socket);

    struct timeval resp_timeout;
    resp_timeout.tv_sec = 1;
    resp_timeout.tv_usec = 0;

    time_t connect_timeout;
    time(&connect_timeout);
    m_IsConnected = 1;
    while (m_IsConnected) {
        time_t now;
        time(&now);
        if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, ip_len) < 0) {
            printf("couldn't send data to X32");
            return 1;
        }
        do {
            FD_ZERO(&m_in_fd);
            FD_SET(m_X32_socket_fd, &m_in_fd);
            p_status = select(m_X32_socket_fd + 1, &m_in_fd, NULL, NULL,
                    &resp_timeout);
        } while (0);
        if (p_status < 0) {
            printf("Polling for data failed\n");
            return 1; // exit on receive error
        } else if (p_status > 0) { // We have received data - process it!
            m_in_length = recvfrom(m_X32_socket_fd, m_in_buffer,
                    X32_BUFFER_MAX, 0, m_SocketPtr, &ip_len);
            if (strcmp(m_in_buffer, "/xinfo") == 0) {
                int result;
                lo_message msg = lo_message_deserialise(m_in_buffer, m_in_length, &result);
                printf("%s ", m_in_buffer);
                lo_message_free(msg);
                break; // Connected!
            }
        }

        if (difftime(now, connect_timeout) > 2) {
            m_IsConnected = 0;
            return 1;
        }
    }
    m_IsConnected = 1;

    sigc::slot<bool> my_slot = sigc::mem_fun(*this, &OX32::on_timeout);
    m_timer = Glib::signal_timeout().connect(my_slot, 1000);

    m_WorkerThread = new std::thread([this] {
        do_work(this);
    });
    return 0;
}

bool OX32::on_timeout() {
    socklen_t ip_len = sizeof (m_Socket);
    lo_message msg = lo_message_new();
    lo_message_serialise(msg, "/xremote", m_out_buffer, &m_out_length);
    if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        printf("send data to X32 failed");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
    return true;
}

int OX32::Disconnect() {

    m_timer.disconnect();
    if (m_IsConnected) {
        m_IsConnected = 0;
        m_WorkerThread->join();
        delete m_WorkerThread;
    }
    return 0;
}

void OX32::SetMixerCallback(MixerCallback callback, void* user_data) {
    m_callback = callback;
    m_caller = user_data;
}

void OX32::do_work(IOMixer *caller) {
    socklen_t ip_len = sizeof (m_Socket);
    int p_status; 
    struct timeval t_rec;

    struct timeval timeout;

    while (m_IsConnected) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        do {
            FD_ZERO(&m_in_fd);
            FD_SET(m_X32_socket_fd, &m_in_fd);
            p_status = select(m_X32_socket_fd + 1, &m_in_fd, NULL, NULL,
                    &timeout);
        } while (0);

        if (p_status > 0) {
            if ((m_in_length = recvfrom(m_X32_socket_fd, m_in_buffer,
                    X32_BUFFER_MAX, 0, m_SocketPtr, &ip_len)) > 0) {
                gettimeofday(&t_rec, NULL); // get precise time
                ProcessOscCmd(m_in_buffer, m_in_length);
            }
        }
    }
    if (m_X32_socket_fd)
        close(m_X32_socket_fd);
}

void OX32::ProcessOscCmd(char *entry, size_t len) {
    int result;
    lo_message msg = lo_message_deserialise(entry, len, &result);
    int argc = lo_message_get_argc(msg);
    lo_arg **argv = lo_message_get_argv(msg);
    OscCmd *cmd = new OscCmd(entry, lo_message_get_types(msg));

    if (argc > 0) {
        switch (cmd->GetTypes().data()[0]) {
            case 'f':
                cmd->SetLastFloat(argv[0]->f);
                break;
            case 'i':
                cmd->SetLastInt(argv[0]->i);
                break;
            case 's':
                cmd->SetLastStr(&argv[0]->s);
                break;
        }
    }
    m_callback(cmd, m_caller);
    lo_message_free(msg);
}

int OX32::IsConnected() {
    return m_IsConnected;
}

void OX32::SendFloat(std::string path, float val) {
    socklen_t ip_len = sizeof (m_Socket);
    lo_message msg = lo_message_new();
    lo_message_add_float(msg, val);
    lo_message_serialise(msg, path.data(), m_out_buffer, &m_out_length);
    if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        printf("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}

void OX32::SendInt(std::string path, int val) {
    lo_message msg = lo_message_new();
    lo_message_add_int32(msg, val);
    lo_message_serialise(msg, path.data(), m_out_buffer, &m_out_length);
    if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        perror("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}

void OX32::Send(std::string path) {
    lo_message msg = lo_message_new();
    lo_message_serialise(msg, path.data(), m_out_buffer, &m_out_length);
    if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        printf("coundn't send data to X32");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}
