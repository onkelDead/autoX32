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

#include <gtkmm.h>
#include "OTypes.h"
#include "OTimeView.h"

#include "embedded/timeview.h"

OTimeView::OTimeView(IOTimer* timer) :
		Gtk::Box(), m_box(0), ui { Gtk::Builder::create_from_string(
				timeview_inline_glade) }, m_daw_time(0) {

	m_timer = timer;

	ui->get_widget < Gtk::Box > ("time-box", m_box);
	ui->get_widget < Gtk::Label > ("o-timecode", m_timecode);
	ui->get_widget < Gtk::Label > ("o-viewstart", m_viewstart);
	ui->get_widget < Gtk::Label > ("o-viewend", m_viewend);

	m_timedraw = new OTimeDraw(timer);
	m_timedraw->set_hexpand(true);
	m_timedraw->set_halign(Gtk::ALIGN_FILL);
	m_box->add(*m_timedraw);

	m_timedraw->SetSignalPosChange(this);
	m_timedraw->SetSignalZoomChange(this);

	add(*m_box);
}

OTimeView::~OTimeView() {
	if (m_timedraw)
		delete m_timedraw;
}

void OTimeView::SetRange(daw_range *range) {
	m_range = range;
	m_timedraw->SetRange(range);
}

void OTimeView::SetDawTime(daw_time *dt) {
	m_daw_time = dt;
	m_timedraw->SetDawTime(dt);
}

void OTimeView::SetTimeCode(std::string code) {
	m_timecode->set_text(code);
}

void OTimeView::UpdateDawTime(bool redraw) {
	char t[32];

	m_timedraw->GetMillisString(m_daw_time->m_viewstart, t);
	m_viewstart->set_text(t);

	m_timedraw->GetMillisString(m_daw_time->m_viewend, t);
	m_viewend->set_text(t);

//	if (redraw)
//		m_timedraw->queue_draw();
}

void OTimeView::EnableZoom(bool val) {
	m_timedraw->EnableZoom(val);
}

void OTimeView::SetZoomLoop() {
	m_timedraw->SetZoomLoop();
	ScaleView();
}

void OTimeView::SetLoopStart() {
	m_timedraw->SetLoopStart();
}

void OTimeView::SetLoopEnd() {
	m_timedraw->SetLoopEnd();
}

void OTimeView::ScaleView(){
	m_daw_time->scale = (gfloat) m_timedraw->get_width() / (gfloat)(m_daw_time->m_viewend - m_daw_time->m_viewstart);
}

void OTimeView::on_timedraw_pos_changed() {
	signal_pos_changed.emit();
}

void OTimeView::on_timedraw_zoom_changed() {
	UpdateDawTime(false);
	m_timedraw->queue_draw();
	signal_zoom_changed.emit();
}

int OTimeView::GetClickMillis() {
	return m_timedraw->GetClickMillis();
}
