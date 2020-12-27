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
#include "OTrackDraw.h"
#include "OX32.h"

OTrackDraw::OTrackDraw(IOMainWnd* wnd) : m_current_cursor(Gdk::CursorType::ARROW), m_left(0), m_right(0), m_width(0){
    m_parent = wnd;
    m_in_drag = false;
    m_btn_down = 0;
    m_daw_time = 0;
    m_trackstore = NULL;
    m_last_x = 0;
}

OTrackDraw::~OTrackDraw() {
}

Gtk::SizeRequestMode OTrackDraw::get_request_mode_vfunc() const {
    return Gtk::Widget::get_request_mode_vfunc();
}

void OTrackDraw::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OTrackDraw::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OTrackDraw::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OTrackDraw::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OTrackDraw::on_size_allocate(Gtk::Allocation& allocation) {
    set_allocation(allocation);

    if (m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height());
    }
}

void OTrackDraw::on_map() {
    Gtk::Widget::on_map();
}

void OTrackDraw::on_unmap() {
    Gtk::Widget::on_unmap();
}

void OTrackDraw::on_realize() {
    set_realized();

    if (!m_refGdkWindow) {

        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof (attributes));

        Gtk::Allocation allocation = get_allocation();

        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events() |
                GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK |
                GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
                GDK_POINTER_MOTION_HINT_MASK | GDK_SCROLL_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes,
                GDK_WA_X | GDK_WA_Y);
        set_window(m_refGdkWindow);

        m_refGdkWindow->set_user_data(gobj());
        m_default_cursor = m_refGdkWindow.get()->get_cursor();
        m_zoom_cursor = Gdk::Cursor::create(Gdk::CursorType::SIZING);
        m_shift_cursor = Gdk::Cursor::create(Gdk::CursorType::FLEUR);
        m_current_cursor = Gdk::CursorType::ARROW;

    }
}

void OTrackDraw::on_unrealize() {
    m_refGdkWindow.reset();

    Gtk::Widget::on_unrealize();
}

float OTrackDraw::GetHeight(lo_arg it, char t) {
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

bool OTrackDraw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    const Gtk::Allocation allocation = get_allocation();
    auto refStyleContext = get_style_context();

    int height = allocation.get_height();
    m_width = allocation.get_width();
    int last_val;

    if (m_in_drag) {
        cr->set_source_rgb(.3, .3, .3);
        cr->rectangle(m_left, 0, m_right - m_left, 80);
        cr->stroke_preserve();
        cr->fill();
    }

    cr->set_line_width(1);
    track_entry *it = m_trackstore->m_tracks;

    cr->set_source_rgba(m_trackstore->m_cmd->m_color.get_red(), m_trackstore->m_cmd->m_color.get_green(),
            m_trackstore->m_cmd->m_color.get_blue(), m_trackstore->m_cmd->m_color.get_alpha());
    
    while(it->next && it->next->sample < m_daw_time->m_viewstart) {
        it = it->next;
    }
    
    if (it) {
        last_val = height - height * GetHeight(it->val, GetCmd()->m_types.data()[0]);
        cr->move_to(0, last_val);
        it = it->next;
        while (it) {
            gint pos = (it->sample - m_daw_time->m_viewstart) * m_daw_time->scale;
            cr->line_to(pos, last_val);
            cr->stroke();
            cr->move_to(pos, last_val);
            last_val = height - height * GetHeight(it->val, GetCmd()->m_types.data()[0]);
            cr->line_to(pos, last_val);
            cr->stroke();
            cr->move_to(pos, last_val);
            it = it->next;
            if ( it && it->sample > m_daw_time->m_viewend)
                break;
        }
        cr->line_to(m_width, last_val);
        cr->stroke();
    }
    
    if (m_parent->GetSettings()->get_boolean("show-path-on-track"))
        draw_text(cr, 2, 2, m_trackstore->m_cmd->GetPathStr());
    
    return true;
}

void OTrackDraw::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
        int rectangle_width, int rectangle_height, std::string text) {
    Pango::FontDescription font;

    font.set_size(7 * Pango::SCALE);
    font.set_family("Sans");
    font.set_weight(Pango::WEIGHT_NORMAL);

    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    cr->set_source_rgb(.8, .8, .8);
    cr->move_to((rectangle_width ) / 2, rectangle_height);

    layout->show_in_cairo_context(cr);
}

void OTrackDraw::SetRecord(bool val) {
    m_trackstore->m_record = val;
}

void OTrackDraw::SetTouch(bool val) {
    m_trackstore->m_touch = val;
}

void OTrackDraw::SetTrackStore(OTrackStore* trackstore) {
    m_trackstore = trackstore;
}

OscCmd* OTrackDraw::GetCmd() {
    return m_trackstore->m_cmd;
}

bool OTrackDraw::on_button_press_event(GdkEventButton* event) {
    if (event->button == 1) {
        m_btn_down = 1;
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

bool OTrackDraw::on_button_release_event(GdkEventButton* event) {
    if (event->button == 1 && m_in_drag) {
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
    }
    m_refGdkWindow.get()->set_cursor(m_default_cursor);
    m_current_cursor = Gdk::CursorType::ARROW;
    m_btn_down = 0;
    return false;
}

bool OTrackDraw::on_motion_notify_event(GdkEventMotion* motion_event) {
    if (motion_event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion* e = (GdkEventMotion*) motion_event;
        if (m_last_x != (gint) e->x) {
            int offset = (gint) e->x - m_last_x;
            if (m_btn_down == 1) {
                if (!m_in_drag) {
                    if (m_current_cursor != Gdk::CursorType::SIZING) {
                        m_refGdkWindow.get()->set_cursor(m_zoom_cursor);
                        m_current_cursor = Gdk::CursorType::SIZING;
                    }
                    m_left = m_right = (gint) e->x;
                    m_in_drag = true;
                } else {
                    m_right = (gint) e->x;
                }
                queue_draw();
            }
            if (m_btn_down == 2) {

                int start = m_daw_time->m_viewstart;
                m_daw_time->m_viewstart -= ((float) offset / (float) m_width) * (m_daw_time->m_viewend - m_daw_time->m_viewstart);
                m_daw_time->m_viewend -= ((float) offset / (float) m_width) * (m_daw_time->m_viewend - start);
                m_parent->notify_overview();
            }
            m_last_x = (gint) e->x;
        }
    }
    return true;
}
