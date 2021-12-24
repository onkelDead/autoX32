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

#ifndef OTRACKDLG_H
#define OTRACKDLG_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>

class OTrackDlg : public Gtk::Dialog {
public:
    OTrackDlg(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~OTrackDlg();

    void on_btn_cancel_clicked();
    void on_btn_ok_clicked();
    
    void SetName(std::string);
    std::string GetName();
    void SetPath(std::string);
    
    Gdk::RGBA GetColor();
    void SetColor(Gdk::RGBA);
    
    void SetCountEntries(gint count);
    
    bool GetResult();

    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:

    bool m_result = false;

    Gtk::Button* m_btn_cancel;
    Gtk::Button* m_btn_ok;
    Gtk::Entry* m_txt_name;
    Gtk::Entry* m_txt_path;
    Gtk::Label* m_count_entries;
    Gtk::ColorButton* m_btn_color;
    
};

#endif /* OTRACKDLG_H */

