/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OTrackDrawBase.cpp
 * Author: onkel
 * 
 * Created on January 6, 2023, 8:59 AM
 */

#include <gdkmm-3.0/gdkmm/cursor.h>
#include <gtkmm/widget.h>
#include <pangomm/layout.h>

#include "IOMainWnd.h"
#include "OTrackDrawBase.h"

OTrackDrawBase::OTrackDrawBase() {
}

OTrackDrawBase::~OTrackDrawBase() {
}


void OTrackDrawBase::SetTrackStore(IOTrackStore *trackstore) {
    m_trackstore = trackstore;
}

IOTrackStore* OTrackDrawBase::GetTrackStore() {
    return m_trackstore;
}

IOscMessage* OTrackDrawBase::GetMessage() {
    return m_trackstore->GetMessage();
}


float OTrackDrawBase::GetHeight(lo_arg it, char t) {
    switch (t) {
        case 'f':
            return it.f;
            break;
        case 'i':
            return (float) it.i;
            break;
    }
    return 0;
}

void OTrackDrawBase::draw_text(const Cairo::RefPtr<Cairo::Context> &cr, int rectangle_width, int rectangle_height, std::string text) {
    Pango::FontDescription font;

    font.set_size(14 * Pango::SCALE);
    font.set_family("Sans");
    font.set_weight(Pango::WEIGHT_NORMAL);

    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    cr->set_source_rgb(.8, .8, .8);
    cr->move_to((rectangle_width) / 2, rectangle_height);

    layout->show_in_cairo_context(cr);
}

void OTrackDrawBase::GetColorByIndex(const Cairo::RefPtr<Cairo::Context> &cr, int index) {
    switch (index) {
        case 0:
        case 8:
            cr->set_source_rgb(0, 0, 0);
            break;
        case 1:
            cr->set_source_rgb(32768, 0, 0);
            break;
        case 2:
            cr->set_source_rgb(0, 32768, 0);
            break;
        case 3:
            cr->set_source_rgb(32768, 32768, 0);
            break;
        case 4:
            cr->set_source_rgb(0, 0, 32768);
            break;
        case 5:
            cr->set_source_rgb(32768, 0, 32768);
            break;
        case 6:
            cr->set_source_rgb(0, 32768, 32768);
            break;
        case 7:
            cr->set_source_rgb(32768, 32768, 32768);
            break;
        case 9:
            cr->set_source_rgb(65535, 0, 0);
            break;
        case 10:
            cr->set_source_rgb(0, 65535, 0);
            break;
        case 11:
            cr->set_source_rgb(65535, 65535, 0);
            break;
        case 12:
            cr->set_source_rgb(0, 0, 65535);
            break;
        case 13:
            cr->set_source_rgb(65535, 0, 65535);
            break;
        case 14:
            cr->set_source_rgb(0, 65535, 65535);
            break;
        case 15:
            cr->set_source_rgb(65535, 65535, 65535);
            break;
    }
}
