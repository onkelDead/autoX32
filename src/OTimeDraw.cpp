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

OTimeDraw::OTimeDraw(IOMainWnd* wnd) {

	set_has_window(true);
	set_name("o-timeline");

	m_mainWnd = wnd;

	menu_popup_start.set_label("Set loop start");
	menu_popup.append(menu_popup_start);
	menu_popup_end.set_label("Set loop end");
	menu_popup.append(menu_popup_end);

	menu_popup_start.signal_activate().connect(
			sigc::mem_fun(this, &OTimeDraw::on_menu_popup_start));
	menu_popup_end.signal_activate().connect(
			sigc::mem_fun(this, &OTimeDraw::on_menu_popup_end));

}

OTimeDraw::~OTimeDraw() {

}

bool OTimeDraw::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
	const Gtk::Allocation allocation = get_allocation();
	auto refStyleContext = get_style_context();

	int height = allocation.get_height();
	m_view_width = allocation.get_width();

	int m_current_time = m_mainWnd->GetPosMillis();

	gint pos = (m_current_time - m_daw_time->m_viewstart) * m_daw_time->scale;

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

	int elems = 100;
	int devider = 1;
	while(elems > 8) {
		devider *= (devider < 120) ? 10 : 6;
		elems = (m_daw_time->m_viewend - m_daw_time->m_viewstart) / devider;
	}

	char t[32];
	for (int i = 0; i < elems;i++) {
		int firstscale = ((devider * i) - (m_daw_time->m_viewstart % devider)) * m_daw_time->scale;
		cr->set_source_rgb(.6, .6, .6);
		cr->move_to(firstscale, height / 2);
		cr->line_to(firstscale, height);
		cr->stroke();

		cr->move_to(firstscale, 0);
		GetMillisString(m_daw_time->m_viewstart + (firstscale / m_daw_time->scale), t);
		draw_text(cr, 120, 30, t);
	}

	return true;
}

void OTimeDraw::draw_text(const Cairo::RefPtr<Cairo::Context> &cr,
		int rectangle_width, int rectangle_height, char *text) {
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
	//cr->move_to((rectangle_width - text_width) / 2, rectangle_height);

	layout->show_in_cairo_context(cr);
}

void OTimeDraw::GetMillisString(int millis, char* t) {
	int mm = millis % 120;
	int sec = (millis / 120) % 60;
	int min = (millis / 7200) % 60;
	int hour = (millis / 432000);
	sprintf(t, "%02d:%02d:%02d:%02d", hour, min, sec, mm);
}

void OTimeDraw::SetMaxMillis(gint max_millis) {
	m_daw_time->m_maxmillis = max_millis;
	if (m_range->m_loopend == -1) {
		m_range->m_loopend = max_millis;
	}
	m_daw_time->m_viewend = max_millis;
}

bool OTimeDraw::on_button_press_event(GdkEventButton *event) {
	if (event->button == 3) {
		menu_popup.show_all();
		menu_popup.popup(3, event->time);
		return true;
	}
	m_click_millis = event->x / m_daw_time->scale + m_daw_time->m_viewstart;
	printf("m_click_millis = %d\n", m_click_millis);
	signal_pos_changed.emit();
	return true;
}

void OTimeDraw::on_menu_popup_start() {
	m_range->m_loopstart = m_mainWnd->GetPosMillis();
	m_range->m_dirty = true;
	queue_draw();
}

void OTimeDraw::on_menu_popup_end() {
	m_range->m_loopend = m_mainWnd->GetPosMillis();
	m_range->m_dirty = true;
	queue_draw();
}

void OTimeDraw::EnableZoom(bool val) {
	m_zoom = val;
}

void OTimeDraw::SetLoopStart() {
	m_range->m_loopstart = m_mainWnd->GetPosMillis();
	m_range->m_dirty = true;
	queue_draw();
}

void OTimeDraw::SetLoopEnd() {
	m_range->m_loopend = m_mainWnd->GetPosMillis();
	m_range->m_dirty = true;
	queue_draw();
}

void OTimeDraw::SetDawTime(daw_time *dt) {
	m_daw_time = dt;
}

void OTimeDraw::SetRange(daw_range *range) {
	m_range = range;
}

void OTimeDraw::SetZoomLoop() {
	m_daw_time->m_viewstart = m_range->m_loopstart;
	m_daw_time->m_viewend = m_range->m_loopend;
	signal_zoom_changed.emit();

}

void OTimeDraw::SetSignalZoomChange(IOTimeView *t) {
	signal_zoom_changed.connect(
			sigc::mem_fun(*t, &IOTimeView::on_timedraw_zoom_changed));
}

void OTimeDraw::SetSignalPosChange(IOTimeView *t) {
	signal_pos_changed.connect(
			sigc::mem_fun(*t, &IOTimeView::on_timedraw_pos_changed));
}

int OTimeDraw::GetClickMillis() {
	return m_click_millis;
}
