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

#include "OCustomWidget.h"
#include "OTrackDrawBase.h"
#include "OTimeDraw.h"
#include "IOTrackStore.h"
#include "IOMainWnd.h"

class OTrackDraw : public OTrackDrawBase {
public:
    OTrackDraw(IOMainWnd*, daw_time*);
    virtual ~OTrackDraw();

    void SetRecord(bool val);

    void SetSelected(bool);

    bool GetSelected() {
        return m_selected;
    }

protected:

    //Overrides:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);

    virtual bool on_motion_notify_event(GdkEventMotion *motion_event);
    virtual bool on_button_press_event(GdkEventButton *event);
    virtual bool on_button_release_event(GdkEventButton *event);

private:

    Glib::RefPtr<Gdk::Cursor> m_default_cursor;
    Glib::RefPtr<Gdk::Cursor> m_shift_cursor;
    Glib::RefPtr<Gdk::Cursor> m_zoom_cursor;
    Gdk::CursorType m_current_cursor;

    IOMainWnd *m_parent;

    daw_time *m_daw_time;


    bool m_in_drag = 0;
    guint m_btn_down = 0;
    gint m_last_x = 0;
    gint m_down_x = 0;
    gint m_left = 0;
    gint m_right = 0;
    gint m_width = 0;

    bool m_selected = false;

};

#endif /* SRC_OTRACKVIEW_H_ */
