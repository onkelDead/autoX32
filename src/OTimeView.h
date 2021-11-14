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

#ifndef OTIMEVIEW_H
#define OTIMEVIEW_H

#include <gtkmm.h>
#include <gtkmm/widget.h>

#include "OTimeDraw.h"
#include "IOTimeView.h"

#include "IOMainWnd.h"

class OTimeView : public Gtk::Box, IOTimeView {
public:
    OTimeView(IOMainWnd*);
    virtual ~OTimeView();

    void SetRange(daw_range* range);
    void SetDawTime(daw_time*);
    void UpdateDawTime(bool redraw);

    void EnableZoom(bool);
    void SetZoomLoop();
    void SetLoopStart();
    int GetLoopStart();
    void SetLoopEnd();
    int GetLoopEnd();
    void SetTimeCode(std::string);
    int GetClickMillis();
    void ScaleView();
        

    sigc::signal<void> signal_zoom_changed;
    sigc::signal<void> signal_pos_changed;
    sigc::signal<void> signal_range_changed;

    virtual void on_timedraw_pos_changed();    
    virtual void on_timedraw_zoom_changed();    
    
protected:
    Glib::RefPtr<Gtk::Builder> ui;
private:

    Gtk::Box *m_box;
    OTimeDraw *m_timedraw = nullptr;
    daw_time* m_daw_time;
    daw_range* m_range = nullptr;
    IOMainWnd* m_mainWnd = nullptr;



    Gtk::Label *m_timecode = nullptr;
    Gtk::Label *m_viewstart = nullptr;
    Gtk::Label *m_viewend = nullptr;

};

#endif /* OTIMEVIEW_H */

