/*
 Copyright 2020 Detlef Urban <onkel@paraair.de>

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <gdkmm-3.0/gdkmm/cursor.h>
#include <gtkmm/widget.h>
#include <pangomm/layout.h>

#include "res/OResource.h"
#include "OTrackDraw.h"
#include "OX32.h"

OTrackDraw::OTrackDraw(IOMainWnd *wnd, daw_time *daw_time) :
m_current_cursor(Gdk::CursorType::ARROW), m_parent(wnd), m_daw_time(daw_time) {
}

OTrackDraw::~OTrackDraw() {
}

bool OTrackDraw::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    const Gtk::Allocation allocation = get_allocation();
    auto refStyleContext = get_style_context();
    char t;
    std::string path;
    
    int height = allocation.get_height();
    m_width = allocation.get_width();
    int last_val;

    track_entry *it = m_trackstore->GetHeadEntry();
    t = m_trackstore->GetMessage()->GetVal(0)->GetType();
    path = m_trackstore->GetName();

    if (m_in_drag) {
        cr->set_source_rgb(.3, .3, .3);
        cr->rectangle(m_left, 0, m_right - m_left, height);
        cr->stroke_preserve();
        cr->fill();
    }

    if (m_selected) {
        cr->set_source_rgb(.4, 0., 0.);
        GetColorByIndex(cr, m_trackstore->GetColor_index());
        cr->move_to(0, 0);
        cr->line_to(m_width, 0);
        cr->stroke();
        cr->move_to(0, height);
        cr->line_to(m_width, height);
        cr->stroke();
    }
    
    cr->set_line_width(1);
    
    GetColorByIndex(cr, m_trackstore->GetColor_index());
    
    while (it->next && it->next->time < m_daw_time->m_viewstart) {
        it = it->next;
    }

    if (it) {
        last_val = -3 + height - (height - 6) * GetHeight(it->val, t);
        cr->move_to(0, last_val);
        it = it->next;
        while (it) {
            gint pos = (it->time - m_daw_time->m_viewstart) * m_daw_time->scale;
            cr->line_to(pos, last_val);
            cr->stroke();
            cr->move_to(pos, last_val);
            last_val = -3 + height - (height - 6) * GetHeight(it->val, t);
            cr->line_to(pos, last_val);
            cr->stroke();
            cr->move_to(pos, last_val);
            it = it->next;
            if (it && it->time > m_daw_time->m_viewend)
                break;
        }
        cr->line_to(m_width, last_val);
        cr->stroke();
    }

    
    if (m_parent->GetConfig()->get_boolean(SETTINGS_SHOW_PATH_ON_TRACK))
        draw_text(cr, 2, 2, path);

    
//    cr->set_source_rgb(0.1, 0.1, 0.1);
//    for (int si = m_daw_time->m_viewstart; si < m_daw_time->m_viewend; si++) {
//        int pos = (si - m_daw_time->m_viewstart) * m_daw_time->scale;
//        cr->move_to(pos, 0);
//        cr->line_to(pos, height);
//        cr->stroke();
//    }

    return true;
}

void OTrackDraw::SetRecord(bool val) {
    m_trackstore->SetRecording(val);
}

void OTrackDraw::SetSelected(bool val) {
    m_selected = val;
}

bool OTrackDraw::on_button_press_event(GdkEventButton *event) {
    if (event->button == 1) {
        m_btn_down = 1;
        m_down_x = m_last_x;
    }
    if (event->button == 2) {
        if (m_current_cursor != Gdk::CursorType::FLEUR) {
            m_current_cursor = Gdk::CursorType::FLEUR;
            m_refGdkWindow.get()->set_cursor(m_shift_cursor);
        }
        m_btn_down = 2;
    }
    return false;
}

bool OTrackDraw::on_button_release_event(GdkEventButton *event) {
    if (event->button == 1) {
        if (m_in_drag) {
            m_in_drag = false;
            if (m_left > m_right) {
                int r = m_right;
                m_right = m_left;
                m_left = r;
            }
            int start = m_daw_time->m_viewstart;
            m_daw_time->m_viewstart += ((float) m_left / (float) m_width) * (m_daw_time->m_viewend - m_daw_time->m_viewstart);
            m_daw_time->m_viewend = m_daw_time->m_viewstart + ((float) (m_right - m_left) / (float) m_width) * (m_daw_time->m_viewend - start);
            m_daw_time->scale = (gfloat) m_width / (gfloat) (m_daw_time->m_viewend - m_daw_time->m_viewstart);
            m_parent->notify_overview();
        } else {
            m_parent->SelectTrackDraw(m_trackstore->GetPath());
        }
    }
    m_refGdkWindow.get()->set_cursor(m_default_cursor);
    m_current_cursor = Gdk::CursorType::ARROW;
    m_btn_down = 0;
    return false;
}

bool OTrackDraw::on_motion_notify_event(GdkEventMotion *motion_event) {
    if (motion_event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion *e = (GdkEventMotion*) motion_event;
        // Position->x changed
        if (m_last_x != (gint) e->x) {
            int offset = (gint) e->x - m_last_x;

            // react on left button pressed
            if (m_btn_down == 1) {
                int down_offset = (gint) e->x - m_down_x;
                // select time range not started
                if (!m_in_drag && abs(down_offset) > 10) {
                    // have to fip cursor
                    m_in_drag = true;
                    if (m_current_cursor != Gdk::CursorType::SIZING) {
                        m_refGdkWindow.get()->set_cursor(m_zoom_cursor);
                        m_current_cursor = Gdk::CursorType::SIZING;
                    }
                }
                if (down_offset < 0) {
                    m_left = (gint) e->x;
                    m_right = m_down_x;
                } else {
                    m_right = (gint) e->x;
                    m_left = m_down_x;
                }
                queue_draw();
            }
            if (m_btn_down == 2) {

                int start = m_daw_time->m_viewstart;
                m_daw_time->m_viewstart -= ((float) offset / (float) m_width) * (m_daw_time->m_viewend - m_daw_time->m_viewstart);
                m_daw_time->m_viewend -= ((float) offset / (float) m_width) * (m_daw_time->m_viewend - start);
                m_parent->notify_overview();
                queue_draw();
                
            }
            m_last_x = (gint) e->x;
        }
    }
    return true;
}
