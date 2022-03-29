/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscMessage.h
 * Author: onkel
 *
 * Created on March 16, 2022, 9:52 PM
 */

#ifndef OSCMESSAGE_H
#define OSCMESSAGE_H

#include <vector>
#include <string>
#include <iostream>

#include "IOMessageHandler.h"
#include "IOscMessage.h"
#include "OscValue.h"

class OscMessage : public IOscMessage {
public:
    OscMessage();
    OscMessage(const OscMessage& orig);
    OscMessage(char const*, const char*);
    
    virtual ~OscMessage();

    void SetPathElements(std::vector<std::string> PathElements);
    std::vector<std::string> GetPathElements() const;

    int Parse();

    std::string GetConfigRequestName() {
        std::string sep = "/";
        std::string path = m_PathElements.at(0) + sep + m_PathElements.at(1) + sep + "config/name" ;
        return path;
    }
    
    std::string GetConfigRequestColor() {
        std::string sep = "/";
        std::string path = m_PathElements.at(0) + sep + m_PathElements.at(1) + sep + "config/color" ;
        return path;
    }    
    
    void Print();
    
    // Getter and setter
    void SetPath(std::string path) {
        m_path = path;
    }

    std::string GetPath() {
        return m_path;
    }

    void SetTypes(char* types) {
        m_types = types;
    }

    const char* GetTypes() const {
        return m_types;
    }
    void SetVal(OscValue* vals);
    
    OscValue* GetVal(int index) const;

    std::vector<OscValue*> GetVals() const {
        return m_vals;
    }

    void SetConfig(bool Config) {
        m_isConfig = Config;
    }

    bool IsConfig() const {
        return m_isConfig;
    }

    void SetTrackstore(IOTrackStore* Trackstore) {
        m_Trackstore = Trackstore;
    }

    IOTrackStore* GetTrackstore() const {
        return m_Trackstore;
    }

    
private:
    std::string m_path;
    char* m_types = nullptr;
    std::vector<OscValue*> m_vals;
    std::vector<std::string> m_PathElements;
    bool m_HasValue;
    bool m_isConfig;
    
    IOTrackStore* m_Trackstore = nullptr;

};

#endif /* OSCMESSAGE_H */


