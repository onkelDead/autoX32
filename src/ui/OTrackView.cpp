/*
 Copyright 2021 Detlef Urban <onkel@paraair.de>

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

#include <libgen.h>
#include "IOTrackStore.h"
#include "OTrackView.h"
#include "OTrackDlg.h"
#include "res/OResource.h"
#include "OX32.h"

#include "res/trackview.h"
#include "res/trackdlg.h"

OTrackView::OTrackView(IOMainWnd *wnd, daw_time* daw_time) : Gtk::Box(), m_parent(wnd) {
    
    Glib::RefPtr<Gtk::Builder> ui = Gtk::Builder::create_from_string(trackview_inline_glade);
    set_name("OTrackView");

    ui->get_widget<Gtk::Expander>("track-expander", m_expander);
    ui->get_widget<Gtk::Box>("track-box", m_box);
    ui->get_widget<Gtk::Box>("track-control", m_boxcontrol);
    ui->get_widget<Gtk::Box>("box-sizer", m_boxsizer);
    ui->get_widget<Gtk::Label>("track-label", m_label);
    ui->get_widget<Gtk::Toolbar>("track-bar", m_toolbar);
    ui->get_widget<Gtk::ToggleToolButton>("track-rec", m_btn_x32_rec);
    ui->get_widget<Gtk::ToolButton>("track-edit", m_btn_edit);
    ui->get_widget<Gtk::ToolButton>("track-up", m_btn_up);
    ui->get_widget<Gtk::ToolButton>("track-down", m_btn_down);

    m_boxcontrol->set_name("OTrackControl");
    m_toolbar->set_name("OTrackBar");

    m_boxcontrol->set_vexpand(true);
    m_boxcontrol->set_valign(Gtk::ALIGN_FILL);

    m_tracksizer = new OTrackSizer(this);
    m_tracksizer->set_size_request(160, 3);
    m_boxsizer->set_valign(Gtk::ALIGN_END);
    m_boxsizer->set_vexpand(true);
    m_boxsizer->add(*m_tracksizer);

    m_trackdraw = new OTrackDraw(wnd, daw_time);
    m_trackdraw->set_halign(Gtk::ALIGN_FILL);
    m_trackdraw->set_hexpand(true);

    m_btn_x32_rec->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_button_x32_rec_clicked));
    m_btn_x32_rec->show_all();

    m_btn_edit->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_edit));
    m_btn_up->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_up));
    m_btn_down->signal_clicked().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_down));

    m_expander->property_expanded().signal_changed().connect(sigc::mem_fun(*this, &OTrackView::on_expander));

    m_box->add(*m_trackdraw);

    m_box->set_vexpand(true);
    m_box->set_valign(Gtk::ALIGN_FILL);

    menu_popup_rename.set_label("Edit...");
    menu_popup_remove.set_label("Remove...");
    menu_popup_rectoggle.set_label("Toggle record");
    menu_popup_hide.set_label("Hide");
    menu_check_data.set_label("Check Data");
    menu_popup.append(menu_popup_rename);
    menu_popup.append(menu_popup_remove);
    menu_popup.append(menu_popup_rectoggle);
    menu_popup.append(menu_popup_hide);
    menu_popup.append(menu_check_data);

    menu_popup_rename.signal_activate().connect(sigc::mem_fun(this, &OTrackView::on_menu_popup_edit));
    menu_popup_remove.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_remove));
    menu_popup_rectoggle.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_rectoggle));
    menu_popup_hide.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_popup_hide));
    menu_check_data.signal_activate().connect(sigc::mem_fun(*this, &OTrackView::on_menu_check_data));

    set_valign(Gtk::ALIGN_FILL);
    add(*m_box);
    show_all_children(true);

}

OTrackView::~OTrackView() {
    m_box->remove(*m_trackdraw);
    delete m_trackdraw;
}

void OTrackView::on_button_x32_rec_clicked() {
    m_btn_x32_rec->show_all();
    m_trackdraw->SetRecord(m_btn_x32_rec->get_active());
    m_parent->PublishUiEvent(E_OPERATION::toggle_recview, this);
}

void OTrackView::SetTrackStore(IOTrackStore *trackstore) {
    m_trackdraw->SetTrackStore(trackstore);
    m_expander->set_expanded(trackstore->GetLayout()->m_expanded);
    SetPath(trackstore->GetPath());
}

IOTrackStore* OTrackView::GetTrackStore() {
    return m_trackdraw->GetTrackStore();
}

//IOscMessage* OTrackView::GetMessage() {
//
//    return m_trackdraw->GetMessage();
//}

void OTrackView::SetTrackName(std::string name) {
    m_label->set_text(name);
}

std::string OTrackView::GetTrackName() {
    return m_label->get_text();
}



void OTrackView::SetTrackColor(int c) {
    
}

void OTrackView::SetRecord(bool val) {
    m_btn_x32_rec->set_active(val);
}

bool OTrackView::GetRecord() {
    return m_btn_x32_rec->get_active();
}

bool OTrackView::on_button_press_event(GdkEventButton *event) {
    if (event->button == 3) {
        menu_popup.show_all();
        menu_popup.popup(3, event->time);
        return true;
    }
    return false;
}

void OTrackView::on_menu_popup_edit() {
    m_parent->EditTrack(m_trackdraw->GetMessage()->GetPath());
}

void OTrackView::on_menu_popup_up() {
    m_parent->TrackViewUp(this->GetPath());
}

void OTrackView::on_menu_popup_down() {
    m_parent->TrackViewDown(this->GetPath());
}

void OTrackView::on_menu_popup_remove() {
    Gtk::MessageDialog dialog("Are you sure to remove this track from project?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
        m_parent->remove_track(GetPath());
    }
}

void OTrackView::on_menu_popup_rectoggle() {
    m_btn_x32_rec->set_active(!m_btn_x32_rec->get_active());
}

void OTrackView::on_menu_popup_hide() {
    m_parent->TrackViewHide(this->GetPath());
}

void OTrackView::on_menu_check_data() {
    IOTrackStore* it = m_trackdraw->GetTrackStore();
    track_entry* entry = it->GetHeadEntry();
    int pos = 0;
    int err_count = 0;
    
    it->CheckData(&pos, &err_count);
    printf ("Checked %d entries, %d errors\n", pos, err_count);
    
    if (err_count > 0) {
        char msg[1024];
        sprintf(msg, "Checked %d entries, %d errors\nShould I delete duplicate entries?", pos, err_count);
        Gtk::MessageDialog dialog(msg,
                false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
        int result = dialog.run();  
        if (result) {
            entry = it->GetHeadEntry();
            while(entry && entry->next) {
                if (entry->time >= entry->next->time) {
                    it->RemoveEntry(entry->next);
                }
                else
                    entry = entry->next;
            }
        }
    }
}

bool OTrackView::on_motion_notify_event(GdkEventMotion *motion_event) {

    if (motion_event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion *e = (GdkEventMotion*) motion_event;
        if (m_in_resize) {
            if (m_last_y != (gint) e->y) {
                IOTrackStore* ts = m_trackdraw->GetTrackStore();
                gint min_height = m_boxcontrol->get_height();
                m_last_y = (gint) e->y;
                ts->GetLayout()->m_height += m_last_y;
                if (ts->GetLayout()->m_height < min_height)
                    ts->GetLayout()->m_height = min_height;
                set_size_request(160, ts->GetLayout()->m_height);
                ts->SetDirty(true);
            }
        }
    }
    return true;
}

void OTrackView::on_expander() {
    if (m_expander->get_expanded()) {
        set_size_request(160, m_trackdraw->GetTrackStore()->GetLayout()->m_height);
    } else {
        set_size_request(160, -1);
    }
    m_trackdraw->GetTrackStore()->GetLayout()->m_expanded = m_expander->get_expanded();

}

void OTrackView::ExpandCollapse(bool expand) {
    m_expander->set_expanded(expand);
    on_expander();
}

void OTrackView::Resize(bool val) {
    m_in_resize = val;
    if (!val) {
        m_last_y = 0;
    }
}

void OTrackView::Reset() {
    SetHeight(80);
    m_trackdraw->GetTrackStore()->GetLayout()->m_visible = true;
    m_expander->set_expanded(true);
    on_expander();  
}

void OTrackView::SetHeight(gint height) {
    IOTrackStore* ts = m_trackdraw->GetTrackStore();
    set_size_request(160, height);
    ts->GetLayout()->m_height = height;
    ts->SetDirty(true);    
}

void OTrackView::SetSelected(bool val) {
    m_trackdraw->SetSelected(val);
    if (val)
        set_name("OTrackView_selected");
    else
        set_name("OTrackView");
}