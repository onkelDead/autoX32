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
    hour = frame_data[5] & 0x1f;
    min = frame_data[6];
    sec = frame_data[7];
    frame = frame_data[8];
    subframe = 0;
}

void OMidiMtc::QuarterFrame(uint8_t data) {
    lock_millis = true;
    subframe++;
    if (subframe == 4) {
        subframe = 0;
        frame++;
    }
    if (frame == 30) {
        frame = 0;
        sec++;
        m_edge_sec = true;
    }
    if (sec == 60) {
        sec = 0;
        min++;
    }
    if (min == 60) {
        min = 0;
        hour++;
    }

    lock_millis = false;
}

int OMidiMtc::GetMillis() {
    while (lock_millis);
    return hour * 432000 
            + min * 7200 
            + sec * 120 
            + (frame * 4) + (subframe);
}

void OMidiMtc::SetFrame(int f) {
    hour = f / 432000;
    f -= hour * 432000 ;
    min = (f / 7200 % 60);
    f -= min * 7200;
    sec = (f / 120) % 60;
    f -= sec * 120;
    frame = (f / 4 ) % 30;
    subframe = 0;
}

std::string OMidiMtc::GetTimeCode() {
    char t[32];

    sprintf(t, "%02d:%02d:%02d:%02d", hour, min, sec, frame);
    m_timecode = t;
    return m_timecode;
}
