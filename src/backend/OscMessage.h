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
#include <lo/lo.h>

class OscMessage {
public:
    OscMessage();
    OscMessage(const OscMessage& orig);
    OscMessage(const char*, const char*);
    
    virtual ~OscMessage();

    void SetPathElements(std::vector<std::string> PathElements);
    std::vector<std::string> GetPathElements() const;

    bool Parse();

    
    // Getter and setter
    void SetPath(std::string path) {
        m_path = path;
    }

    std::string GetPath() const {
        return m_path;
    }
    
private:
    std::string m_path;
    std::string m_types;
    std::vector<lo_arg> m_args;
    std::vector<std::string> m_PathElements;
    bool m_HasValue;
};

#endif /* OSCMESSAGE_H */

