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

#ifndef ODLGPREFS_H
#define ODLGPREFS_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>

class ODlgPrefs : public Gtk::Dialog {
public:
    ODlgPrefs(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ODlgPrefs();
    
    void on_btn_cancel_clicked();
    void on_btn_ok_clicked();
    
    void SetShowTrackPath(bool);
    bool GetShowTrackPath();
    
    void SetSmoothScreen(bool);
    bool GetSmoothScreen();
    
    void SetTrackFilter(std::string);
    std::string GetTrackFilter();
    
    bool m_result;

protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:
    Gtk::Button* m_btn_cancel;
    Gtk::Button* m_btn_ok;
    
    Gtk::CheckButton* m_chk_showtrackpath;
    Gtk::CheckButton* m_chk_smooth_screen;
    Gtk::Entry* m_track_filter;
};

#endif /* ODLGPREFS_H */

