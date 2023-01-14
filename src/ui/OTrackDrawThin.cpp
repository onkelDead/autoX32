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

#include "OTrackDrawBase.h"
#include "OTrackDrawThin.h"

OTrackDrawThin::OTrackDrawThin(IOThinWnd* wnd, daw_time *daw_time) : m_parent(wnd), m_daw_time(daw_time) {
}

OTrackDrawThin::~OTrackDrawThin() {
}

bool OTrackDrawThin::on_draw(const Cairo::RefPtr<Cairo::Context> &cairo) {
    const Gtk::Allocation allocation = get_allocation();
    char valType;

    int height = allocation.get_height();
    m_width = allocation.get_width();
    int last_val;

    if (m_trackstore != nullptr) {
        track_entry *entry = m_trackstore->GetHeadEntry();
        valType = m_trackstore->GetMessage()->GetVal(0)->GetType();


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
        
        if (m_parent->GetConfig()->get_boolean(SETTINGS_SHOW_PATH_ON_TRACK)) {
            draw_text(cairo, 2, 2, m_trackstore->GetName());
        }
    }

    return true;
}

bool OTrackDrawThin::on_button_press_event(GdkEventButton *event) {
    m_click_frame = event->x / m_daw_time->scale + m_daw_time->m_viewstart;
    signal_pos_changed.emit();
    return true;
}

void OTrackDrawThin::SetSignalPosChange(IOTimeView *t) {
    signal_pos_changed.connect(
            sigc::mem_fun(*t, &IOTimeView::on_timedraw_pos_changed));
}
