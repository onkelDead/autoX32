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

#ifndef SRC_OTRACKSLAYOUT_H_
#define SRC_OTRACKSLAYOUT_H_

#include "OTrackView.h"
#include "IOMainWnd.h"

class OTracksLayout : public Gtk::VBox {
public:
    OTracksLayout();
    virtual ~OTracksLayout();

    void AddTrack(OTrackView *v);
    OTrackView* GetTrackview(std::string);

    void RemoveTrackView(std::string path);
    void RemoveAllTackViews();

    void SetPos();

    void StopRecord();

    void redraw();

    std::map<std::string, OTrackView *> GetTrackMap();

private:
    
    std::map<std::string, OTrackView *> m_trackmap;
    Gtk::Grid m_grig;
    Gtk::Box m_bbox;
    Gtk::Label m_label;
};



#endif /* SRC_OTRACKSLAYOUT_H_ */
