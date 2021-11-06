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

#ifndef SRC_OTIMELINE_H_
#define SRC_OTIMELINE_H_

#include <gtkmm.h>
#include <gdkmm/event.h>

#include "OCustomWidget.h"

#include "IOTimeView.h"
#include "OTypes.h"

#include "IOMainWnd.h"

class OTimeDraw : public OCustomWidget {
public:
    OTimeDraw(IOMainWnd*);
    virtual ~OTimeDraw();

    void SetMaxMillis(gint);

    void EnableZoom(bool val);

    void SetLoopStart();    
    int GetLoopStart();
    void SetLoopEnd();
    int GetLoopEnd();
    void SetZoomLoop();
    
    void SetSignalZoomChange(IOTimeView *t);
    void SetSignalPosChange(IOTimeView *t);

    int GetClickMillis();
    void GetMillisString(int, char*);
    
    void SetDawTime(daw_time*);
    void SetRange(daw_range*);

protected:

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    virtual void draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
            int rectangle_width, int rectangle_height, char* text);
    virtual bool on_button_press_event(GdkEventButton* event) override;


private:

    bool m_zoom = false;
    int m_view_width = 0;
    int m_click_millis = 0;
    
    IOMainWnd* m_mainWnd = nullptr;
    daw_time* m_daw_time = nullptr;;
    daw_range* m_range = nullptr;

    Gtk::Menu menu_popup;
    Gtk::MenuItem menu_popup_start;
    Gtk::MenuItem menu_popup_end;

    sigc::signal<void> signal_zoom_changed;
    sigc::signal<void> signal_pos_changed;
    
    void on_menu_popup_start();
    void on_menu_popup_end();
};

#endif /* SRC_OTIMELINE_H_ */
