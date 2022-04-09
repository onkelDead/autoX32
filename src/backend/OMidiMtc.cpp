/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OMidiMtc.cpp
 * Author: onkel
 * 
 * Created on December 12, 2021, 9:33 AM
 */

#include "OMidiMtc.h"

// JackMtc

void OMidiMtc::FullFrame(uint8_t *frame_data) {
    diggit[3] = frame_data[5] & 0x1f;
    diggit[2] = frame_data[6];
    diggit[1] = frame_data[7];
    diggit[0] = frame_data[8];
    subframe = 0;
}

int OMidiMtc::QuarterFrame(uint8_t data) {
    int ret = 0;
    lock_millis = true;
    subframe++;
    if (subframe == 4) {
        subframe = 0;
        diggit[0]++;
        ret = 2;
    }
    
    if (diggit[0] % 4 == 0) {
        m_edge_sec = true;
    }
    
    if (diggit[0] == 30) {
        diggit[0] = 0;
        diggit[1]++;
        ret = 4;
    }
    if (diggit[1] == 60) {
        diggit[1] = 0;
        diggit[2]++;
        ret = 6;
    }
    if (diggit[2] == 60) {
        diggit[2] = 0;
        diggit[3]++;
        ret = 8;
    }

    lock_millis = false;
    return ret;
}

int OMidiMtc::GetMillis() {
    int ret;
    while (lock_millis);
    ret = diggit[3] * 432000 
            + diggit[2] * 7200 
            + diggit[1] * 120 
            + (diggit[0] * 4) + (subframe);
    return ret;
}

void OMidiMtc::SetFrame(int f) {
    diggit[3] = f / 432000;
    f -= diggit[3] * 432000 ;
    diggit[2] = (f / 7200 % 60);
    f -= diggit[2] * 7200;
    diggit[1] = (f / 120) % 60;
    f -= diggit[1] * 120;
    diggit[0] = (f / 4 ) % 30;
    subframe = 0;
}

std::string OMidiMtc::GetTimeCode() {

    sprintf(m_timecode.data(), "%02d:%02d:%02d:%02d", diggit[3], diggit[2], diggit[1], diggit[0]);
    return m_timecode;
}
