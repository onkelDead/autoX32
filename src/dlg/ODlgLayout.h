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

#ifndef ODLGLAYOUT_H
#define ODLGLAYOUT_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include "IOTracksLayout.h"
#include "IOTrackView.h"


class ODlgLayoutTrack : public Gtk::Box {
public:
    ODlgLayoutTrack(IOTracksLayout* parent, IOTrackView* view);    
    virtual ~ODlgLayoutTrack();
    
    void SetCollapsed(bool colapse);
    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;  
    
private:
    Gtk::Label* m_label;
    Gtk::Label* m_name;
    Gtk::CheckButton* m_check;
    Gtk::CheckButton* m_expand;
};

class ODlgLayout : public Gtk::Dialog {
public:
    ODlgLayout(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ODlgLayout();

    void on_btn_ok_clicked();
    void on_btn_collapse();
    void on_btn_expand();
    
    void AddTrack(IOTracksLayout* parent, IOTrackView* view);
    
    bool m_result;
    
protected:
    Glib::RefPtr<Gtk::CssProvider> m_refCssProvider;    
    Glib::RefPtr<Gtk::Builder> ui;
    
private:
    Gtk::Button* m_btn_ok;    
    Gtk::Button* m_btn_collapse;    
    Gtk::Button* m_btn_expand;    
    Gtk::Box* m_view;
    
    std::map<std::string, ODlgLayoutTrack*> m_expanders;

};


#endif /* ODLGLAYOUT_H */

