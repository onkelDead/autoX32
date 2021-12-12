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
	void QuarterFrame(uint8_t data);
        void SetFrame(int);
	int GetMillis();
	std::string GetTimeCode();
        bool m_edge_sec = false;

private:
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;
	uint8_t frame = 0;
	uint8_t subframe = 0;

	uint8_t n1 = 0;
	uint8_t n2 = 0;
	std::string m_timecode;
	int millis;
	bool lock_millis = false;
};


#endif /* OMIDIMTC_H */

