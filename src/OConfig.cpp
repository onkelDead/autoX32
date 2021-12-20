/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OConfig.cpp
 * Author: onkel
 * 
 * Created on December 16, 2021, 9:40 AM
 */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "OConfig.h"

OConfig::OConfig() {
    config_init(&m_cfg);
    load_config();
}

OConfig::OConfig(const OConfig& orig) {
}

OConfig::~OConfig() {
    config_write_file(&m_cfg, m_conf_path);
    config_destroy(&m_cfg);
}

int OConfig::load_config() {
    const char *str;
    
    
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    sprintf(m_conf_path, CONF_FORMATTER, homedir);
    if (!config_read_file(&m_cfg, m_conf_path)) {
        m_root = config_root_setting(&m_cfg);
        create_default();
    }
    else
        m_root = config_root_setting(&m_cfg);
    
    return 0;
}

void OConfig::create_default() {
    set_boolean("mixer-autoconnect", true);
    set_string("mixer-host", "192.168.178.43");
    set_boolean("daw-autoconnect", true);
    set_string("daw-host", "localhost");
    set_string("daw-port", "3819");
    set_string("daw-reply-port", "38200");
    set_int("window-width", 600);
    set_int("window-height", 500);
    set_int("window-height", 500);
    set_int("window-top", 200);
    set_int("window-left", 200);
}

bool OConfig::get_boolean(const char* path) {
    config_setting_t* s;
    int ret;

    if (config_lookup_bool(&m_cfg, path, &ret) == CONFIG_TRUE) {
        return ret == 1;
    }
    s = config_setting_add(m_root, path, CONFIG_TYPE_BOOL);
    config_setting_set_bool(s, false);
    
    return false;
}

void OConfig::set_boolean(const char* path, bool val) {
    config_setting_t* s;
    
    s = config_setting_lookup(m_root, path);
    
    if (!s) {
        s = config_setting_add(m_root, path, CONFIG_TYPE_BOOL);        
    }
    config_setting_set_bool(s, val);
}

int OConfig::get_int(const char* path) {
    config_setting_t* s;
    int ret;

    if (config_lookup_int(&m_cfg, path, &ret) == CONFIG_TRUE) {
        return ret;
    }
    s = config_setting_add(m_root, path, CONFIG_TYPE_INT);
    config_setting_set_int(s, 0);
    
    return 0;
}

int OConfig::get_int(const char* path, int def_val) {
    config_setting_t* s;
    int ret;

    if (config_lookup_int(&m_cfg, path, &ret) == CONFIG_TRUE) {
        return ret;
    }
    s = config_setting_add(m_root, path, CONFIG_TYPE_INT);
    config_setting_set_int(s, def_val);
    
    return def_val;
}

void OConfig::set_int(const char* path, int val) {
    config_setting_t* s;
    
    s = config_setting_lookup(m_root, path);
    
    if (!s) {
        s = config_setting_add(m_root, path, CONFIG_TYPE_INT);        
    }
    config_setting_set_int(s, val);
}

const char* OConfig::get_string(const char* path) {
    config_setting_t* s;
    const char* ret;

    if (config_lookup_string(&m_cfg, path, &ret) == CONFIG_TRUE) {
        return ret;
    }
    s = config_setting_add(m_root, path, CONFIG_TYPE_STRING);
    config_setting_set_string(s, "");
    
    return "";
}

void OConfig::set_string(const char* path, const char* val) {
    config_setting_t* s;
    
    s = config_setting_lookup(m_root, path);
    
    if (!s) {
        s = config_setting_add(m_root, path, CONFIG_TYPE_STRING);        
    }
    config_setting_set_string(s, val);
}
