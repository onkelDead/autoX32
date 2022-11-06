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
#include "res/main.h"
#include "res/autoX32_css.h"

OTracksLayout::OTracksLayout() : Gtk::VBox() {
    m_bbox.set_vexpand(true);
    m_bbox.set_border_width(1);
}

OTracksLayout::~OTracksLayout() {
}

void OTracksLayout::AddTrack(OTrackView *view, bool show) {
    view->set_hexpand(true);
    view->set_halign(Gtk::ALIGN_FILL);
    view->set_valign(Gtk::ALIGN_START);
    append_entry(view);
    if (show) {
        add(*view);
    }
}

OTrackView* OTracksLayout::GetTrackHead() {
    if (m_tracklist.size() == 0)
        return nullptr;
    return m_tracklist.at(0);
}

OTrackView* OTracksLayout::GetTrackTail() {
    if (m_tracklist.size() == 0)
        return nullptr;
    return m_tracklist.at(m_tracklist.size());
}

OTrackView* OTracksLayout::GetTrackview(std::string path) {
    if (m_tracklist.size() == 0)
        return nullptr;
    for (OTrackView* list : m_tracklist) {
        if (list->GetPath() == path)
            return list;
    }
    return nullptr;
}


void OTracksLayout::redraw() {
    queue_draw();
}

void OTracksLayout::StopRecord() {
    for (OTrackView* list : m_tracklist) { 
        if (list->GetTrackStore()->IsRecording())
            list->SetRecord(false);
    }
}

void OTracksLayout::StopTeach() {
    StopRecord();
}

void OTracksLayout::RemoveAllTackViews() {
    for (OTrackView* list : m_tracklist) { 
        remove(*list);
        delete list;
    }
    m_tracklist.clear();
}

void OTracksLayout::RemoveTrackView(std::string path) {
    int index = 0;
    for (OTrackView* list : m_tracklist) { 
        if (list->GetPath() == path) {
            remove (*list);
            m_tracklist.erase(m_tracklist.begin() + index);
            delete list;
        }
        index++;
    }
}

void OTracksLayout::RemoveTrackView() {
    RemoveTrackView(m_selected_path);
    m_selected_path = "";
}

gint OTracksLayout::GetTrackIndex(std::string path) {
    gint index = 0;
    if (m_tracklist.size() == 0) {
        return -1;
    }
    for (OTrackView* list : m_tracklist) {     
        if (list->GetPath().compare(path) == 0) {
            return index;
        }
        index++;
    }
    return -1;
}


void OTracksLayout::EditLayout() {
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_string(main_inline_glade);
    ODlgLayout *pDialog = nullptr;
    builder->get_widget_derived("dlg-layout", pDialog);  
            
    if (m_tracklist.size() == 0) {
        return;
    }
    for (OTrackView* list : m_tracklist) { 
        pDialog->AddTrack(this, list);
    }
    
    pDialog->show_all_children(true);
    pDialog->run();
    
}

void OTracksLayout::on_hide_toggle(IOTrackView* view, Gtk::CheckButton* check) 
{
    TrackHide(view->GetPath(), check->get_active());
}

void OTracksLayout::on_expand_toggle(IOTrackView* view, Gtk::CheckButton* check)
{
    view->ExpandCollapse(check->get_active());
}

void OTracksLayout::ExpandCollapseAll(bool expand) {
    for (OTrackView* list : m_tracklist) { 
        list->ExpandCollapse(expand);
    }    
}

void OTracksLayout::ResetAll() {
    for (OTrackView* list : m_tracklist) {
        if (!list->GetTrackStore()->GetLayout()->m_visible)
            TrackHide(list->GetPath(), true);
        list->Reset();
    }        
}

void OTracksLayout::FitView(gint full_size) {
    gint count_visible = get_count_visible();
    if (count_visible == 0)
        return;
    gint req_size = full_size / count_visible;

    if (m_tracklist.size() == 0) {
        return;
    }
    for (OTrackView* list : m_tracklist) {
        if (list->GetTrackStore()->GetLayout()->m_visible) {
            list->SetHeight(req_size);
        }
        if (!list->GetTrackStore()->GetLayout()->m_expanded) {
            list->ExpandCollapse(true);
        }
    }    
    show_all_children(true);
}

void OTracksLayout::TrackHide(std::string path, bool hide) {
    for (OTrackView* list : m_tracklist) {
        if (list->GetPath().compare(path) == 0) {
            list->GetTrackStore()->GetLayout()->m_visible = hide;
            list->GetTrackStore()->SetDirty(true);
            if (!hide) {
                remove (*list);
            }
            else {
                add(*list);
                reorder_child(*list, list->GetTrackStore()->GetLayout()->m_index);
                show_all_children(true);
            }
            return;
        }   
    }
}

void OTracksLayout::TrackUp(std::string path) {
    int index = 0;
    for (OTrackView* list : m_tracklist) {
        if (list->GetPath().compare(path) == 0) {
            if (index > 0) {
                int i1 = 0;
                for (OTrackView* m : m_tracklist) {
                    remove(*m);
                    i1++;
                }
                i1 = 0;
                iter_swap(m_tracklist.begin() + index - 1, m_tracklist.begin() + index);
                for (OTrackView* m : m_tracklist) {
                    add(*m);
                    i1++;
                }
                break;
            }
            
        }
        index++;
    }
}

void OTracksLayout::TrackDown(std::string path) {
    size_t index = 0;
    for (OTrackView* list : m_tracklist) {
        if (list->GetPath().compare(path) == 0) {
            if (index < m_tracklist.size() - 1) {
                size_t i1 = 0;
                for (OTrackView* m : m_tracklist) {
                    remove(*m);
                    i1++;
                }
                i1 = 0;
                iter_swap(m_tracklist.begin() + index, m_tracklist.begin() + index + 1);
                for (OTrackView* m : m_tracklist) {
                    add(*m);
                    i1++;
                }
                break;
            }
            
        }
        index++;
    }
    
}
void OTracksLayout::swap_tracks(OTrackView* t1, OTrackView* t2) {
    OTrackView* tmp = t1;
    t1 = t2;
    t2 = tmp;
}

void OTracksLayout::append_entry(OTrackView* entry) {
    m_tracklist.push_back(entry);
}

gint OTracksLayout::get_count_visible() {
    gint c = 0;
    for (OTrackView* list : m_tracklist) {
        if (list->GetTrackStore()->GetLayout()->m_visible)
            c++;
    }
    return c;
}

void OTracksLayout::UnselectTrack() {
    if (m_selected_path != "") {
        GetTrackview(m_selected_path)->SetSelected(false);
        m_selected_path = "";
        
    }
}

void OTracksLayout::SelectTrack(std::string path) {
    IOTrackView *view = GetTrackview(path);
    if (view) {
        m_selected_path = path;
        GetTrackview(path)->SetSelected(true);
    }
}
