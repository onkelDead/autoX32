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

#include "IOTimer.h"

class OTimeDraw : public OCustomWidget {
public:
    OTimeDraw();
    virtual ~OTimeDraw();

    void SetMaxSamples(gint max_samples);

    void EnableZoom(bool val);

    void SetLoopStart();    
    void SetLoopEnd();
    void SetZoomLoop();
    
    void SetSignalZoomChange(IOTimeView *t);
    void SetSignalPosChange(IOTimeView *t);

    int GetClickSamplePos();
    
    void SetScrollStep(gint val);
    
    void SetDawTime(daw_time*);
    void SetTimer(IOTimer*);
    void SetRange(daw_range*);

protected:

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    virtual void draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
            int rectangle_width, int rectangle_height, char* text);


private:

    bool m_zoom;

    gint m_scroll_step;
    
    IOTimer* m_timer;
    
    daw_time* m_daw_time;
    daw_range* m_range;
    
    int m_view_width;

    sigc::signal<void> signal_zoom_changed;
    sigc::signal<void> signal_pos_changed;
    
    int m_click_sample_pos;
    
    virtual bool on_button_press_event(GdkEventButton* event) override;

    void zoom_in();
    void zoom_out();

    void on_menu_popup_start();
    void on_menu_popup_end();
    Gtk::Menu menu_popup;
    Gtk::MenuItem menu_popup_start;
    Gtk::MenuItem menu_popup_end;
};

#endif /* SRC_OTIMELINE_H_ */
