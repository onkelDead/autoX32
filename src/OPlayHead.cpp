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

#include "OPlayHead.h"

OPlayHead::OPlayHead() {
}

OPlayHead::OPlayHead(const OPlayHead& orig) {
}

OPlayHead::~OPlayHead() {
}


Gtk::SizeRequestMode OPlayHead::get_request_mode_vfunc() const {
    return Gtk::Widget::get_request_mode_vfunc();
}

void OPlayHead::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OPlayHead::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OPlayHead::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OPlayHead::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
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
    const Gtk::Allocation allocation = get_allocation();
    auto refStyleContext = get_style_context();

    int height = allocation.get_height();
    int width = allocation.get_width();

//    m_daw_time->scale = (gfloat) width / (gfloat) (m_daw_time->m_viewend - m_daw_time->m_viewstart);
//    gint pos = (m_daw_time->m_pos - m_daw_time->m_viewstart) * m_daw_time->scale;
//
//    //draw_text(cr, allocation.get_width(), 8, (char*)"Hello");
    cr->set_source_rgb(1., 1., 1.);
    cr->move_to(5, 0);
    cr->line_to(5, height);
    cr->stroke();
//
//    pos = (m_range->m_loopstart - m_daw_time->m_viewstart) * m_daw_time->scale;
//    cr->set_source_rgb(.0, .8, 0);
//    cr->move_to(pos, height / 2);
//    cr->line_to(pos, height);
//    cr->stroke();
//
//
//    pos = (m_range->m_loopend - m_daw_time->m_viewstart) * m_daw_time->scale;
//    cr->set_source_rgb(.4, .4, 1);
//    cr->move_to(pos, height / 2);
//    cr->line_to(pos, height);
//    cr->stroke();

    return true;
}
