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

OTimeView::OTimeView() : Gtk::Box(), ui{Gtk::Builder::create_from_string(timeview_inline_glade)}
{
    ui->get_widget < Gtk::Box > ("time-box", m_box);
    ui->get_widget < Gtk::Label > ("o-timecode", m_timecode);
    ui->get_widget < Gtk::Label > ("o-viewstart", m_viewstart);
    ui->get_widget < Gtk::Label > ("o-viewend", m_viewend);

    m_timedraw = new OTimeDraw();
    m_timedraw->set_hexpand(true);
    m_timedraw->set_halign(Gtk::ALIGN_FILL);
    m_box->add(*m_timedraw);

    m_timedraw->SetSignalPosChange(this);
    m_timedraw->SetSignalZoomChange(this);

    add(*m_box);
}

OTimeView::OTimeView(const OTimeView& orig) {
}

OTimeView::~OTimeView() {
}

void OTimeView::SetRange(daw_range* range) {
    m_timedraw->SetRange(range);
}

void OTimeView::SetDawTime(daw_time* dt) {
    m_daw_time = dt;
    m_timedraw->SetDawTime(dt);
}

void OTimeView::SetTimer(IOTimer* timer) {
    m_timedraw->SetTimer(timer);
}


void OTimeView::UpdateDawTime(bool redraw) {
    char t[32];
    daw_time* dt = m_daw_time;
    
    float fmillis = ((float)dt->m_viewstart / (float)dt->m_bitrate) * 1000 ;
    int millis = (int)(fmillis / 10) % 100;
    int sec = (int)(fmillis / 1000) % 60;
    int min = (int)(fmillis / 60000) % 60;
    int hour = (int(fmillis / 3600000));
    sprintf(t, "%02d:%02d:%02d:%02d", hour, min, sec, millis);
    m_viewstart->set_text(t);
    
    fmillis = ((float)dt->m_viewend / (float)dt->m_bitrate) * 1000 ;
    millis = (int)(fmillis / 10) % 100;
    sec = (int)(fmillis / 1000) % 60;
    min = (int)(fmillis / 60000) % 60;
    hour = (int(fmillis / 3600000));
    sprintf(t, "%02d:%02d:%02d:%02d", hour, min, sec, millis);
    m_viewend->set_text(t);
    
    if (redraw)
        m_timedraw->queue_draw();
}

void OTimeView::SetScrollStep(int step) {
    m_timedraw->SetScrollStep(step);
}

void OTimeView::EnableZoom(bool val) {
    m_timedraw->EnableZoom(val);
}

void OTimeView::SetZoomLoop() {
    m_timedraw->SetZoomLoop();
    UpdateDawTime(true);
}


void OTimeView::SetLoopStart() {
    m_timedraw->SetLoopStart();
}

void OTimeView::SetLoopEnd() {
    m_timedraw->SetLoopEnd();
}

void OTimeView::on_timedraw_pos_changed() {
    signal_pos_changed.emit();
}

void OTimeView::on_timedraw_zoom_changed() {
    UpdateDawTime(false);
    m_timedraw->queue_draw();
    signal_zoom_changed.emit();
}