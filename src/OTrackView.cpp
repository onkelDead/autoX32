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

#include <libgen.h>
#include "OTrackStore.h"
#include "OTrackView.h"
#include "OTrackDlg.h"
#include "OResource.h"
#include "OX32.h"

#include "embedded/trackview.h"
#include "embedded/trackdlg.h"

OTrackView::OTrackView(IOMainWnd *wnd, daw_time* daw_time) :
		Gtk::Box(), ui { Gtk::Builder::create_from_string(trackview_inline_glade) }, m_parent(wnd) {
	set_name("OTrackView");

	ui->get_widget<Gtk::Expander>("track-expander", m_expander);
	ui->get_widget<Gtk::Box>("track-box", m_box);
	ui->get_widget<Gtk::Box>("track-control", m_boxcontrol);
	ui->get_widget<Gtk::Box>("box-sizer", m_boxsizer);
	ui->get_widget<Gtk::Label>("track-label", m_label);
	ui->get_widget<Gtk::Toolbar>("track-bar", m_toolbar);
	ui->get_widget<Gtk::ToggleToolButton>("track-rec", m_btn_x32_rec);

	m_boxcontrol->set_name("OTrackControl");
	m_toolbar->set_name("OTrackControl");

	m_boxcontrol->set_vexpand(true);
	m_boxcontrol->set_valign(Gtk::ALIGN_FILL);

	m_tracksizer = new OTrackSizer(this);
	m_tracksizer->set_size_request(160, 3);
	m_boxsizer->set_valign(Gtk::ALIGN_END);
	m_boxsizer->set_vexpand(true);
	m_boxsizer->add(*m_tracksizer);

	m_trackdraw = new OTrackDraw(wnd, daw_time);
	m_trackdraw->set_halign(Gtk::ALIGN_FILL);
	m_trackdraw->set_hexpand(true);

	m_img_rec_off.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*) rec_off_inline, FALSE));
	m_img_rec_on.set(Gdk::Pixbuf::create_from_inline(-1, (unsigned char*) rec_on_inline, FALSE));
	m_btn_x32_rec->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_button_x32_rec_clicked));
	m_btn_x32_rec->set_icon_widget(m_img_rec_off);
	m_btn_x32_rec->show_all();

	m_expander->property_expanded().signal_changed().connect(sigc::mem_fun(*this, &OTrackView::on_expander));

	m_box->add(*m_trackdraw);

	m_box->set_vexpand(true);
	m_box->set_valign(Gtk::ALIGN_FILL);

	menu_popup_rename.set_label("Edit...");
	menu_popup_remove.set_label("Remove...");
	menu_popup.append(menu_popup_rename);
	menu_popup.append(menu_popup_remove);

	menu_popup_rename.signal_activate().connect(sigc::mem_fun(this, &OTrackView::on_menu_popup_edit));
	menu_popup_remove.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_remove));

	set_valign(Gtk::ALIGN_FILL);
	add(*m_box);
	show_all_children(true);

}

OTrackView::~OTrackView() {
	m_box->remove(*m_trackdraw);
	delete m_trackdraw;
}

void OTrackView::on_button_x32_rec_clicked() {
	m_btn_x32_rec->set_icon_widget(m_btn_x32_rec->get_active() ? m_img_rec_on : m_img_rec_off);
	m_btn_x32_rec->show_all();

	m_trackdraw->SetRecord(m_btn_x32_rec->get_active());

}

void OTrackView::SetTrackStore(OTrackStore *trackstore) {
	m_trackdraw->SetTrackStore(trackstore);
	m_expander->set_expanded(trackstore->m_expanded);
}

OscCmd* OTrackView::GetCmd() {

	return m_trackdraw->GetCmd();
}

void OTrackView::UpdateConfig() {
	m_label->set_text(m_trackdraw->GetCmd()->GetName());
}

void OTrackView::SetRecord(bool val) {
	m_btn_x32_rec->set_active(val);
}

bool OTrackView::GetRecord() {
	return m_btn_x32_rec->get_active();
}

bool OTrackView::on_button_press_event(GdkEventButton *event) {
	if (event->button == 3) {
		menu_popup.show_all();
		menu_popup.popup(3, event->time);
		return true;
	}
	return false;
}

void OTrackView::on_menu_popup_edit() {
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(trackdlg_inline_glade);
	OTrackDlg *pDialog = nullptr;
	builder->get_widget_derived("track-dlg", pDialog);
	pDialog->SetName(m_trackdraw->GetCmd()->GetName());
	pDialog->SetPath(m_trackdraw->GetCmd()->GetPath());
	pDialog->SetColor(m_trackdraw->GetCmd()->GetColor());
	pDialog->run();
	if (pDialog->GetResult()) {
		m_trackdraw->GetCmd()->SetName(pDialog->GetName());
		m_trackdraw->GetCmd()->SetColor(pDialog->GetColor());
		UpdateConfig();
	}
}

void OTrackView::on_menu_popup_remove() {
	Gtk::MessageDialog dialog("Are you sure to remove this track from project?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
		m_parent->remove_track(this);
	}
}

bool OTrackView::on_motion_notify_event(GdkEventMotion *motion_event) {

	if (motion_event->type == GDK_MOTION_NOTIFY) {
		GdkEventMotion *e = (GdkEventMotion*) motion_event;
		if (m_in_resize) {
			int offset;
			if (m_last_y != (gint) e->y) {
				m_last_y = (gint) e->y;
			}
		}
	}
	return true;
}

void OTrackView::on_expander() {
	if (m_expander->get_expanded()) {
		set_size_request(160, m_trackdraw->GetTrackStore()->m_height);
	}
	else {
		set_size_request(160, -1);
	}
	m_trackdraw->GetTrackStore()->m_expanded = m_expander->get_expanded();

}

void OTrackView::Resize(bool val) {
	m_in_resize = val;
	if (!val) {
		m_trackdraw->GetTrackStore()->m_height += m_last_y;
		if (m_trackdraw->GetTrackStore()->m_height < 80)
			m_trackdraw->GetTrackStore()->m_height = 80;
		set_size_request(160, m_trackdraw->GetTrackStore()->m_height);
		m_last_y = 0;
	} else {
		m_trackdraw->GetTrackStore()->m_height = get_height();
	}
}
