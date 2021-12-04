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

#include <gtkmm.h>
#include "OTracksLayout.h"
#include "OMainWnd.h"

OTracksLayout::OTracksLayout() : Gtk::VBox() {
    m_bbox.set_vexpand(true);
    m_bbox.set_border_width(1);
}

OTracksLayout::~OTracksLayout() {
}

void OTracksLayout::AddTrack(OTrackView *v) {
    v->set_hexpand(true);
    v->set_halign(Gtk::ALIGN_FILL);
    v->set_valign(Gtk::ALIGN_START);
    trackview_entry* entry = new_entry();
    entry->item = v;
    append_entry(entry);
    //m_trackmap[v->GetCmd()->GetPath()] = v;
    add(*v);
    v->show();
}

OTrackView* OTracksLayout::GetTrackview(std::string path) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return nullptr;
    }
    while(list != nullptr) {
        if (list->item->GetCmd()->GetPath() == path)
            return list->item;
        list = list->next;
    }
    return nullptr;
    
}


void OTracksLayout::redraw() {
    //    queue_draw();
}

void OTracksLayout::StopRecord() {
    trackview_entry* list = m_tracklist;
    while(list) {
        list->item->SetRecord(false);
        list = list->next;
    }
}
void OTracksLayout::StopTeach() {
    StopRecord();
}

void OTracksLayout::RemoveAllTackViews() {
    trackview_entry* list = m_tracklist;
    while(list) {
        remove(*list->item);
        trackview_entry* d = list;
        list->prev = nullptr;
        list = list->next;
        delete d;
    }
    m_tracklist = nullptr;
}

void OTracksLayout::RemoveTrackView(std::string path) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list->next != nullptr) {
        if (list->item->GetCmd()->GetPath() == path) {
            trackview_entry* d = list;
            if (list->next)
                list->next->prev = list->prev;
            if (list->prev)
                list->prev->next = list->next;
            remove (*d->item);
            delete d;
        }
    }
}

void OTracksLayout::TrackUp(std::string path) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        if (list->item->GetCmd()->GetPath().compare(path) == 0) {
            if (list->prev) {
                trackview_entry* tt = list->prev;
                while(tt) {
                    remove(*tt->item);
                    tt = tt->next;
                }
                swap_tracks(list->prev, list);
                tt = list->prev;
                while(tt) {
                    add(*tt->item);
                    tt = tt->next;
                }
                break;
            }
            
        }
        list = list->next;
    }
    
}

void OTracksLayout::TrackDown(std::string path) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list->next != nullptr) {
        if (list->item->GetCmd()->GetPath().compare(path) == 0) {
            if (list->next) {
                trackview_entry* tt = list;
                while(tt) {
                    remove(*tt->item);
                    tt = tt->next;
                }
                swap_tracks(list, list->next);
                tt = list;
                while(tt) {
                    add(*tt->item);
                    tt = tt->next;
                }
                break;
            }
            
        }
        list = list->next;
    }
    
}
void OTracksLayout::swap_tracks(trackview_entry* t1, trackview_entry* t2) {
    OTrackView* tmp = t1->item;
    t1->item = t2->item;
    t2->item = tmp;
}

trackview_entry* OTracksLayout::new_entry() {
    trackview_entry* entry = new trackview_entry;
    entry->prev = nullptr;
    entry->next = nullptr;
    return entry;
}

void OTracksLayout::append_entry(trackview_entry* entry) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        m_tracklist = entry;
        return;
    }
        
    while( list->next) {
        list = list->next;
    }
    list->next = entry;
    entry->prev = list;
}


