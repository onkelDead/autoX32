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

#include "OTimeDraw.h"
#include "OTypes.h"

OTimeDraw::OTimeDraw() : m_daw_time(0){






    m_zoom = false;

    set_has_window(true);
    set_name("o-timeline");
    m_scroll_step = 5;

    menu_popup_start.set_label("Set loop start");
    menu_popup.append(menu_popup_start);
    menu_popup_end.set_label("Set loop end");
    menu_popup.append(menu_popup_end);

    menu_popup_start.signal_activate().connect(sigc::mem_fun(this, &OTimeDraw::on_menu_popup_start));
    menu_popup_end.signal_activate().connect(sigc::mem_fun(this, &OTimeDraw::on_menu_popup_end));

}

OTimeDraw::~OTimeDraw() {

}

Gtk::SizeRequestMode OTimeDraw::get_request_mode_vfunc() const {
    return Gtk::Widget::get_request_mode_vfunc();
}

void OTimeDraw::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OTimeDraw::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OTimeDraw::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const {
    minimum_height = 30;
    natural_height = 30;
}

void OTimeDraw::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const {
    minimum_width = 30;
    natural_width = 30;
}

void OTimeDraw::on_size_allocate(Gtk::Allocation& allocation) {
    set_allocation(allocation);

    if (m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height());
    }
}

void OTimeDraw::on_map() {
    Gtk::Widget::on_map();
}

void OTimeDraw::on_unmap() {
    Gtk::Widget::on_unmap();
}

void OTimeDraw::on_realize() {
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

void OTimeDraw::on_unrealize() {
    m_refGdkWindow.reset();

    Gtk::Widget::on_unrealize();
}

bool OTimeDraw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    const Gtk::Allocation allocation = get_allocation();
    auto refStyleContext = get_style_context();

    int height = allocation.get_height();
    int width = allocation.get_width();

    m_daw_time->scale = (gfloat) width / (gfloat) (m_daw_time->m_viewend - m_daw_time->m_viewstart);
    gint pos = (m_daw_time->m_pos - m_daw_time->m_viewstart) * m_daw_time->scale;

//    //draw_text(cr, allocation.get_width(), 8, (char*)"Hello");
//    cr->set_source_rgb(.6, 0, 0);
//    cr->move_to(pos, 0);
//    cr->line_to(pos, height);
//    cr->stroke();

    pos = (m_range->m_loopstart - m_daw_time->m_viewstart) * m_daw_time->scale;
    cr->set_source_rgb(.0, .8, 0);
    cr->move_to(pos, height / 2);
    cr->line_to(pos, height);
    cr->stroke();


    pos = (m_range->m_loopend - m_daw_time->m_viewstart) * m_daw_time->scale;
    cr->set_source_rgb(.4, .4, 1);
    cr->move_to(pos, height / 2);
    cr->line_to(pos, height);
    cr->stroke();

    return true;
}

void OTimeDraw::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
        int rectangle_width, int rectangle_height, char* text) {
    Pango::FontDescription font;

    font.set_size(7 * Pango::SCALE);
    font.set_family("Sans");
    font.set_weight(Pango::WEIGHT_NORMAL);

    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    int text_width;
    int text_height;

    layout->get_pixel_size(text_width, text_height);

    cr->set_source_rgb(.8, .8, .8);
    cr->move_to((rectangle_width - text_width) / 2, rectangle_height);

    layout->show_in_cairo_context(cr);
}

void OTimeDraw::set_samples(gint new_val) {
    if (new_val > m_daw_time->m_maxsamples) {
        SetMaxSamples(new_val);
    }
    m_daw_time->m_pos = new_val;
    if (m_daw_time->m_pos < 0)
        m_daw_time->m_pos = 0;
    queue_draw();

}

void OTimeDraw::SetMaxSamples(gint max_samples) {
    m_daw_time->m_maxsamples = max_samples;
    if (m_range->m_loopend == -1) {
        m_range->m_loopend = max_samples;
    }
    m_daw_time->m_viewend = max_samples;
}

bool OTimeDraw::on_button_press_event(GdkEventButton* event) {
    if (event->button == 3) {
        menu_popup.show_all();
        menu_popup.popup(3, event->time);
        return true;
    }
    m_daw_time->m_pos = event->x / m_daw_time->scale + m_daw_time->m_viewstart;
    signal_pos_changed.emit();
    return true;
}

bool OTimeDraw::on_scroll_event(GdkEventScroll* event) {
    if (m_zoom) {
        if (event->direction == GDK_SCROLL_UP) { // zoom in
            zoom_in();
        } else { // zoom out
            zoom_out();
        }
        return true;
    }
    m_daw_time->m_pos += (event->direction == GDK_SCROLL_UP ? (m_scroll_step * m_daw_time->m_bitrate) : -(m_scroll_step * m_daw_time->m_bitrate));
    if (m_daw_time->m_pos < 0)
        m_daw_time->m_pos = 0;
    if (m_daw_time->m_pos > m_daw_time->m_maxsamples)
        m_daw_time->m_pos = m_daw_time->m_maxsamples;
    signal_pos_changed.emit();
    return true;
}

void OTimeDraw::on_menu_popup_start() {
    m_range->m_loopstart = m_daw_time->m_pos;
    m_range->m_dirty = true;
    queue_draw();
}

void OTimeDraw::on_menu_popup_end() {
    m_range->m_loopend = m_daw_time->m_pos;
    m_range->m_dirty = true;
    queue_draw();
}

void OTimeDraw::EnableZoom(bool val) {
    m_zoom = val;
}

void OTimeDraw::SetLoopStart() {
    m_range->m_loopstart = m_daw_time->m_pos;
    m_range->m_dirty = true;
    queue_draw();
}

void OTimeDraw::SetLoopEnd() {
    m_range->m_loopend = m_daw_time->m_pos;
    m_range->m_dirty = true;
    queue_draw();
}

void OTimeDraw::SetScrollStep(gint val) {
    m_scroll_step = val;
}

void OTimeDraw::SetDawTime(daw_time* dt) {
    m_daw_time = dt;
}

void OTimeDraw::SetRange(daw_range* range) {
    m_range = range;
}

void OTimeDraw::zoom_in() {
    m_daw_time->m_viewstart += m_daw_time->m_pos / 10;
    m_daw_time->m_viewend -= (m_daw_time->m_viewend - m_daw_time->m_pos) / 10;
    if (m_daw_time->m_viewstart > m_daw_time->m_viewend)
        m_daw_time->m_viewstart = m_daw_time->m_viewend;
    signal_zoom_changed.emit();
}

void OTimeDraw::zoom_out() {
    m_daw_time->m_viewstart -= m_daw_time->m_pos / 10;
    m_daw_time->m_viewend += (m_daw_time->m_viewend - m_daw_time->m_pos) / 10;
    if (m_daw_time->m_viewstart < 0)
        m_daw_time->m_viewstart = 0;
    if (m_daw_time->m_viewend > m_daw_time->m_maxsamples)
        m_daw_time->m_viewend = m_daw_time->m_maxsamples;

    signal_zoom_changed.emit();

}

void OTimeDraw::SetZoomLoop() {
    m_daw_time->m_viewstart = m_range->m_loopstart;
    m_daw_time->m_viewend = m_range->m_loopend;
    signal_zoom_changed.emit();

}

void OTimeDraw::SetSignalZoomChange(IOTimeView* t) {
    signal_zoom_changed.connect(sigc::mem_fun(*t, &IOTimeView::on_timedraw_zoom_changed));
}

void OTimeDraw::SetSignalPosChange(IOTimeView* t) {
    signal_pos_changed.connect(sigc::mem_fun(*t, &IOTimeView::on_timedraw_pos_changed));
}