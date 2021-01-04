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

#ifndef OOVERVIEW_H
#define OOVERVIEW_H

#include <gtkmm.h>
#include <gtkmm/widget.h>

#include "OTypes.h"
#include "IOMainWnd.h"

class OOverView : public Gtk::Widget {
public:
    OOverView(IOMainWnd*, daw_time*);
    virtual ~OOverView();


protected:
    //Overrides:
    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    virtual void on_size_allocate(Gtk::Allocation& allocation) override;
    virtual void on_map() override;
    virtual void on_unmap() override;
    virtual void on_realize() override;
    virtual void on_unrealize() override;
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    virtual bool on_motion_notify_event(GdkEventMotion* motion_event) override;
    virtual bool on_button_press_event(GdkEventButton* event) override;
    virtual bool on_button_release_event(GdkEventButton* event) override;

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;

private:
    
    bool m_in_drag = false;
    gint m_width = 0;
    gint m_left = 0;
    gint m_right = 0;
    gdouble m_last_x = 0;

    daw_time* m_daw_time = nullptr;
    IOMainWnd* m_parent = nullptr;
    
    Gdk::CursorType m_current_cursor = Gdk::CursorType::ARROW;
    Glib::RefPtr<Gdk::Cursor> m_default_cursor;
    Glib::RefPtr<Gdk::Cursor> m_left_cursor;
    Glib::RefPtr<Gdk::Cursor> m_right_cursor;
    Glib::RefPtr<Gdk::Cursor> m_shift_cursor;
    
    void UpdateCursor();
    
};

#endif /* OOVERVIEW_H */

