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

#ifndef SRC_OTRACKVIEW_H_
#define SRC_OTRACKVIEW_H_

#include <set>
#include <gtkmm.h>
#include <gtkmm/widget.h>
#include <lo/lo.h>
#include "OscCmd.h"
#include "OTimeDraw.h"
#include "OTrackStore.h"
#include "IOMainWnd.h"

class OTrackDraw : public Gtk::Widget {
public:
    OTrackDraw(IOMainWnd*);
    virtual ~OTrackDraw();

    void SetTrackStore(OTrackStore*);
    OscCmd* GetCmd();

    void SetRecord(bool val);
    void SetTouch(bool val);

    daw_time* m_daw_time;

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
    virtual void draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
            int rectangle_width, int rectangle_height, std::string text);

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
protected:
    virtual bool on_motion_notify_event(GdkEventMotion* motion_event) override;
    virtual bool on_button_press_event(GdkEventButton* event) override;
    virtual bool on_button_release_event(GdkEventButton* event) override;
private:

    Glib::RefPtr<Gdk::Cursor> m_default_cursor;
    Glib::RefPtr<Gdk::Cursor> m_shift_cursor;
    Glib::RefPtr<Gdk::Cursor> m_zoom_cursor;
    Gdk::CursorType m_current_cursor;

    IOMainWnd* m_parent;
    
    float GetHeight(lo_arg it, char t);
    OTrackStore* m_trackstore;

    bool m_in_drag;
    guint m_btn_down;
    gint m_last_x;
    gint m_left;
    gint m_right;
    gint m_width;
    


};

#endif /* SRC_OTRACKVIEW_H_ */
