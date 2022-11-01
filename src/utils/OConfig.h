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

#ifndef OCONFIG_H
#define OCONFIG_H

#include <vector>
#include <string>
#include <libconfig.h>

#define SETTING_SMOOTH_SCREEN           "smooth-screen"    
#define SETTINGS_DAW_HOST               "daw-host"    
#define SETTINGS_DAW_PORT               "daw-port"
#define SETTINGS_DAW__REPLAY_PORT       "daw-reply-port"
#define SETTINGS_DAW_AUTOCONNECT        "daw-autoconnect"
#define SETTINGS_MIXER_HOST             "mixer-host"
#define SETTINGS_MIXER_AUTOCONNECT      "mixer-autoconnect"
    
#define SETTINGS_WINDOW_WIDTH           "window-width"    
#define SETTINGS_WINDOW_HEIGHT          "window-height"    
#define SETTINGS_WINDOW_TOP             "window-top"    
#define SETTINGS_WINDOW_LEFT            "window-left"    
    
#define SETTINGS_SHOW_PATH_ON_TRACK     "show-path-on-track"
    
#define SETTINGS_RECENT_PROJECTS        "recent-projects"    
#define SETTINGS_MIDI_BACKEND           "midi-backend"  

#define SETTINGS_TRACK_FILTER           "track-filter"

#define CONF_FORMATTER "%s/.config/autoX32/autoX32.conf"

class OConfig {
public:
    OConfig();
    OConfig(const OConfig& orig);
    virtual ~OConfig();
    
    int load_config();
    void create_default();
    
    bool get_boolean(const char* path);
    void set_boolean(const char* path, bool val);
    
    int get_int(const char* path);
    int get_int(const char* path, int def_val);
    void set_int(const char* path, int val);
    
    const char* get_string(const char* path);
    void set_string(const char* path, const char* val);
    
    void set_string_array(const char* path, std::vector<std::string> strings);
    
private:
    char m_conf_path[256];
    config_t m_cfg;
    config_setting_t* m_root;
    
};

#endif /* OCONFIG_H */

