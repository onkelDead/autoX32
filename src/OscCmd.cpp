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

#include "OscCmd.h"

OscCmd::OscCmd(const char* path, const char* types) : m_colorindex(0) {
    m_color.set_rgba_u(32768, 32768, 32768);
    m_path = path;
    m_types = types;
    last_float = 0.;
    last_int = 0;

}

OscCmd::OscCmd(const OscCmd &src) : m_colorindex(0) {
    m_color.set_rgba_u(32768, 32768, 32768);
    m_path = src.m_path;
    m_types = src.m_types;
    last_float = src.last_float;
    last_int = src.last_int;
}

OscCmd::~OscCmd() {

}

std::string OscCmd::GetPathStr() {
    return m_path;
}

void OscCmd::SetPathStr(std::string path) {
    m_path = path;
}

void OscCmd::Parse() {
    if (m_path != "")
        SplitPath(m_path);
}

bool OscCmd::IsConfig() {
    if (m_elements.size() > 3 &&  m_elements.at(3) == "config")
        return true;
    return false;
}

std::string OscCmd::GetConfigName() {
    return m_configName;
}

std::string OscCmd::GetConfigColor() {
    return m_configColor;
}

void OscCmd::SplitPath(std::string s) {
    m_elements.clear();
    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find('/', pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        m_elements.push_back(substring);

        prev_pos = ++pos;
    }

    m_elements.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
    
    if (m_elements.at(1) == "ch" || m_elements.at(1) == "bus") {
        char qn[64];
        sprintf(qn, "/%s/%s/config/name", m_elements.at(1).data(), m_elements.at(2).data());
        m_configName = qn;
        sprintf(qn, "/%s/%s/config/color", m_elements.at(1).data(), m_elements.at(2).data());
        m_configColor = qn;
    }
}

void OscCmd::SetColorIndex(int index) {
    switch(index) {
        case 0:
        case 8:
            m_color.set_rgba_u(0, 0, 0);
            break;
        case 1:
            m_color.set_rgba_u(32768, 0, 0);
            break;
        case 2:
            m_color.set_rgba_u(0, 32768, 0);
            break;
        case 3:
            m_color.set_rgba_u(32768, 32768, 0);
            break;
        case 4:
            m_color.set_rgba_u(0, 0, 32768);
            break;
        case 5:
            m_color.set_rgba_u(32768, 0, 32768);
            break;
        case 6:
            m_color.set_rgba_u(0, 32768, 32768);
            break;
        case 7:
            m_color.set_rgba_u(32768, 32768, 32768);
            break;
        case 9:
            m_color.set_rgba_u(65535, 0, 0);
            break;  
        case 10:
            m_color.set_rgba_u(0, 65535, 0);
            break;     
        case 11:
            m_color.set_rgba_u(65535, 65535, 0);
            break;       
        case 12:
            m_color.set_rgba_u(0, 0, 65535);
            break; 
        case 13:
            m_color.set_rgba_u(65535, 0, 65535);
            break;   
        case 14:
            m_color.set_rgba_u(0, 65535, 65535);
            break;            
        case 15:
            m_color.set_rgba_u(65535, 65535, 65535);
            break;
    }
}
