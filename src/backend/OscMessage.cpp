/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscMessage.cpp
 * Author: onkel
 * 
 * Created on March 16, 2022, 9:52 PM
 */

#include "OscMessage.h"



OscMessage::OscMessage() {
}

OscMessage::OscMessage(const OscMessage& orig) {
}

OscMessage::~OscMessage() {
}

OscMessage::OscMessage(const char* path, const char* types) {
    m_path = path;
    m_types = types;
}

bool OscMessage::Parse() {
    m_PathElements.clear();
    std::string::size_type prev_pos = 0, pos = 1;
    std::string s = GetPath();

    while ((pos = s.find('/', pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        m_PathElements.push_back(substring);

        prev_pos = ++pos;
    }

    m_PathElements.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
    
    return true;
}

void OscMessage::SetPathElements(std::vector<std::string> PathElements) {
    m_PathElements = PathElements;
}

std::vector<std::string> OscMessage::GetPathElements() const {
    return m_PathElements;
}


