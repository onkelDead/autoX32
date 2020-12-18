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

#ifndef SRC_OTRACKEDITDLG_H_
#define SRC_OTRACKEDITDLG_H_

#include <gtkmm.h>
#include "OscCmd.h"

class OTrackEditDlg: public Gtk::Dialog {
public:
	OTrackEditDlg();
	virtual ~OTrackEditDlg();

	int GetResult();
	void SetTrack(OscCmd *track);
	std::string GetPath();
	std::string GetName();

	void AddKnownCmd(std::string entry);
	void ClearKnownCmds();
private:
	int m_result;
	OscCmd *m_track;

    Gtk::Button *m_button_ok;
    void on_button_ok_clicked();
    Gtk::Button *m_button_cancel;
    void on_button_cancel_clicked();
    void on_combo_control_changed();
    void on_entry_changed();

    Gtk::Grid m_grid;

    Gtk::Label m_label_oscpath;
    Gtk::Entry m_entry_name;
    Gtk::ComboBoxText m_entry_knownoscpath;
};

#endif /* SRC_OTRACKEDITDLG_H_ */
