/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IOTracksLayout.h
 * Author: onkel
 *
 * Created on December 4, 2021, 9:16 AM
 */

#ifndef IOTRACKSLAYOUT_H
#define IOTRACKSLAYOUT_H

class IOTracksLayout {
public:
    virtual gint GetTrackIndex(std::string) = 0;
};

#endif /* IOTRACKSLAYOUT_H */

