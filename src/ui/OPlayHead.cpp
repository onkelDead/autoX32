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

#include <iostream>
#include "OPlayHead.h"

OPlayHead::OPlayHead() {
}

OPlayHead::~OPlayHead() {
}

Gtk::SizeRequestMode OPlayHead::get_request_mode_vfunc() const {
    return Gtk::Widget::get_request_mode_vfunc();
}


void OPlayHead::on_size_allocate(Gtk::Allocation& allocation) {
    set_allocation(allocation);

    if (m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height());
    }
}

void OPlayHead::on_map() {
    Gtk::Widget::on_map();
}

void OPlayHead::on_unmap() {
    Gtk::Widget::on_unmap();
}

void OPlayHead::on_realize() {
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
    }
}

void OPlayHead::on_unrealize() {
    m_refGdkWindow.reset();

    Gtk::Widget::on_unrealize();
}

bool OPlayHead::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    if (m_active) {
        const Gtk::Allocation allocation = get_allocation();
        auto refStyleContext = get_style_context();

        int height = allocation.get_height();
        cr->set_line_width(1.);
        cr->set_source_rgb(1., 0, 0);
        cr->move_to(0, 0);
        cr->line_to(0, height);
        cr->stroke();
    }

    return true;
}

bool OPlayHead::calc_new_pos(daw_time* dt, int millis) {
    gint pos = (millis - dt->m_viewstart) * dt->scale;    
    if (pos == m_last_pos)
        return false;
    m_last_pos = pos;
    return true;
}

void OPlayHead::set_x_pos(int pos) {
    
    if (m_last_pos < 0) {
        set_active(false);
        
    } else if (m_last_pos < 0x7fff) {
        
        set_active(true);
        
        set_margin_start(160 + m_last_pos);
    }
}