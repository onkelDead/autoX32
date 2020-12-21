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

#include "OTrackEditDlg.h"

OTrackEditDlg::OTrackEditDlg() {
	m_button_ok = add_button("_Ok", 1);
	m_button_ok->set_sensitive(false);
	m_button_cancel = add_button("_Cancel", 0);
	m_button_ok->signal_clicked().connect(sigc::mem_fun(*this, &OTrackEditDlg::on_button_ok_clicked));
	m_button_cancel->signal_clicked().connect(sigc::mem_fun(*this, &OTrackEditDlg::on_button_cancel_clicked));

	Gtk::Box *vbox = get_vbox();
	m_label_oscpath.set_text("OSC path");
	m_grid.attach(m_label_oscpath, 0, 0, 1, 1);
	m_grid.attach(m_entry_name, 1, 0, 1, 1);
	m_grid.attach(m_entry_knownoscpath, 1, 1, 1, 1);
	m_entry_knownoscpath.signal_changed().connect(sigc::mem_fun(this, &OTrackEditDlg::on_combo_control_changed));
	vbox->pack_start(m_grid, true, true);
	show_all_children(true);
}

OTrackEditDlg::~OTrackEditDlg() {
}

void OTrackEditDlg::on_entry_changed() {
//	if (m_entry_oscpath.get_text().length() > 0) {
//		m_button_ok->set_sensitive(true);
//	}
//	else {
//		m_button_ok->set_sensitive(false);
//	}
}

void OTrackEditDlg::on_button_ok_clicked() {
	m_track->SetPathStr(strdup(m_entry_knownoscpath.get_active_text().c_str()));
	m_result = 1;
	hide();
}

void OTrackEditDlg::on_combo_control_changed() {
//	m_entry_oscpath.set_text(m_entry_knownoscpath.get_active_text());
    m_button_ok->set_sensitive(true);
}

void OTrackEditDlg::on_button_cancel_clicked() {
	m_result = 0;
	hide();
}

void OTrackEditDlg::AddKnownCmd(std::string entry) {
	m_entry_knownoscpath.append(entry);
}

void OTrackEditDlg::ClearKnownCmds() {
	m_entry_knownoscpath.remove_all();
}

int OTrackEditDlg::GetResult() {
	return m_result;
}

void OTrackEditDlg::SetTrack(OscCmd *track) {
	m_track = track;
//	m_entry_oscpath.set_text(track->m_path);
}

std::string OTrackEditDlg::GetPath() {
	return m_entry_knownoscpath.get_active_text();
}

std::string OTrackEditDlg::GetName() {
	return m_entry_name.get_text();
}
