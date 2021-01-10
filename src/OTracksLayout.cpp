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
    m_label.set_hexpand(true);
    m_label.set_halign(Gtk::ALIGN_FILL);
    m_label.set_vexpand(true);
    m_label.set_valign(Gtk::ALIGN_FILL);
    m_label.set_text("..");
    //m_bbox.add(m_label);
    m_bbox.set_vexpand(true);
    m_bbox.set_border_width(1);
    //add(m_bbox);
}

OTracksLayout::~OTracksLayout() {
}

void OTracksLayout::AddTrack(OTrackView *v) {
    v->set_hexpand(true);
    v->set_halign(Gtk::ALIGN_FILL);
    v->set_valign(Gtk::ALIGN_START);
    add(*v);
    m_trackmap[v->GetCmd()->GetPath()] = v;
    v->show();
}

OTrackView* OTracksLayout::GetTrackview(std::string path) {
    return m_trackmap[path];
}

std::map<std::string, OTrackView *> OTracksLayout::GetTrackMap() {
    return m_trackmap;
}

void OTracksLayout::redraw() {
//    queue_draw();
}


void OTracksLayout::StopRecord() {
    for (std::map<std::string, OTrackView*>::iterator it = m_trackmap.begin(); it != m_trackmap.end(); ++it) {
        if (it->second) {
            it->second->SetRecord(false);
            it->second->SetTouch(false);
        }
    }    
}

void OTracksLayout::RemoveAllTackViews() {
    while(m_trackmap.size() > 0) {
        std::map<std::string, OTrackView*>::iterator it = m_trackmap.begin();
        RemoveTrackView(it->first);
    }
}

void OTracksLayout::RemoveTrackView(std::string path) {
    OTrackView* tv = m_trackmap[path];
    remove(*tv);
    m_trackmap.erase(path);
    delete tv;
    
}



