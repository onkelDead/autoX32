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
#include "IOTracksLayout.h"

typedef struct trackview_entry {
    trackview_entry* prev;
    OTrackView* item;
    trackview_entry* next;
    
} trackview_entry;

class OTracksLayout : public Gtk::VBox, public IOTracksLayout {
public:
    OTracksLayout();
    virtual ~OTracksLayout();

    void AddTrack(OTrackView *v);
    OTrackView* GetTrackview(std::string);

    void RemoveTrackView(std::string path);
    void RemoveAllTackViews();

    void TrackUp(std::string path);
    void TrackDown(std::string path);
    
    void SetPos();

    void StopRecord();
    void StopTeach();

    void redraw();
    
    virtual gint GetTrackIndex(std::string path);

private:
    
    trackview_entry* new_entry();
    void append_entry(trackview_entry* entry);
    void swap_tracks(trackview_entry* t1, trackview_entry* t2);
    
    trackview_entry* m_tracklist = nullptr;
    Gtk::Grid m_grig;
    Gtk::Box m_bbox;
};



#endif /* SRC_OTRACKSLAYOUT_H_ */
