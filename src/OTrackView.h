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
#include "OTrackSizer.h"
#include "OTrackDraw.h"
#include "OTrackStore.h"
#include "IOMainWnd.h"
#include "IOTrackView.h"

class OTrackView : public Gtk::Box, public IOTrackView {
public:
    OTrackView(IOMainWnd*, daw_time*);
    virtual ~OTrackView();

    OscCmd* GetCmd();

    void SetTrackStore(OTrackStore*);
    OTrackStore* GetTrackStore();

    virtual void Resize(bool);

    void on_button_x32_rec_clicked();
    
    void SetRecord(bool);
    bool GetRecord();

    void UpdateConfig();
    
    void Expand();
    void Collapse();
    
protected:
    Glib::RefPtr<Gtk::Builder> ui;
    virtual bool on_motion_notify_event(GdkEventMotion* motion_event) override;

private:
    
    bool m_in_resize = 0;
    int m_last_y;

    IOMainWnd *m_parent;
    
    Gtk::Expander *m_expander;
    Gtk::Box *m_box;
    Gtk::Box *m_boxcontrol;
    Gtk::Box *m_boxsizer;
    Gtk::Box *m_boxdraw;
    Gtk::Toolbar *m_toolbar;
    Gtk::Label *m_label;

    OTrackSizer* m_tracksizer;

    OTrackDraw* m_trackdraw;
    
    Gtk::ToggleToolButton *m_btn_x32_rec;
    Gtk::Image m_img_rec_off;
    Gtk::Image m_img_rec_on;

    Gtk::ToolButton *m_btn_edit;
    Gtk::ToolButton *m_btn_up;
    Gtk::ToolButton *m_btn_down;
    
    void on_expander();

    virtual bool on_button_press_event(GdkEventButton* event) override;
    void on_menu_popup_edit();
    void on_menu_popup_remove();
    void on_menu_popup_rectoggle();
    void on_menu_popup_up();
    void on_menu_popup_down();
    void on_menu_popup_hide();
    
    Gtk::Menu menu_popup;
    Gtk::MenuItem menu_popup_rename;
    Gtk::MenuItem menu_popup_remove;
    Gtk::MenuItem menu_popup_rectoggle;
    Gtk::MenuItem menu_popup_hide;
    
    
};

#endif /* OTRACKVIEW_H */

