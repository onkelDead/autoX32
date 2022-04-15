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
#include <string.h>

OscMessage::OscMessage() {
}

void OscMessage::AddVal(OscValue* val) {
    m_vals.push_back(val);
}

OscValue* OscMessage::GetVal(int index) const {
    return m_vals.at(index);
}

void OscMessage::SetVal(OscValue* new_val) {
    switch (new_val->GetType()) {
        case 's':
            m_vals.at(0)->SetString(new_val->GetString());
            break;
        case 'i':
            m_vals.at(0)->SetInteger(new_val->GetInteger());
            break;
        case 'f':
            m_vals.at(0)->SetFloat(new_val->GetFloat());
            break;
    }
}

OscMessage::OscMessage(const OscMessage& orig) {
}

OscMessage::~OscMessage() {
    for (OscValue* v : m_vals)
        delete v;
    if (m_types)
        free(m_types);
}

OscMessage::OscMessage(char const* path, const char* types) {
    m_path = path;
    m_types = strdup(types);
    int i = 0;
    while (types[i]) {
        m_vals.push_back(new OscValue(types[i++]));
    }
    if (Parse() > 3) {
        if (m_PathElements.at(2) == "config") {
            m_isConfig = true;
        }
    }
}

int OscMessage::Parse() {
    m_PathElements.clear();
    std::string::size_type prev_pos = 0, pos = 1;
    std::string s = GetPath();

    while ((pos = s.find('/', pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        m_PathElements.push_back(substring);

        prev_pos = ++pos;
    }

    m_PathElements.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return (int) m_PathElements.size();
}

void OscMessage::SetPathElements(std::vector<std::string> PathElements) {
    m_PathElements = PathElements;
}

std::vector<std::string> OscMessage::GetPathElements() const {
    return m_PathElements;
}

void OscMessage::Print() {
    std::cout << "OscMessage: path: " << m_path << std::endl;
    std::cout << "OscMessage: types: " << m_types << std::endl;

    for (OscValue* arg : m_vals) {
        std::cout << "OscMessage: arg: ";
        switch (arg->GetType()) {
            case 's':
                std::cout << arg->GetString() << std::endl;
                break;
            case 'i':
                std::cout << arg->GetInteger() << std::endl;
                break;
            case 'f':
                std::cout << arg->GetFloat() << std::endl;
                break;
        }
    }
}
