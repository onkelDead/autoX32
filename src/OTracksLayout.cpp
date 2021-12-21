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
#include "embedded/main.h"
#include "embedded/autoX32_css.h"

OTracksLayout::OTracksLayout() : Gtk::VBox() {
    m_bbox.set_vexpand(true);
    m_bbox.set_border_width(1);
}

OTracksLayout::~OTracksLayout() {
}

void OTracksLayout::AddTrack(OTrackView *v, bool show) {
    v->set_hexpand(true);
    v->set_halign(Gtk::ALIGN_FILL);
    v->set_valign(Gtk::ALIGN_START);
    trackview_entry* entry = new_entry();
    entry->item = v;
    append_entry(entry);
    //m_trackmap[v->GetCmd()->GetPath()] = v;
    if (show) {
        add(*v);
        v->show();
    }
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
    queue_draw();
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

    while(list != nullptr) {
        if (list->item->GetCmd()->GetPath() == path) {
            trackview_entry* d = list;
            if (list->next)
                list->next->prev = list->prev;
            if (list->prev)
                list->prev->next = list->next;
            remove (*d->item);
            delete d;
            list = list->next;
        }
    }
}

gint OTracksLayout::GetTrackIndex(std::string path) {
    trackview_entry* list = m_tracklist;
    gint index = 0;
    if (list == nullptr) {
        return -1;
    }
    while(list != nullptr) {
        if (list->item->GetCmd()->GetPath().compare(path) == 0) {
            return index;
        }
        list = list->next;
        index++;
    }
    return -1;
}

void OTracksLayout::EditLayout() {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(main_inline_glade);
    ODlgLayout *pDialog = nullptr;
    builder->get_widget_derived("dlg-layout", pDialog);  
            
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        pDialog->AddTrack(this, list->item);
        list = list->next;
    }
    
    pDialog->show_all_children(true);
    pDialog->run();
    
}

void OTracksLayout::on_hide_toggle(IOTrackView* view, Gtk::CheckButton* check) 
{
    TrackHide(view->GetCmd()->GetPath(), check->get_active());
}

void OTracksLayout::on_expand_toggle(IOTrackView* view, Gtk::CheckButton* check)
{
    view->ExpandCollapse(check->get_active());
}

void OTracksLayout::ExpandCollapseAll(bool expand) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        list->item->ExpandCollapse(expand);
        list = list->next;
    }    
}

void OTracksLayout::ResetAll() {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        if (!list->item->GetTrackStore()->GetLayout()->m_visible)
            TrackHide(list->item->GetCmd()->GetPath(), true);
        list->item->Reset();
        list = list->next;
    }        
}

void OTracksLayout::FitView(gint full_size) {
    gint count_visible = get_count_visible();
    if (count_visible == 0)
        return;
    gint req_size = full_size / count_visible;
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        if (list->item->GetTrackStore()->GetLayout()->m_visible) {
            list->item->SetHeight(req_size);
        }
        if (!list->item->GetTrackStore()->GetLayout()->m_expanded) {
            list->item->ExpandCollapse(true);
        }

        list = list->next;
    }    
    show_all_children(true);
}

void OTracksLayout::TrackHide(std::string path, bool hide) {
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return;
    }
    while(list != nullptr) {
        if (list->item->GetCmd()->GetPath().compare(path) == 0) {
            list->item->GetTrackStore()->GetLayout()->m_visible = hide;
            list->item->GetTrackStore()->SetDirty(true);
            if (!hide)
                remove (*list->item);
            else {
                add(*list->item);
                reorder_child(*list->item, list->item->GetTrackStore()->GetLayout()->m_index);
                show_all_children(true);
            }
            return;
        }   
        list = list->next;
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

gint OTracksLayout::get_count_visible() {
    gint c = 0;
    trackview_entry* list = m_tracklist;
    if (list == nullptr) {
        return 0;
    }
        
    while( list) {
        if (list->item->GetTrackStore()->GetLayout()->m_visible)
            c++;
        list = list->next;
    }
    return c;
}
