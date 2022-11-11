/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OMidiMtc.h
 * Author: onkel
 *
 * Created on December 12, 2021, 9:33 AM
 */

#ifndef OMIDIMTC_H
#define OMIDIMTC_H

#include <string>
#include <stdint.h>
#include <stdlib.h>

class OMidiMtc {
public:

    void FullFrame(uint8_t* frame_data);
    int QuarterFrame(uint8_t data);
    void SetFrame(int);
    int GetFrame();
    std::string GetTimeCode();
    bool m_edge_sec = false;
    uint8_t diggit[4] = {0, 0, 0, 0};

private:
    uint8_t m_subframe = 0;

    uint8_t n1 = 0;
    uint8_t n2 = 0;
    std::string m_timecode = "00:00:00:00";
    bool lock_frame = false;
    
    uint32_t m_frame4;
};


#endif /* OMIDIMTC_H */

