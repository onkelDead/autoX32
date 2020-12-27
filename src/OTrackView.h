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

#ifndef OTRACKVIEW_H
#define OTRACKVIEW_H

#include <gtkmm.h>
#include <gtkmm/widget.h>

#include "OscCmd.h"

#include "OTimeDraw.h"
#include "OTrackDraw.h"
#include "OTrackStore.h"
#include "IOMainWnd.h"
#include "IOTrackView.h"

class OTrackView : public Gtk::Box, public IOTrackView {
public:
    OTrackView(IOMainWnd*);
    virtual ~OTrackView();

    OscCmd* GetCmd();

    void SetDawTime(daw_time*);
    void SetTrackStore(OTrackStore*);

    void BindRemove(IOMainWnd*);
    
    void on_button_x32_rec_clicked();
    void on_button_x32_touch_clicked();
    
    void SetRecord(bool);
    bool GetRecord();
    
    bool GetTouch();
    void SetTouch(bool);

    void UpdateConfig();
    
protected:
    Glib::RefPtr<Gtk::Builder> ui;

private:
    
    IOMainWnd *m_parent;
    
    Gtk::Box *m_box;
    Gtk::Box *m_boxcontrol;
    Gtk::Toolbar *m_toolbar;
    Gtk::Label *m_label;
    OTrackDraw* m_trackdraw;
    
    Gtk::ToggleToolButton *m_btn_x32_rec;
    Gtk::ToggleToolButton *m_btn_x32_touch;
    Gtk::Image m_img_rec_off;
    Gtk::Image m_img_rec_on;
    Gtk::Image m_img_touch_off;
    Gtk::Image m_img_touch_on;

    virtual bool on_button_press_event(GdkEventButton* event) override;
    void on_menu_popup_edit();
    void on_menu_popup_remove();
    
    Gtk::Menu menu_popup;
    Gtk::MenuItem menu_popup_rename;
    Gtk::MenuItem menu_popup_remove;
    
};

#endif /* OTRACKVIEW_H */

