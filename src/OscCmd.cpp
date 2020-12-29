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

OscCmd::OscCmd(const char* path, const char* types) : m_path(path), m_types(types)  {
    m_color.set_rgba_u(32768, 32768, 32768);

}

OscCmd::OscCmd(OscCmd &src) {
    m_color.set_rgba_u(32768, 32768, 32768);
    m_path = src.m_path;
    m_types = src.m_types;
    m_last_float = src.GetLastFloat();
    m_last_int = src.GetLastInt();
    m_colorindex = src.m_colorindex;
}

OscCmd::~OscCmd() {

}

std::string OscCmd::GetPath() {
    return m_path;
}

void OscCmd::SetPath(std::string path) {
    m_path = path;
}

std::string OscCmd::GetName() {
	return m_name;
}

void OscCmd::SetName(std::string name) {
	m_name = name;
}

std::string OscCmd::GetTypes() {
	return m_types;
}

void OscCmd::SetTypes(std::string types) {
	m_types = types;
}

float OscCmd::GetLastFloat() {
	return m_last_float;
}

void OscCmd::SetLastFloat(float val) {
	m_last_float = val;
}

int OscCmd::GetLastInt() {
	return m_last_int;
}

void OscCmd::SetLastInt(int val) {
	m_last_int = val;
}

std::string OscCmd::GetLastStr() {
	return m_last_str;
}

void OscCmd::SetLastStr(std::string str) {
	m_last_str = str;
}

Gdk::RGBA OscCmd::GetColor() {
	return m_color;
}

void OscCmd::SetColor(Gdk::RGBA color) {
	m_color = color;
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

std::string OscCmd::GetConfigRequestName() {
    return m_config_request_name;
}

std::string OscCmd::GetConfigRequestColor() {
    return m_config_request_color;
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
        m_config_request_name = qn;
        sprintf(qn, "/%s/%s/config/color", m_elements.at(1).data(), m_elements.at(2).data());
        m_config_request_color = qn;
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
