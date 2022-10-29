/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOEngine.h
 * Author: onkel
 *
 * Created on June 13, 2022, 6:03 AM
 */

#ifndef IOENGINE_H
#define IOENGINE_H

class IOEngine {
public:
    virtual ~IOEngine(){}
    virtual void SelectTrack(std::string, bool) = 0;
};

#endif /* IOENGINE_H */

