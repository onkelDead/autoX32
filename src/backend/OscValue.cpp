/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OscValue.cpp
 * Author: onkel
 * 
 * Created on March 18, 2022, 5:46 PM
 */

#include <string.h>
#include "OscValue.h"

OscValue::OscValue(char t) {
    SetType(t);
}

OscValue::OscValue(const lo_arg& arg, const char t) {
    switch (t) {
        case 's':
            SetString(&arg.s);
            SetType(t);
            break;
        case 'i':
            SetInteger(arg.i);
            SetType(t);
            break;
        case 'f':
            SetFloat(arg.f);
            SetType(t);
            break;
    }
}

OscValue::OscValue(const OscValue& orig) {
}

OscValue::~OscValue() {

}

void OscValue::ToString(const char t, char* s) {
    switch (t) {
        case 's':
            sprintf(s, "%s", GetString().data());
            break;
        case 'f':
            sprintf(s, "%f", GetFloat());
            break;
        case 'i':
            sprintf(s, "%d", GetInteger());
            break;
    }
}