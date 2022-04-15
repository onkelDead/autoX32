/*
 Copyright 2021 Detlef Urban <onkel@paraair.de>

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


#include "OQueue.h"

template <class T>
OQueue<T>::OQueue() {
}

template <class T>
OQueue<T>::OQueue(const OQueue& orig) {
}

template <class T>
OQueue<T>::~OQueue() {
}

template <class T>
void OQueue<T>::push(T t) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(t);
}

template <class T>
void OQueue<T>::front_pop(T* result) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        result = NULL;
        return;
    }
    *result = m_queue.front();
    m_queue.pop();
}

template class OQueue<operation_t*>;
template class OQueue<ctl_command*>;
template class OQueue<uint8_t>;
template class OQueue<DAW_PATH>;
template class OQueue<JACK_EVENT>;
template class OQueue<IOscMessage*>;


