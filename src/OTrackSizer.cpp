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

#include "OTrackSizer.h"

OTrackSizer::OTrackSizer(IOTrackView *parent) :
		m_parent(parent) {
}

OTrackSizer::~OTrackSizer() {
}

void OTrackSizer::on_realize() {
	OCustomWidget::on_realize();
	m_default_cursor = Gdk::Cursor::create(Gdk::CursorType::SB_V_DOUBLE_ARROW);

	m_refGdkWindow.get()->set_cursor(m_default_cursor);
}

bool OTrackSizer::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
	const Gtk::Allocation allocation = get_allocation();
	auto refStyleContext = get_style_context();

	return true;
}

bool OTrackSizer::on_button_press_event(GdkEventButton *event) {
	if (event->button == 1) {
		m_parent->Resize(true);
	}

	if (event->button == 1 && event->type == GdkEventType::GDK_2BUTTON_PRESS)
		printf("double\n");
	return false;
}

bool OTrackSizer::on_button_release_event(GdkEventButton *event) {
	if (event->button == 1) {
		m_parent->Resize(false);
	}
	return false;
}

