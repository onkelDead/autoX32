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

#ifndef OPLAYHEAD_H
#define OPLAYHEAD_H

#include <gtkmm.h>
#include <gtkmm/widget.h>
#include "OTypes.h"

class OPlayHead : public Gtk::Widget {
public:
    OPlayHead();
    virtual ~OPlayHead();

    void set_active(bool val) { m_active = val; }
    
    bool calc_new_pos(daw_time* dt, int frame);
    void set_x_pos(int pos);
    void SetBorder(int val) {
        m_border = val;
    }
protected:

    //Overrides:
    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    virtual void on_size_allocate(Gtk::Allocation& allocation) override;
    virtual void on_map() override;
    virtual void on_unmap() override;
    virtual void on_realize() override;
    virtual void on_unrealize() override;
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
    
private:
    
    int m_pos;
    int m_last_pos = 0;
    bool m_active = false;
    int m_border = 0;
};

#endif /* OPLAYHEAD_H */

