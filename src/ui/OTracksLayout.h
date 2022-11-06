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

class OTracksLayout : public Gtk::VBox, public IOTracksLayout {
public:
    OTracksLayout();
    virtual ~OTracksLayout();

    void AddTrack(OTrackView *v, bool show);
    OTrackView* GetTrackview(std::string);
    OTrackView* GetTrackHead();
    OTrackView* GetTrackTail();
    
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
    
    std::string GetSelectedTrackName();
    float GetSelectedTrackValue();
    OTrackView* GetSelectedTrackView();     
    
    std::string GetNextTrackPath();
    void SelectPrevTrack();
    
    void SelectTrack(std::string path);
    void UnselectTrack();
    
    void EditLayout();
    void on_hide_toggle(IOTrackView* view, Gtk::CheckButton* check);
    void on_expand_toggle(IOTrackView* view, Gtk::CheckButton* check);
    
    void ExpandCollapseAll(bool expand);
    void ResetAll();
    void FitView(gint);
    
private:
    
    OTrackView* new_entry();
    void append_entry(OTrackView* entry);
    void swap_tracks(OTrackView* t1, OTrackView* t2);
    gint get_count_visible();
    
    std::vector<OTrackView*> m_tracklist;
    Gtk::Grid m_grig;
    Gtk::Box m_bbox;
    IOTrackStore* m_selected_track = nullptr;
};



#endif /* SRC_OTRACKSLAYOUT_H_ */
