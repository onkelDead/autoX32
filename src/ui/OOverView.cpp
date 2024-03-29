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


#include <gdkmm-3.0/gdkmm/cursor.h>
#include <gtk-3.0/gdk/gdkevents.h>
#include "OOverView.h"

OOverView::OOverView(IOMainWnd* wnd, daw_time* dt) : m_daw_time(dt), m_parent(wnd) {

    set_name("OOVerView");
}

OOverView::~OOverView() {
}

Gtk::SizeRequestMode OOverView::get_request_mode_vfunc() const {
    return Gtk::Widget::get_request_mode_vfunc();
}

void OOverView::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OOverView::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OOverView::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OOverView::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OOverView::on_size_allocate(Gtk::Allocation& allocation) {
    set_allocation(allocation);

    if (m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height());
    }
}

void OOverView::on_map() {
    Gtk::Widget::on_map();
}

void OOverView::on_unmap() {
    Gtk::Widget::on_unmap();
}

void OOverView::on_realize() {
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
        m_left_cursor = Gdk::Cursor::create(Gdk::CursorType::LEFT_SIDE);
        m_right_cursor = Gdk::Cursor::create(Gdk::CursorType::RIGHT_SIDE);
        m_shift_cursor = Gdk::Cursor::create(Gdk::CursorType::SB_H_DOUBLE_ARROW);
        m_drag_mode = OV_NONE;
    }
}

void OOverView::on_unrealize() {
    m_refGdkWindow.reset();

    Gtk::Widget::on_unrealize();
}

bool OOverView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    const Gtk::Allocation allocation = get_allocation();
    auto refStyleContext = get_style_context();

    int height = allocation.get_height();
    m_width = allocation.get_width();

    m_left = ((float) m_daw_time->m_viewstart / (float) m_daw_time->m_maxframes) * m_width;
    m_right = ((float) m_daw_time->m_viewend / (float) m_daw_time->m_maxframes) * m_width;

    cr->set_source_rgb(.3, .3, .3);
    cr->rectangle(m_left, 0, m_right - m_left, 80);
    cr->stroke_preserve();
    cr->fill();

    cr->set_source_rgb(.6, .8, .6);
    cr->move_to(m_left, 0);
    cr->line_to(m_left, height);
    cr->stroke();
    cr->set_source_rgb(.8, .6, .6);
    cr->move_to(m_right, 0);
    cr->line_to(m_right, height);
    cr->stroke();

    cr->set_line_width(1.0);
    cr->set_source_rgb(.8, .0, .0);
    cr->move_to(m_pos, 0);
    cr->line_to(m_pos, height);
    cr->stroke();
    
    
    return true;
}

bool OOverView::on_motion_notify_event(GdkEventMotion* motion_event) {
    if (motion_event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion* e = (GdkEventMotion*) motion_event;

        gint offset = (gint) e->x - (gint) m_last_x;
        if (abs(offset) > 1) {
            if (!m_in_drag) {
                UpdateCursor();
            } else {
                if (m_drag_mode == OV_LEFT_SIZE) {
                    if (m_last_x < 0) m_last_x = 0;
                    if (m_last_x >= m_right)
                        return true;
                    m_left = m_last_x;
                    UpdateDawTime(); 
                }
                if (m_drag_mode == OV_RIGHT_SIZE) {
                    if (m_last_x > m_width) m_last_x = m_width;
                    if (m_last_x <= m_left)
                        return true;
                    m_right = m_last_x;
                    UpdateDawTime(); 
                }
                if (m_drag_mode == OV_DRAG) {
                    if (m_right + offset > m_width)
                        return true;
                    if (m_left + offset < 0)
                        return true;
                    m_left += offset;
                    m_right += offset;
                    UpdateDawTime();        
                }
            }
            m_last_x = e->x;
        }
    }
    return true;
}

bool OOverView::on_button_press_event(GdkEventButton* event) {
    if (event->button == 1) {
        m_in_drag = true;
    }
    return false;
}

bool OOverView::on_button_release_event(GdkEventButton* event) {
    if (event->button == 1) {
        m_in_drag = false;
        UpdateCursor();
    }
    return false;
}

void OOverView::UpdateCursor() {
    if (abs(m_last_x - m_left) < 4) {
        if (m_drag_mode != OV_LEFT_SIZE) {
            m_refGdkWindow.get()->set_cursor(m_left_cursor);
            m_drag_mode = OV_LEFT_SIZE;
        }
        return;
    } else if (abs(m_last_x - m_right) < 4) {
        if (m_drag_mode != OV_RIGHT_SIZE) {
            m_refGdkWindow.get()->set_cursor(m_right_cursor);
            m_drag_mode = OV_RIGHT_SIZE;
        }
        return;
    } else if (m_last_x > m_left && m_last_x < m_right) {
        if (m_drag_mode != OV_DRAG) {
            m_refGdkWindow.get()->set_cursor(m_shift_cursor);
            m_drag_mode = OV_DRAG;
        }
        return;
    }

    if (m_drag_mode != OV_NONE) {
        m_refGdkWindow.get()->set_cursor(m_default_cursor);
        m_drag_mode = OV_NONE;
    }
}

void OOverView::UpdateDawTime() {
    m_daw_time->m_viewstart = ((float) m_left / (float) m_width) * m_daw_time->m_maxframes;
    m_daw_time->m_viewend = ((float) m_right / (float) m_width) * m_daw_time->m_maxframes;
    m_daw_time->scale = (gfloat) (m_width - 160) / (gfloat) (m_daw_time->m_viewend - m_daw_time->m_viewstart);
    m_parent->notify_overview();
}

bool OOverView::on_scroll_event(GdkEventScroll* scroll_event) {
    if (scroll_event->state == 16) {
        if (scroll_event->direction == GDK_SCROLL_DOWN) {
            gdouble l = m_left;
            m_left -= (m_right - l);
            m_right -= (m_right - l);
            if (m_left < 0.0) {
                m_right -= m_left;
                m_left = 0;
            }
            UpdateDawTime();
        }
        if (scroll_event->direction == GDK_SCROLL_UP) {
            gdouble l = m_left;
            m_left += (m_right - l);
            m_right += (m_right - l);
            if (m_right > m_width) {
                m_left -= (m_right - m_width);
                m_right = m_width;
            }
            UpdateDawTime();
        }
    }
    if (scroll_event->state == 20) {
        gdouble delta = (m_right - m_left) / 4;
        if (scroll_event->direction == GDK_SCROLL_DOWN) {
            m_left -= delta;
            m_right += delta;
            UpdateDawTime();
        }
        if (scroll_event->direction == GDK_SCROLL_UP) {
            m_left += delta;
            m_right -= delta;
            UpdateDawTime();
        }
    }
    return true;
}

void OOverView::SetFrame(gint frame) {

    m_pos = ((float)frame / (float) m_daw_time->m_maxframes) * m_width;
    if (m_last_pos != m_pos) {
        m_last_pos = m_pos;
        queue_draw();
    }
}