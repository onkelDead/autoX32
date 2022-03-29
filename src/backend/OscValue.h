/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscValue.h
 * Author: onkel
 *
 * Created on March 18, 2022, 5:46 PM
 */

#ifndef OSCVALUE_H
#define OSCVALUE_H

#include <string>
#include "lo/lo.h"

class OscValue {
public:
    OscValue(const lo_arg& arg, const char type);
    OscValue(const OscValue&);
    OscValue(char t);
    virtual ~OscValue();

    void SetString(std::string string) {
        m_string = string;
    }

    std::string GetString() const {
        return m_string;
    }

    void SetType(char type) {
        m_type = type;
    }

    char GetType() const {
        return m_type;
    }

    void SetInteger(int integer) {
        m_integer = integer;
    }

    int GetInteger() const {
        return m_integer;
    }

    void SetFloat(float f) {
        m_float = f;
    }

    float GetFloat() const {
        return m_float;
    }
    
    OscValue& operator=(const OscValue& right) {
        switch(right.m_type) {
            case 's':
                SetString(right.GetString());
                break;
            case 'i':
                SetInteger(right.GetInteger());
                break;
            case 'f':
                SetFloat(right.GetFloat());
                break;
        }    
        return *this;
    }


private:
    char m_type;
    std::string m_string;
    int m_integer;
    float m_float;
};

#endif /* OSCVALUE_H */

