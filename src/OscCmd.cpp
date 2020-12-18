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

OscCmd::OscCmd() : m_path(""), m_types(""), last_float(0) {
    m_color.set_rgba_u(32768, 32768, 32768);
    m_path = "";
}

OscCmd::OscCmd(const char* path, const char* types) {
    m_color.set_rgba_u(32768, 32768, 32768);
    m_path = path;
    m_types = types;
    last_float = 0.;
}

OscCmd::~OscCmd() {

}

std::string OscCmd::GetPathStr() {
    return m_path;
}

void OscCmd::SetPathStr(std::string path) {
    m_path = path;
    if (path != "")
        SplitPath(path);
}


bool OscCmd::IsConfig() {
    if (m_elements.at(3) == "config")
        return true;
    return false;
}

std::string OscCmd::GetConfigName() {
    return m_configName;
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
    
    if (strcmp(m_elements.at(1).data(), "ch") == 0) {
        char qn[64];
        sprintf(qn, "/ch/%s/config/name", m_elements.at(2).data());
        m_configName = qn;
    }
}
