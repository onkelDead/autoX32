/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ISerializer.h
 * Author: onkel
 *
 * Created on April 2, 2022, 11:39 PM
 */

#ifndef ISERIALIZER_H
#define ISERIALIZER_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

class ISerializer {
public:
    virtual void Save(xmlTextWriterPtr) = 0;
    
};

#endif /* ISERIALIZER_H */

