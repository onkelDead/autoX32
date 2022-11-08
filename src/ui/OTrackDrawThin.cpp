/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OTrackDrawThin.cpp
 * Author: onkel
 * 
 * Created on November 6, 2022, 4:06 PM
 */

#include "OTrackDrawThin.h"

OTrackDrawThin::OTrackDrawThin(daw_time *daw_time) : m_daw_time(daw_time) {
}

OTrackDrawThin::~OTrackDrawThin() {
}

bool OTrackDrawThin::on_draw(const Cairo::RefPtr<Cairo::Context> &cairo) {
    const Gtk::Allocation allocation = get_allocation();
    char valType;
    std::string path;

    int height = allocation.get_height();
    m_width = allocation.get_width();
    int last_val;

    if (m_trackstore != nullptr) {
        track_entry *entry = m_trackstore->GetHeadEntry();
        valType = m_trackstore->GetMessage()->GetVal(0)->GetType();
        
        path = m_trackstore->GetPath();

        cairo->set_source_rgb(.4, 0., 0.);
        GetColorByIndex(cairo, m_trackstore->GetColor_index());
        cairo->move_to(0, 0);
        cairo->line_to(m_width, 0);
        cairo->stroke();
        cairo->move_to(0, height);
        cairo->line_to(m_width, height);
        cairo->stroke();

        cairo->set_line_width(1);

        GetColorByIndex(cairo, m_trackstore->GetColor_index());

        while (entry->next && entry->next->time < m_daw_time->m_viewstart) {
            entry = entry->next;
        }

        if (entry) {
            last_val = -3 + height - (height - 6) * GetHeight(entry->val, valType);
            cairo->move_to(0, last_val);
            entry = entry->next;
            while (entry) {
                int pos = (entry->time - m_daw_time->m_viewstart) * m_daw_time->scale;
                cairo->line_to(pos, last_val);
                cairo->stroke();
                cairo->move_to(pos, last_val);
                last_val = -3 + height - (height - 6) * GetHeight(entry->val, valType);
                cairo->line_to(pos, last_val);
                cairo->stroke();
                cairo->move_to(pos, last_val);
                entry = entry->next;
                if (entry && entry->time > m_daw_time->m_viewend)
                    break;
            }
            cairo->line_to(m_width, last_val);
            cairo->stroke();
        }
    }
    return true;
}

void OTrackDrawThin::SetTrackStore(IOTrackStore *trackstore) {
    m_trackstore = trackstore;
}

void OTrackDrawThin::GetColorByIndex(const Cairo::RefPtr<Cairo::Context> &cr, int index) {
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

float OTrackDrawThin::GetHeight(lo_arg it, char t) {
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