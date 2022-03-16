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
    OscCmd(OscCmd &);
    virtual ~OscCmd();

    void CopyLastVal(OscCmd* src);
    
    std::string GetPath();
    void SetPath(std::string m_path);

    std::string GetName();
    void SetName(std::string);
    
    std::string GetTypes();
    void SetTypes(std::string);
    
    bool GetSolo();
    void SetSolo(bool);

    int GetChIndex();
    void SetChIndex(int);
    
    float GetLastFloat();
    void SetLastFloat(float);

    int GetLastInt();
    void SetLastInt(int);

    std::string GetLastStr();
    void SetLastStr(std::string);

    Gdk::RGBA GetColor();
    void SetColor(Gdk::RGBA);

    void Parse();
    
    std::string GetConfigRequestName();
    std::string GetConfigRequestColor();
    std::string GetStatsRequestSolo();
    
    void SetColorIndex(int);
    
    bool IsConfig();
    
private:
    
    void SplitPath(std::string s);
    
    std::vector<std::string> m_path_elements;
   
    std::string m_name_request = "";
    std::string m_color_request = "";
    std::string m_solo_request = "";
    
    std::string m_path = "";
    std::string m_name = "";
    int m_ch_index = 0;
    std::string m_types = "";
    int m_colorindex = 0;
    Gdk::RGBA m_color;

    float m_last_float = 0.;
    int m_last_int = 0;
    std::string m_last_str = "";
    bool m_solo = false;

};

#endif /* SRC_OSCCMD_H_ */
