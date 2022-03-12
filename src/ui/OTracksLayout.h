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
#include "ODlgLayout.h"

typedef struct trackview_entry {
    trackview_entry* prev;
    OTrackView* item;
    trackview_entry* next;
    
} trackview_entry;

class OTracksLayout : public Gtk::VBox, public IOTracksLayout {
public:
    OTracksLayout();
    virtual ~OTracksLayout();

    void AddTrack(OTrackView *v, bool show);
    OTrackView* GetTrackview(std::string);
    trackview_entry* GetTrackHead();
    trackview_entry* GetTrackTail();
    trackview_entry* GetTrackSelected();
    
    void RemoveTrackView(std::string path);
    void RemoveAllTackViews();

    void TrackUp(std::string path);
    void TrackDown(std::string path);
    void TrackHide(std::string path, bool hide);
    
    void SetPos();

    void StopRecord();
    void StopTeach();

    void redraw();
    
    virtual gint GetTrackIndex(std::string path);
    void SelectTrack(std::string, bool);
    
    std::string GetSelectedTrackName();
    float GetSelectedTrackValue();
    OTrackView* GetSelectedTrackView();     
    
    void SelectNextTrack();
    std::string GetNextTrack();
    void SelectPrevTrack();
    std::string GetPrevTrack();
    
    void EditLayout();
    void on_hide_toggle(IOTrackView* view, Gtk::CheckButton* check);
    void on_expand_toggle(IOTrackView* view, Gtk::CheckButton* check);
    
    void ExpandCollapseAll(bool expand);
    void ResetAll();
    void FitView(gint);
    
private:
    
    OTrackView* m_selectedView = nullptr;
    trackview_entry* new_entry();
    void append_entry(trackview_entry* entry);
    void swap_tracks(trackview_entry* t1, trackview_entry* t2);
    gint get_count_visible();
    
    trackview_entry* m_tracklist = nullptr;
    Gtk::Grid m_grig;
    Gtk::Box m_bbox;
};



#endif /* SRC_OTRACKSLAYOUT_H_ */