/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OConfig.h
 * Author: onkel
 *
 * Created on December 16, 2021, 9:40 AM
 */

#ifndef OCONFIG_H
#define OCONFIG_H

#include <libconfig.h>

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
    
private:
    char m_conf_path[256];
    config_t m_cfg;
    config_setting_t* m_root;
    
};

#endif /* OCONFIG_H */

