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
#include "OTimer.h"
#include "OscMessage.h"

OX32::OX32() {
    m_cache.SetCallback_handler(this);
}

OX32::~OX32() {
    if (IsConnected())
        Disconnect();
    m_timer.stop();
}

int OX32::Start() {

    m_timer.SetUserData(NULL);
    m_timer.setFunc(this);
    m_timer.setInterval(1000);
    m_timer.start();   
    return 0;
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
    
    lo_message msg;
    
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
            p_status = select(m_X32_socket_fd + 1, &m_in_fd, NULL, NULL, &resp_timeout);
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


    m_WorkerThread = new std::thread([this] {
        do_work(this);
    });
    return 0;
}

void OX32::OnTimer(void*) {
    lo_message msg = lo_message_new();
    lo_message_serialise(msg, "/xremote", m_out_buffer, &m_out_length);
    if (sendto(m_X32_socket_fd, m_out_buffer, m_out_length, 0, m_SocketPtr, sizeof (m_Socket)) < 0) {
        printf("send data to X32 failed");
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}

int OX32::Disconnect() {

    m_timer.stop();
    if (m_IsConnected) {
        m_IsConnected = 0;
        m_WorkerThread->join();
        delete m_WorkerThread;
    }
    return 0;
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
                FrameCallback(m_in_buffer, m_in_length);
            }
        }
    }
    if (m_X32_socket_fd)
        close(m_X32_socket_fd);
}


void OX32::PauseCallbackHandler(bool val) {
    m_pause_handler = val;
}


void OX32::SetMsg_callback(MessageCallback msg_callback, void* userPtr) {
    m_msg_callback = msg_callback;
    m_userPtr = userPtr;
}

int OX32::IsConnected() {
    return m_IsConnected;
}

void OX32::SendFloat(std::string path, float val) {
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

void OX32::SendString(std::string path, std::string val) {
    lo_message msg = lo_message_new();
    lo_message_add_string(msg, val.data());
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
        std::cerr << "OX32::Send: coundn't send data to X32." << std::endl;
        m_IsConnected = 0;
    }
    lo_message_free(msg);
}


void OX32::FrameCallback(char* entry, size_t len) {
    int result;
    lo_message msg;
    lo_arg **argv;
    
    // filter messages here
    if (entry[1] == '-') {
        msg = lo_message_deserialise(entry, len, &result);
        argv = lo_message_get_argv(msg);
        printf("%s ", entry);
        lo_message_pp(msg);
        if (strcmp(entry, "/-stat/selidx") == 0) {
            m_MessageHandler->ProcessSelectMessage((*argv)->i);
        }
        lo_message_free(msg);  
        return;
    }
    
    msg = lo_message_deserialise(entry, len, &result);
    argv = lo_message_get_argv(msg);
    
    int i = 0;
    IOscMessage* cached = m_cache.GetCachedMsg(entry);
    if (cached) {
        while(char c = cached->GetTypes()[i]) {
            switch(c) {
                case 'f':
                    cached->GetVal(i)->SetFloat(argv[i]->f);
                    break;
                case 's':
                    cached->GetVal(i)->SetString(&argv[i]->s);
                    break;
                case 'i':
                    cached->GetVal(i)->SetInteger(argv[i]->i32);
                    break;
            }
            i++;
        }
        m_cache.ProcessMessage(cached);
        lo_message_free(msg);    
        return;
    }
    
    OscMessage cmd(entry, lo_message_get_types(msg));

    while(char c = cmd.GetTypes()[i]) {
        switch(c) {
            case 'f':
                cmd.GetVal(i)->SetFloat(argv[i]->f);
                break;
            case 's':
                cmd.GetVal(i)->SetString(&argv[i]->s);
                break;
            case 'i':
                cmd.GetVal(i)->SetInteger(argv[i]->i32);
                break;
        }
        i++;
    }
    
    m_cache.NewMessage(&cmd);
    
    lo_message_free(msg);    
}

int OX32::NewMessageCallback(IOscMessage* msg) {
    if (m_pause_handler) return 0;
    
    if (m_MessageHandler != nullptr) {
        m_MessageHandler->NewMessageCallback(msg);
    }
    else {
        std::cout << "OX32::NewMessageCallback called with no handler." << std::endl;
    }
    return 0;
}

int OX32::UpdateMessageCallback(IOscMessage* val) {
    if (m_pause_handler) return 0;
    
    if (m_MessageHandler != nullptr) {
        m_MessageHandler->UpdateMessageCallback(val);
    }
    else {    
        std::cout << "OX32::UpdateMessageCallback called with no handler." << std::endl;
    }
    return 0;
}

bool OX32::GetCachedValue(std::string path, float* val) {
    return m_cache.GetCachedValue(path, val);
}
bool OX32::GetCachedValue(std::string path, int* val) {
    return m_cache.GetCachedValue(path, val);
}

bool OX32::GetCachedValue(std::string path, std::string* val) {
    return m_cache.GetCachedValue(path, val);
}

IOscMessage* OX32::AddCacheMessage(const char* path, const char* types) {
    return m_cache.AddCacheMessage(path, types);
}

IOscMessage* OX32::AddCacheMessage(const char* path, const char* types, const char* value) {
    
    IOscMessage* msg = m_cache.GetCachedMsg(path);
    if (msg == nullptr) {
        msg = m_cache.AddCacheMessage(path, types);
    }
    switch(msg->GetTypes()[0]) {
        case 's':
            msg->GetVal(0)->SetString(value);
            break;
        case 'i':
            msg->GetVal(0)->SetInteger(atoi(value));
            break;
        case 'f':
            msg->GetVal(0)->SetFloat(std::stof(value));
            break;
    }
    return msg;
}

void OX32::ReleaseCacheMessage(std::string path) {
    m_cache.ReleaseCacheMessage(path);
}

IOscMessage* OX32::GetCachedMessage(std::string path) {
    return m_cache.GetCachedMsg(path.c_str());
}

void OX32::Save(std::string location) {
    m_cache.Save(location);
}

void OX32::Load(std::string location) {
    m_cache.Load(location);
}
