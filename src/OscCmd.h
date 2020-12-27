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

#ifndef SRC_OSCCMD_H_
#define SRC_OSCCMD_H_

#include <gtkmm.h>
#include <gdkmm/rgba.h>

class OscCmd {
public:
    OscCmd(const char*, const char*);
    OscCmd(const OscCmd &);
    virtual ~OscCmd();

    std::string GetPathStr();
    void SetPathStr(std::string m_path);
    
    void Parse();
    
    std::string GetConfigName();    
    std::string GetConfigColor();
    
    void SetColorIndex(int);
    
    std::string m_name;
    std::string m_path;
    std::string m_types;

    Gdk::RGBA m_color;
    int m_colorindex;
    
    float last_float;
    int last_int;
    char last_str[64];

    bool IsConfig();
    
private:
    void SplitPath(std::string s);
    std::vector<std::string> m_elements;
    std::string m_configName;
    std::string m_configColor;
};

#endif /* SRC_OSCCMD_H_ */
